#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/SPackageFile.h"
#include "IO/StmData/MemoryData2.h"
#include "Sync/MutexUsage.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

/*
0	Smpf
4	Flags (Bit 0 = Custom Type, Custom Size exist, Bit 1 = V2 header)

V1: (Bit 1 = 0)
8	V1Directory Offset

V2: (Bit 1 = 1)
8	Last V2Directory Offset
16	Last V2Directory Size

Custom Type:
0	Custom Type (1 = OSMTileMap)
4	Custom Size
8	Custom data

Custom Type = 1:
0 = number of URL
1 = size of url 0
2 = URL 0
? = size of url 1

V1Directory:
0	Directory Entry[]

V2Directory:
0	Previous V2Directory Offset, 0 = no previous
8	Previous V2Directory Size
16	Directory Entry[]

Directory Entry:
0	Offset
8	Size
16	Modify Time Ticks
24	File Name Size
26	File Name (UTF-8)

*/

void IO::SPackageFile::ReadV2DirEnt(UInt64 ofst, UInt64 size)
{
	if (ofst == 0 || size < 16)
		return;
	UInt64 nextOfst;
	UInt64 nextSize;
	UInt8 *dirBuff = MemAlloc(UInt8, (UOSInt)size);
	this->stm->SeekFromBeginning(ofst);
	this->stm->Read(dirBuff, (UOSInt)size);
	nextOfst = ReadUInt64(&dirBuff[0]);
	nextSize = ReadUInt64(&dirBuff[8]);
	this->ReadV2DirEnt(nextOfst, nextSize);

	UTF8Char *sbuff;
	UOSInt i;
	UOSInt nameSize;
	FileInfo *file;
	sbuff = MemAlloc(UTF8Char, 512);
	i = 16;
	while (i < size)
	{
		nameSize = ReadUInt16(&dirBuff[i + 24]);
		MemCopyNO(sbuff, &dirBuff[i + 26], nameSize);
		sbuff[nameSize] = 0;
		
		file = this->fileMap->Get(sbuff);
		if (file == 0)
		{
			file = MemAlloc(FileInfo, 1);
			file->ofst = ReadUInt64(&dirBuff[i]);
			file->size = ReadUInt64(&dirBuff[i + 8]);
			this->fileMap->Put(sbuff, file);
		}
		i += 26 + nameSize;
	}
	MemFree(sbuff);
	MemFree(dirBuff);
}

void IO::SPackageFile::AddPackageInner(IO::PackageFile *pkg, UTF8Char pathSeperator, UTF8Char *pathStart, UTF8Char *pathEnd)
{
	UOSInt i = 0;
	UOSInt j = pkg->GetCount();
	UTF8Char *sptr;
	while (i < j)
	{
		sptr = pkg->GetItemName(pathEnd, i);
		IO::PackageFile::PackObjectType pt = pkg->GetItemType(i);
		if (pathEnd[0] == '.')
		{
		}
		else if (pt == IO::PackageFile::POT_PACKAGEFILE)
		{
			IO::PackageFile *innerPack = pkg->GetItemPack(i);
			if (innerPack)
			{
				*sptr++ = pathSeperator;
				AddPackageInner(innerPack, pathSeperator, pathStart, sptr);
				DEL_CLASS(innerPack);
			}
		}
		else if (pt == IO::PackageFile::POT_STREAMDATA)
		{
			IO::IStreamData *fd = pkg->GetItemStmData(i);
			if (fd)
			{
				this->AddFile(fd, pathStart, pkg->GetItemModTimeTick(i));
				DEL_CLASS(fd);
			}
		}
		i++;
	}
}

Bool IO::SPackageFile::OptimizeFileInner(IO::SPackageFile *newFile, UInt64 dirOfst, UInt64 dirSize)
{
	UInt64 lastOfst;
	UInt64 lastSize;
	UOSInt i;
	UOSInt j;
	Bool succ = true;
	UInt8 *dirBuff = MemAlloc(UInt8, (UOSInt)dirSize);
	this->stm->SeekFromBeginning(dirOfst);
	if (this->stm->Read(dirBuff, (UOSInt)dirSize) == dirSize)
	{
		lastOfst = ReadUInt64(&dirBuff[0]);
		lastSize = ReadUInt64(&dirBuff[8]);
		if (lastOfst != 0 && lastSize != 0)
		{
			if (!OptimizeFileInner(newFile, lastOfst, lastSize))
			{
				succ = false;
			}
		}

		UInt64 thisOfst;
		UInt64 thisSize;
		UTF8Char *sbuff = MemAlloc(UTF8Char, 512);
		UInt8 *fileBuff;
		lastOfst = 0;
		lastSize = 0;
		i = 16;
		while (succ && i < dirSize)
		{
			thisOfst = ReadUInt64(&dirBuff[i]);
			thisSize = ReadUInt64(&dirBuff[i + 8]);
			j = ReadUInt16(&dirBuff[i + 24]);
			MemCopyNO(sbuff, &dirBuff[i + 26], j);
			sbuff[j] = 0;
			fileBuff = MemAlloc(UInt8, (UOSInt)thisSize);
			if (thisOfst != lastOfst + lastSize)
			{
				this->stm->SeekFromBeginning(thisOfst);
			}
			if (this->stm->Read(fileBuff, (UOSInt)thisSize) == thisSize)
			{
				newFile->AddFile(fileBuff, (UOSInt)thisSize, sbuff, ReadInt64(&dirBuff[j + 16]));
				lastOfst = thisOfst;
				lastSize = thisSize;
			}
			else
			{
				succ = false;
			}
			MemFree(fileBuff);
			i += 26 + j;
		}
		MemFree(sbuff);
	}
	else
	{
		succ = false;
	}
	MemFree(dirBuff);
	return succ;
}

IO::SPackageFile::SPackageFile(IO::SeekableStream *stm, Bool toRelease)
{
	UInt8 hdr[24];
	this->stm = stm;
	this->toRelease = toRelease;
	NEW_CLASS(this->mstm, IO::MemoryStream(UTF8STRC("IO.SPackageFile.mstm")));
	this->currOfst = 24;
	hdr[0] = 'S';
	hdr[1] = 'm';
	hdr[2] = 'p';
	hdr[3] = 'f';
	WriteInt32(&hdr[4], 2);
	WriteInt64(&hdr[8], 24);
	WriteInt64(&hdr[16], 0);
	this->stm->Write(hdr, 24);
	this->customType = 0;
	this->customSize = 0;
	this->customBuff = 0;
	this->writeMode = true;
	this->flags = 2;
	this->pauseCommit = false;
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->fileMap, Data::BTreeUTF8Map<FileInfo*>());
	WriteInt64(&hdr[0], 0);
	WriteInt64(&hdr[8], 0);
	this->mstm->Write(hdr, 16);
}

IO::SPackageFile::SPackageFile(IO::SeekableStream *stm, Bool toRelease, Int32 customType, UOSInt customSize, const UInt8 *customBuff)
{
	UInt8 hdr[32];
	this->stm = stm;
	this->toRelease = toRelease;
	NEW_CLASS(this->mstm, IO::MemoryStream(UTF8STRC("IO.SPackageFile.mstm")));
	this->currOfst = 32 + customSize;
	hdr[0] = 'S';
	hdr[1] = 'm';
	hdr[2] = 'p';
	hdr[3] = 'f';
	WriteInt32(&hdr[4], 3);
	WriteUInt64(&hdr[8], 32 + customSize);
	WriteInt64(&hdr[16], 0);
	WriteInt32(&hdr[24], customType);
	WriteUInt32(&hdr[28], (UInt32)customSize);
	this->stm->Write(hdr, 32);
	if (customSize > 0)
	{
		this->stm->Write(customBuff, customSize);
	}
	this->flags = 3;
	this->customType = customType;
	this->customSize = customSize;
	if (customSize > 0)
	{
		this->customBuff = MemAlloc(UInt8, customSize);
		MemCopyNO(this->customBuff, customBuff, customSize);
	}
	else
	{
		this->customBuff = 0;
	}
	this->writeMode = true;
	this->pauseCommit = false;
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->fileMap, Data::BTreeUTF8Map<FileInfo*>());
	WriteInt64(&hdr[0], 0);
	WriteInt64(&hdr[8], 0);
	this->mstm->Write(hdr, 16);
}

IO::SPackageFile::SPackageFile(const UTF8Char *fileName)
{
	UInt8 hdr[24];
	UInt64 flength;
	UInt64 dirSize;
	UTF8Char sbuff[512];
	NEW_CLASS(this->stm, IO::FileStream(fileName, IO::FileMode::Append, IO::FileShare::DenyWrite, IO::FileStream::BufferType::NoWriteBuffer));
	this->toRelease = true;
	this->customType = 0;
	this->customSize = 0;
	this->customBuff = 0;
	NEW_CLASS(this->mstm, IO::MemoryStream(UTF8STRC("IO.SPackageFile.mstm")));
	NEW_CLASS(this->fileMap, Data::BTreeUTF8Map<FileInfo*>());
	flength = this->stm->GetLength();
	if (flength >= 16)
	{
		this->stm->SeekFromBeginning(0);
		this->stm->Read(hdr, 24);
		if (hdr[0] == 'S' && hdr[1] == 'm' && hdr[2] == 'p' && hdr[3] == 'f')
		{
			this->flags = ReadInt32(&hdr[4]);
			if (this->flags & 2)
			{
				UInt64 lastOfst = ReadUInt64(&hdr[8]);
				UInt64 lastSize = ReadUInt64(&hdr[16]);
				this->currOfst = lastOfst + lastSize;
				if (this->currOfst > flength || lastOfst < 0 || lastSize < 0)
				{
					this->stm->SeekFromBeginning(0);
					hdr[0] = 'S';
					hdr[1] = 'm';
					hdr[2] = 'p';
					hdr[3] = 'f';
					WriteInt32(&hdr[4], 2);
					WriteInt64(&hdr[8], 24);
					WriteInt64(&hdr[16], 0);
					this->stm->Write(hdr, 24);
					this->flags = 2;
					this->currOfst = 24;
					WriteInt64(&hdr[0], 0);
					WriteInt64(&hdr[8], 0);
					this->mstm->Write(hdr, 16);
				}
				else
				{
					if (this->flags & 1)
					{
						UInt8 customBuff[8];
						this->stm->Read(customBuff, 8);
						this->customType = ReadInt32(&customBuff[0]);
						this->customSize = ReadUInt32(&customBuff[4]);
						if (this->customSize > 0)
						{
							this->customBuff = MemAlloc(UInt8, this->customSize);
							this->stm->Read(this->customBuff, this->customSize);
						}
					}
					this->mstm->Write(&hdr[8], 16);
					this->ReadV2DirEnt(lastOfst, lastSize);
					this->stm->SeekFromBeginning(this->currOfst);
				}
			}
			else
			{
				this->currOfst = ReadUInt64(&hdr[8]);
				dirSize = flength - this->currOfst;
				if (dirSize > 0)
				{
					UInt8 *dirBuff = MemAlloc(UInt8, (UOSInt)dirSize);
					this->stm->SeekFromBeginning(this->currOfst);
					this->stm->Read(dirBuff, (UOSInt)dirSize);
					this->stm->SeekFromBeginning(this->currOfst);
					this->mstm->Write(dirBuff, (UOSInt)dirSize);

					UOSInt i;
					UOSInt nameSize;
					FileInfo *file;
					i = 0;
					while (i < dirSize)
					{
						nameSize = ReadUInt16(&dirBuff[i + 24]);
						MemCopyNO(sbuff, &dirBuff[i + 26], nameSize);
						sbuff[nameSize] = 0;
						
						file = this->fileMap->Get(sbuff);
						if (file == 0)
						{
							file = MemAlloc(FileInfo, 1);
							file->ofst = ReadUInt64(&dirBuff[i]);
							file->size = ReadUInt64(&dirBuff[i + 8]);
							this->fileMap->Put(sbuff, file);
						}
						i += 26 + nameSize;
					}
					MemFree(dirBuff);
				}
				else
				{
					this->stm->SeekFromBeginning(this->currOfst);
				}
			}
		}
		else
		{
			this->stm->SeekFromBeginning(0);
			hdr[0] = 'S';
			hdr[1] = 'm';
			hdr[2] = 'p';
			hdr[3] = 'f';
			WriteInt32(&hdr[4], 2);
			WriteInt64(&hdr[8], 24);
			WriteInt64(&hdr[16], 0);
			this->stm->Write(hdr, 24);
			this->flags = 2;
			this->currOfst = 24;
			WriteInt64(&hdr[0], 0);
			WriteInt64(&hdr[8], 0);
			this->mstm->Write(hdr, 16);
		}
	}
	else
	{
		hdr[0] = 'S';
		hdr[1] = 'm';
		hdr[2] = 'p';
		hdr[3] = 'f';
		WriteInt32(&hdr[4], 2);
		WriteInt64(&hdr[8], 24);
		WriteInt64(&hdr[16], 0);
		this->stm->Write(hdr, 24);
		this->flags = 2;
		this->currOfst = 24;
		WriteInt64(&hdr[0], 0);
		WriteInt64(&hdr[8], 0);
		this->mstm->Write(hdr, 16);
	}
	this->writeMode = true;
	this->pauseCommit = false;
	NEW_CLASS(this->mut, Sync::Mutex());
}

IO::SPackageFile::~SPackageFile()
{
	UInt8 hdr[16];
	UInt8 *buff;
	UOSInt buffSize;
	if (!this->writeMode)
	{
		this->writeMode = true;
		this->stm->SeekFromBeginning(this->currOfst);
	}
	buff = this->mstm->GetBuff(&buffSize);
	if (this->flags & 2)
	{
		if (buffSize > 16)
		{
			this->stm->Write(buff, buffSize);
			this->stm->SeekFromBeginning(8);
			WriteUInt64(&hdr[0], this->currOfst);
			WriteUInt64(&hdr[8], buffSize);
			this->stm->Write(hdr, 16);
		}
	}
	else
	{
		if (buffSize > 0)
		{
			this->stm->Write(buff, buffSize);
		}
		this->stm->SeekFromBeginning(8);
		WriteUInt64(hdr, this->currOfst);
		this->stm->Write(hdr, 8);
	}

	if (this->toRelease)
	{
		DEL_CLASS(this->stm);
	}
	DEL_CLASS(this->mstm);
	DEL_CLASS(this->mut);
	if (this->customBuff)
	{
		MemFree(this->customBuff);
		this->customBuff = 0;
	}
	if (!this->fileMap->IsEmpty())
	{
		UOSInt i;
		IO::SPackageFile::FileInfo **fileArr = this->fileMap->ToArray(&i);
		while (i-- > 0)
		{
			MemFree(fileArr[i]);
		}
		MemFree(fileArr);
	}
	DEL_CLASS(this->fileMap);
}

Bool IO::SPackageFile::AddFile(IO::IStreamData *fd, const UTF8Char *fileName, Int64 modTimeTicks)
{
	UInt8 dataBuff[512];
	UOSInt strLen;
	UInt64 dataSize = fd->GetDataSize();
	UOSInt writeSize;
	Bool needCommit = false;

	Sync::MutexUsage mutUsage(this->mut);
	if (this->fileMap->Get(fileName) == 0)
	{
	}
	else
	{
		mutUsage.EndUse();
		return false;
	}
	WriteUInt64(&dataBuff[0], this->currOfst);
	WriteUInt64(&dataBuff[8], dataSize);
	WriteInt64(&dataBuff[16], modTimeTicks);
	strLen = Text::StrCharCnt(fileName);
	MemCopyNO(&dataBuff[26], fileName, strLen);
	WriteUInt16(&dataBuff[24], (UInt16)strLen);

	if (!this->writeMode)
	{
		this->writeMode = true;
		this->stm->SeekFromBeginning(this->currOfst);
	}

	writeSize = 0;
	UInt8 *fileBuff;
	if (dataSize > 1048576)
	{
		UOSInt readSize;
		UInt64 sizeLeft = dataSize;
		UInt64 fileOfst = 0;
		fileBuff = MemAlloc(UInt8, 1048576);
		while (sizeLeft > 0)
		{
			if (sizeLeft > 1048576)
			{
				readSize = 1048576;
			}
			else
			{
				readSize = (UOSInt)sizeLeft;
			}
			fd->GetRealData(fileOfst, readSize, fileBuff);
			writeSize += this->stm->Write(fileBuff, readSize);
			fileOfst += readSize;
			sizeLeft -= readSize;
		}
		MemFree(fileBuff);
	}
	else
	{
		fileBuff = MemAlloc(UInt8, (UOSInt)dataSize);
		fd->GetRealData(0, (UOSInt)dataSize, fileBuff);
		writeSize = this->stm->Write(fileBuff, (UOSInt)dataSize);
		MemFree(fileBuff);
	}
	Bool succ = false;
	if (writeSize == dataSize)
	{
		FileInfo *file = MemAlloc(FileInfo, 1);
		file->ofst = this->currOfst;
		file->size = dataSize;
		this->fileMap->Put(fileName, file);

		this->mstm->Write(dataBuff, 26 + strLen);
		this->currOfst += dataSize;
		succ = true;
		if (mstm->GetLength() >= 65536)
		{
			needCommit = true;
		}
	}
	else
	{
		this->writeMode = false;
	}
	mutUsage.EndUse();
	if (needCommit && !this->pauseCommit)
	{
		this->Commit();
	}
	return succ;
}

Bool IO::SPackageFile::AddFile(const UInt8 *fileBuff, UOSInt fileSize, const UTF8Char *fileName, Int64 modTimeTicks)
{
	UInt8 dataBuff[512];
	UOSInt strLen;
	Bool needCommit = false;
	Sync::MutexUsage mutUsage(this->mut);
	if (this->fileMap->Get(fileName) == 0)
	{
	}
	else
	{
		mutUsage.EndUse();
		return false;
	}
	WriteUInt64(&dataBuff[0], this->currOfst);
	WriteUInt64(&dataBuff[8], fileSize);
	WriteInt64(&dataBuff[16], modTimeTicks);
	strLen = Text::StrCharCnt(fileName);
	MemCopyNO(&dataBuff[26], fileName, strLen);
	WriteInt16(&dataBuff[24], (UInt16)strLen);

	if (!this->writeMode)
	{
		this->writeMode = true;
		this->stm->SeekFromBeginning(this->currOfst);
	}
	Bool succ = false;
	if (this->stm->Write(fileBuff, fileSize) == fileSize)
	{
		FileInfo *file = MemAlloc(FileInfo, 1);
		file->ofst = this->currOfst;
		file->size = fileSize;
		this->fileMap->Put(fileName, file);

		this->mstm->Write(dataBuff, 26 + strLen);
		this->currOfst += fileSize;
		succ = true;
		if (mstm->GetLength() >= 65536)
		{
			needCommit = true;
		}
	}
	else
	{
		this->writeMode = false;
	}

	mutUsage.EndUse();
	if (needCommit && !this->pauseCommit)
	{
		this->Commit();
	}
	return succ;
}

Bool IO::SPackageFile::AddPackage(IO::PackageFile *pkg, UTF8Char pathSeperator)
{
	UTF8Char sbuff[512];
	AddPackageInner(pkg, pathSeperator, sbuff, sbuff);
	return true;
}

Bool IO::SPackageFile::Commit()
{
	UOSInt buffSize;
	UInt8 *buff;
	UInt8 hdr[16];
	Bool succ = false;
	Sync::MutexUsage mutUsage(this->mut);
	buff = this->mstm->GetBuff(&buffSize);
	if (this->flags & 2)
	{
		if (buffSize > 16)
		{
			UOSInt writeSize;
			if (!this->writeMode)
			{
				this->writeMode = true;
				this->stm->SeekFromBeginning(this->currOfst);
			}

			writeSize = this->stm->Write(buff, buffSize);
			if (writeSize == buffSize)
			{
				this->stm->SeekFromBeginning(8);
				WriteUInt64(&hdr[0], this->currOfst);
				WriteUInt64(&hdr[8], buffSize);
				this->stm->Write(hdr, 16);
				this->writeMode = false;
				this->mstm->Clear();
				this->mstm->Write(hdr, 16);
				this->currOfst += writeSize;
				succ = true;
			}
			else
			{
				this->writeMode = false;
			}
		}
	}
	else
	{
		succ = false;
	}
	mutUsage.EndUse();
	return succ;
}

Bool IO::SPackageFile::OptimizeFile(const UTF8Char *newFile)
{
	UInt8 hdr[24];
	if ((this->flags & 2) == 0)
	{
		return false;
	}
	this->Commit();
	IO::FileStream *fs;
	IO::SPackageFile *spkg;
	NEW_CLASS(fs, IO::FileStream(newFile, IO::FileMode::Create, IO::FileShare::DenyWrite, IO::FileStream::BufferType::NoWriteBuffer));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	if (this->flags & 1)
	{
		NEW_CLASS(spkg, IO::SPackageFile(fs, true, this->customType, this->customSize, this->customBuff));
	}
	else
	{
		NEW_CLASS(spkg, IO::SPackageFile(fs, true));
	}
	spkg->PauseCommit(true);
	Sync::MutexUsage mutUsage(this->mut);
	this->writeMode = false;
	this->stm->SeekFromBeginning(0);
	this->stm->Read(hdr, 24);
	UInt64 lastOfst = ReadUInt64(&hdr[8]);
	UInt64 lastSize = ReadUInt64(&hdr[16]);
	if (lastSize > 0)
	{
		this->OptimizeFileInner(spkg, lastOfst, lastSize);
	}
	mutUsage.EndUse();
	DEL_CLASS(spkg);
	return true;
}

void IO::SPackageFile::PauseCommit(Bool pauseCommit)
{
	this->pauseCommit = pauseCommit;
}

IO::IStreamData *IO::SPackageFile::CreateStreamData(const UTF8Char *fileName)
{
	IO::IStreamData *fd = 0;
	Sync::MutexUsage mutUsage(this->mut);
	FileInfo *file = this->fileMap->Get(fileName);
	if (file)
	{
		UInt8 *fileBuff = MemAlloc(UInt8, (UOSInt)file->size);
		this->writeMode = false;
		this->stm->SeekFromBeginning(file->ofst);
		this->stm->Read(fileBuff, (UOSInt)file->size);
		NEW_CLASS(fd, IO::StmData::MemoryData2(fileBuff, (UOSInt)file->size));
		MemFree(fileBuff);
	}
	mutUsage.EndUse();
	return fd;
}
