#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/FileStream.h"
#include "IO/SPackageFile.h"
#include "IO/StmData/MemoryDataCopy.h"
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
	Data::ByteBuffer dirBuff((UIntOS)size);
	this->stm->SeekFromBeginning(ofst);
	this->stm->Read(dirBuff);
	nextOfst = ReadUInt64(&dirBuff[0]);
	nextSize = ReadUInt64(&dirBuff[8]);
	this->ReadV2DirEnt(nextOfst, nextSize);

	UnsafeArray<UTF8Char> sbuff;
	UIntOS i;
	UIntOS nameSize;
	NN<FileInfo> file;
	sbuff = MemAllocArr(UTF8Char, 512);
	i = 16;
	while (i < size)
	{
		nameSize = ReadUInt16(&dirBuff[i + 24]);
		MemCopyNO(sbuff.Ptr(), &dirBuff[i + 26], nameSize);
		sbuff[nameSize] = 0;
		
		if (!this->fileMap.Get({sbuff, nameSize}).SetTo(file))
		{
			file = MemAllocNN(FileInfo);
			file->ofst = ReadUInt64(&dirBuff[i]);
			file->size = ReadUInt64(&dirBuff[i + 8]);
			this->fileMap.Put({sbuff, nameSize}, file);
		}
		i += 26 + nameSize;
	}
	MemFreeArr(sbuff);
}

void IO::SPackageFile::AddPackageInner(NN<IO::PackageFile> pkg, UTF8Char pathSeperator, UnsafeArray<UTF8Char> pathStart, UnsafeArray<UTF8Char> pathEnd)
{
	UIntOS i = 0;
	UIntOS j = pkg->GetCount();
	UnsafeArray<UTF8Char> sptr;
	while (i < j)
	{
		if (pkg->GetItemName(pathEnd, i).SetTo(sptr))
		{
			IO::PackageFile::PackObjectType pt = pkg->GetItemType(i);
			if (pathEnd[0] == '.')
			{
			}
			else if (pt == IO::PackageFile::PackObjectType::PackageFileType)
			{
				Bool innerNeedDelete;
				NN<IO::PackageFile> innerPack;
				if (pkg->GetItemPack(i, innerNeedDelete).SetTo(innerPack))
				{
					*sptr++ = pathSeperator;
					AddPackageInner(innerPack, pathSeperator, pathStart, sptr);
					if (innerNeedDelete)
					{
						innerPack.Delete();
					}
				}
			}
			else if (pt == IO::PackageFile::PackObjectType::StreamData)
			{
				NN<IO::StreamData> fd;
				if (pkg->GetItemStmDataNew(i).SetTo(fd))
				{
					this->AddFile(fd, {pathStart, (UIntOS)(sptr - pathStart)}, pkg->GetItemModTime(i));
					fd.Delete();
				}
			}
		}
		i++;
	}
}

Bool IO::SPackageFile::OptimizeFileInner(NN<IO::SPackageFile> newFile, UInt64 dirOfst, UInt64 dirSize)
{
	UInt64 lastOfst;
	UInt64 lastSize;
	UIntOS i;
	UIntOS j;
	Bool succ = true;
	Data::ByteBuffer dirBuff((UIntOS)dirSize);
	this->stm->SeekFromBeginning(dirOfst);
	if (this->stm->Read(dirBuff) == dirSize)
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
		UnsafeArray<UTF8Char> sbuff = MemAllocArr(UTF8Char, 512);
		lastOfst = 0;
		lastSize = 0;
		i = 16;
		while (succ && i < dirSize)
		{
			thisOfst = ReadUInt64(&dirBuff[i]);
			thisSize = ReadUInt64(&dirBuff[i + 8]);
			j = ReadUInt16(&dirBuff[i + 24]);
			MemCopyNO(sbuff.Ptr(), &dirBuff[i + 26], j);
			sbuff[j] = 0;
			Data::ByteBuffer fileBuff((UIntOS)thisSize);
			if (thisOfst != lastOfst + lastSize)
			{
				this->stm->SeekFromBeginning(thisOfst);
			}
			if (this->stm->Read(fileBuff) == thisSize)
			{
				newFile->AddFile(fileBuff.Arr(), (UIntOS)thisSize, {sbuff, j}, Data::Timestamp(ReadInt64(&dirBuff[i + 16]), 0));
				lastOfst = thisOfst;
				lastSize = thisSize;
			}
			else
			{
				succ = false;
			}
			i += 26 + j;
		}
		MemFreeArr(sbuff);
	}
	else
	{
		succ = false;
	}
	return succ;
}

IO::SPackageFile::SPackageFile(NN<IO::SeekableStream> stm, Bool toRelease)
{
	UInt8 hdr[24];
	this->stm = stm;
	this->toRelease = toRelease;
	this->currOfst = 24;
	hdr[0] = 'S';
	hdr[1] = 'm';
	hdr[2] = 'p';
	hdr[3] = 'f';
	WriteInt32(&hdr[4], 2);
	WriteInt64(&hdr[8], 24);
	WriteInt64(&hdr[16], 0);
	this->stm->Write(Data::ByteArrayR(hdr, 24));
	this->customType = 0;
	this->customSize = 0;
	this->writeMode = true;
	this->flags = 2;
	this->pauseCommit = false;
	WriteInt64(&hdr[0], 0);
	WriteInt64(&hdr[8], 0);
	this->mstm.Write(Data::ByteArrayR(hdr, 16));
}

IO::SPackageFile::SPackageFile(NN<IO::SeekableStream> stm, Bool toRelease, Int32 customType, UIntOS customSize, Data::ByteArrayR customBuff)
{
	UInt8 hdr[32];
	this->stm = stm;
	this->toRelease = toRelease;
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
	this->stm->Write(Data::ByteArrayR(hdr, 32));
	if (customSize > 0)
	{
		this->stm->Write(customBuff.WithSize(customSize));
	}
	this->flags = 3;
	this->customType = customType;
	this->customSize = customSize;
	if (customSize > 0)
	{
		this->customBuff.ChangeSizeAndClear(customSize);
		this->customBuff.CopyFrom(customBuff.WithSize(customSize));
	}
	this->writeMode = true;
	this->pauseCommit = false;
	WriteInt64(&hdr[0], 0);
	WriteInt64(&hdr[8], 0);
	this->mstm.Write(Data::ByteArrayR(hdr, 16));
}

IO::SPackageFile::SPackageFile(Text::CStringNN fileName)
{
	UInt8 hdr[24];
	UInt64 flength;
	UInt64 dirSize;
	UTF8Char sbuff[512];
	NEW_CLASSNN(this->stm, IO::FileStream(fileName, IO::FileMode::Append, IO::FileShare::DenyWrite, IO::FileStream::BufferType::NoWriteBuffer));
	this->toRelease = true;
	this->customType = 0;
	this->customSize = 0;
	flength = this->stm->GetLength();
	if (flength >= 16)
	{
		this->stm->SeekFromBeginning(0);
		this->stm->Read(BYTEARR(hdr));
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
					this->stm->Write(Data::ByteArrayR(hdr, 24));
					this->flags = 2;
					this->currOfst = 24;
					WriteInt64(&hdr[0], 0);
					WriteInt64(&hdr[8], 0);
					this->mstm.Write(Data::ByteArrayR(hdr, 16));
				}
				else
				{
					if (this->flags & 1)
					{
						UInt8 customBuff[8];
						this->stm->Read(BYTEARR(customBuff));
						this->customType = ReadInt32(&customBuff[0]);
						this->customSize = ReadUInt32(&customBuff[4]);
						if (this->customSize > 0)
						{
							this->customBuff.ChangeSizeAndClear(this->customSize);
							this->stm->Read(this->customBuff);
						}
					}
					this->mstm.Write(Data::ByteArrayR(&hdr[8], 16));
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
					Data::ByteBuffer dirBuff((UIntOS)dirSize);
					this->stm->SeekFromBeginning(this->currOfst);
					this->stm->Read(dirBuff);
					this->stm->SeekFromBeginning(this->currOfst);
					this->mstm.Write(dirBuff);

					UIntOS i;
					UIntOS nameSize;
					NN<FileInfo> file;
					i = 0;
					while (i < dirSize)
					{
						nameSize = ReadUInt16(&dirBuff[i + 24]);
						MemCopyNO(sbuff, &dirBuff[i + 26], nameSize);
						sbuff[nameSize] = 0;
						
						if (!this->fileMap.Get({sbuff, nameSize}).SetTo(file))
						{
							file = MemAllocNN(FileInfo);
							file->ofst = ReadUInt64(&dirBuff[i]);
							file->size = ReadUInt64(&dirBuff[i + 8]);
							this->fileMap.Put({sbuff, nameSize}, file);
						}
						i += 26 + nameSize;
					}
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
			this->stm->Write(Data::ByteArrayR(hdr, 24));
			this->flags = 2;
			this->currOfst = 24;
			WriteInt64(&hdr[0], 0);
			WriteInt64(&hdr[8], 0);
			this->mstm.Write(Data::ByteArrayR(hdr, 16));
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
		this->stm->Write(Data::ByteArrayR(hdr, 24));
		this->flags = 2;
		this->currOfst = 24;
		WriteInt64(&hdr[0], 0);
		WriteInt64(&hdr[8], 0);
		this->mstm.Write(Data::ByteArrayR(hdr, 16));
	}
	this->writeMode = true;
	this->pauseCommit = false;
}

IO::SPackageFile::~SPackageFile()
{
	UInt8 hdr[16];
	UnsafeArray<UInt8> buff;
	UIntOS buffSize;
	if (!this->writeMode)
	{
		this->writeMode = true;
		this->stm->SeekFromBeginning(this->currOfst);
	}
	buff = this->mstm.GetBuff(buffSize);
	if (this->flags & 2)
	{
		if (buffSize > 16)
		{
			this->stm->Write(Data::ByteArrayR(buff, buffSize));
			this->stm->SeekFromBeginning(8);
			WriteUInt64(&hdr[0], this->currOfst);
			WriteUInt64(&hdr[8], buffSize);
			this->stm->Write(Data::ByteArrayR(hdr, 16));
		}
	}
	else
	{
		if (buffSize > 0)
		{
			this->stm->Write(Data::ByteArrayR(buff, buffSize));
		}
		this->stm->SeekFromBeginning(8);
		WriteUInt64(hdr, this->currOfst);
		this->stm->Write(Data::ByteArrayR(hdr, 8));
	}

	if (this->toRelease)
	{
		this->stm.Delete();
	}
	if (!this->fileMap.IsEmpty())
	{
		UIntOS i;
		NN<FileInfo> file;
		UnsafeArray<Optional<IO::SPackageFile::FileInfo>> fileArr = this->fileMap.ToArray(i);
		while (i-- > 0)
		{
			if (fileArr[i].SetTo(file))
				MemFreeNN(file);
		}
		MemFreeArr(fileArr);
	}
}

Bool IO::SPackageFile::AddFile(NN<IO::StreamData> fd, Text::CStringNN fileName, const Data::Timestamp &modTime)
{
	UInt8 dataBuff[512];
	UInt64 dataSize = fd->GetDataSize();
	UIntOS writeSize;
	Bool needCommit = false;

	Sync::MutexUsage mutUsage(this->mut);
	if (this->fileMap.Get(fileName).IsNull())
	{
	}
	else
	{
		mutUsage.EndUse();
		return false;
	}
	WriteUInt64(&dataBuff[0], this->currOfst);
	WriteUInt64(&dataBuff[8], dataSize);
	WriteInt64(&dataBuff[16], modTime.ToTicks());
	MemCopyNO(&dataBuff[26], fileName.v.Ptr(), fileName.leng);
	WriteUInt16(&dataBuff[24], (UInt16)fileName.leng);

	if (!this->writeMode)
	{
		this->writeMode = true;
		this->stm->SeekFromBeginning(this->currOfst);
	}

	writeSize = 0;
	if (dataSize > 1048576)
	{
		UIntOS readSize;
		UInt64 sizeLeft = dataSize;
		UInt64 fileOfst = 0;
		Data::ByteBuffer fileBuff(1048576);
		while (sizeLeft > 0)
		{
			if (sizeLeft > 1048576)
			{
				readSize = 1048576;
			}
			else
			{
				readSize = (UIntOS)sizeLeft;
			}
			fd->GetRealData(fileOfst, readSize, fileBuff);
			writeSize += this->stm->Write(fileBuff.WithSize(readSize));
			fileOfst += readSize;
			sizeLeft -= readSize;
		}
	}
	else
	{
		Data::ByteBuffer fileBuff((UIntOS)dataSize);
		fd->GetRealData(0, (UIntOS)dataSize, fileBuff);
		writeSize = this->stm->Write(fileBuff);
	}
	Bool succ = false;
	if (writeSize == dataSize)
	{
		FileInfo *file = MemAlloc(FileInfo, 1);
		file->ofst = this->currOfst;
		file->size = dataSize;
		this->fileMap.Put(fileName, file);

		this->mstm.Write(Data::ByteArrayR(dataBuff, 26 + fileName.leng));
		this->currOfst += dataSize;
		succ = true;
		if (this->mstm.GetLength() >= 65536)
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

Bool IO::SPackageFile::AddFile(UnsafeArray<const UInt8> fileBuff, UIntOS fileSize, Text::CStringNN fileName, const Data::Timestamp &modTime)
{
	UInt8 dataBuff[512];
	Bool needCommit = false;
	Sync::MutexUsage mutUsage(this->mut);
	if (this->fileMap.Get(fileName).IsNull())
	{
	}
	else
	{
		mutUsage.EndUse();
		return false;
	}
	WriteUInt64(&dataBuff[0], this->currOfst);
	WriteUInt64(&dataBuff[8], fileSize);
	WriteInt64(&dataBuff[16], modTime.ToTicks());
	
	MemCopyNO(&dataBuff[26], fileName.v.Ptr(), fileName.leng);
	WriteInt16(&dataBuff[24], (UInt16)fileName.leng);

	if (!this->writeMode)
	{
		this->writeMode = true;
		this->stm->SeekFromBeginning(this->currOfst);
	}
	Bool succ = false;
	if (this->stm->Write(Data::ByteArrayR(fileBuff, fileSize)) == fileSize)
	{
		FileInfo *file = MemAlloc(FileInfo, 1);
		file->ofst = this->currOfst;
		file->size = fileSize;
		this->fileMap.Put(fileName, file);

		this->mstm.Write(Data::ByteArrayR(dataBuff, 26 + fileName.leng));
		this->currOfst += fileSize;
		succ = true;
		if (this->mstm.GetLength() >= 65536)
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

Bool IO::SPackageFile::AddPackage(NN<IO::PackageFile> pkg, UTF8Char pathSeperator)
{
	UTF8Char sbuff[512];
	AddPackageInner(pkg, pathSeperator, sbuff, sbuff);
	return true;
}

Bool IO::SPackageFile::Commit()
{
	UIntOS buffSize;
	UnsafeArray<UInt8> buff;
	UInt8 hdr[16];
	Bool succ = false;
	Sync::MutexUsage mutUsage(this->mut);
	buff = this->mstm.GetBuff(buffSize);
	if (this->flags & 2)
	{
		if (buffSize > 16)
		{
			UIntOS writeSize;
			if (!this->writeMode)
			{
				this->writeMode = true;
				this->stm->SeekFromBeginning(this->currOfst);
			}

			writeSize = this->stm->Write(Data::ByteArrayR(buff, buffSize));
			if (writeSize == buffSize)
			{
				this->stm->SeekFromBeginning(8);
				WriteUInt64(&hdr[0], this->currOfst);
				WriteUInt64(&hdr[8], buffSize);
				this->stm->Write(Data::ByteArrayR(hdr, 16));
				this->writeMode = false;
				this->mstm.Clear();
				this->mstm.Write(Data::ByteArrayR(hdr, 16));
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

Bool IO::SPackageFile::OptimizeFile(Text::CStringNN newFile)
{
	UInt8 hdr[24];
	if ((this->flags & 2) == 0)
	{
		return false;
	}
	this->Commit();
	NN<IO::FileStream> fs;
	NN<IO::SPackageFile> spkg;
	NEW_CLASSNN(fs, IO::FileStream(newFile, IO::FileMode::Create, IO::FileShare::DenyWrite, IO::FileStream::BufferType::NoWriteBuffer));
	if (fs->IsError())
	{
		fs.Delete();
		return false;
	}
	if (this->flags & 1)
	{
		NEW_CLASSNN(spkg, IO::SPackageFile(fs, true, this->customType, this->customSize, this->customBuff));
	}
	else
	{
		NEW_CLASSNN(spkg, IO::SPackageFile(fs, true));
	}
	spkg->PauseCommit(true);
	Sync::MutexUsage mutUsage(this->mut);
	this->writeMode = false;
	this->stm->SeekFromBeginning(0);
	this->stm->Read(BYTEARR(hdr));
	UInt64 lastOfst = ReadUInt64(&hdr[8]);
	UInt64 lastSize = ReadUInt64(&hdr[16]);
	if (lastSize > 0)
	{
		this->OptimizeFileInner(spkg, lastOfst, lastSize);
	}
	mutUsage.EndUse();
	spkg.Delete();
	return true;
}

void IO::SPackageFile::PauseCommit(Bool pauseCommit)
{
	this->pauseCommit = pauseCommit;
}

Optional<IO::StreamData> IO::SPackageFile::CreateStreamData(Text::CStringNN fileName)
{
	IO::StreamData *fd = 0;
	Sync::MutexUsage mutUsage(this->mut);
	NN<FileInfo> file;
	if (this->fileMap.Get(fileName).SetTo(file))
	{
		Data::ByteBuffer fileBuff((UIntOS)file->size);
		this->writeMode = false;
		this->stm->SeekFromBeginning(file->ofst);
		this->stm->Read(fileBuff);
		NEW_CLASS(fd, IO::StmData::MemoryDataCopy(fileBuff));
	}
	return fd;
}
