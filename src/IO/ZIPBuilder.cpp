#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "Data/Compress/Inflate.h"
#include "IO/MinizZIP.h"
#include "IO/ZIPBuilder.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#define ZIPVER 63
IO::ZIPBuilder::ZIPBuilder(NotNullPtr<IO::SeekableStream> stm, ZIPOS os)
{
	this->stm = stm;
	this->baseOfst = this->stm->GetPosition();
	this->currOfst = 0;
	this->osType = os;
}

IO::ZIPBuilder::~ZIPBuilder()
{
	IO::ZIPBuilder::FileInfo *file;
	UInt8 hdrBuff[512];
	Data::DateTime dt;
	UOSInt hdrLen;
	UOSInt cdLen = 0;
	UOSInt i = 0;
	UOSInt j = this->files.GetCount();
	UInt8 minVer;
	while (i < j)
	{
		file = this->files.GetItem(i);
		minVer = 20;
		dt.SetValue(file->fileModTime.inst, file->fileModTime.tzQhr);
		WriteUInt32(&hdrBuff[0], 0x02014b50);
		hdrBuff[4] = ZIPVER;
		hdrBuff[5] = (UInt8)this->osType;
		hdrBuff[7] = (UInt8)this->osType;
		WriteUInt16(&hdrBuff[8], 0); //General purpose flag
		WriteUInt16(&hdrBuff[10], file->compMeth);
		WriteUInt16(&hdrBuff[12], dt.ToMSDOSTime());
		WriteUInt16(&hdrBuff[14], dt.ToMSDOSDate());
		WriteUInt32(&hdrBuff[16], file->crcVal);
		WriteUInt32(&hdrBuff[20], (UInt32)file->compSize);
		WriteUInt32(&hdrBuff[24], (UInt32)file->uncompSize);
		WriteUInt16(&hdrBuff[28], file->fileName->leng);
		WriteUInt16(&hdrBuff[30], 0); //extra field length
		WriteUInt16(&hdrBuff[32], 0); //File comment length
		WriteUInt16(&hdrBuff[34], 0); //Disk number where file starts
		WriteUInt16(&hdrBuff[36], 0); //Internal file attributes
		WriteUInt32(&hdrBuff[38], file->fileAttr); //External file attributes
		WriteUInt32(&hdrBuff[42], (UInt32)file->fileOfst);
		MemCopyNO(&hdrBuff[46], file->fileName->v, file->fileName->leng);
		hdrLen = 46 + file->fileName->leng;
		#if _OSINT_SIZE > 32
		if (file->compSize >= 0x100000000LL || file->fileOfst >= 0x100000000LL || file->uncompSize >= 0x100000000LL)
		{
			UOSInt len = 0;
			WriteUInt16(&hdrBuff[hdrLen], 1);
			if (file->uncompSize >= 0x100000000LL)
			{
				WriteUInt64(&hdrBuff[hdrLen + 4 + len], file->uncompSize);
				WriteUInt32(&hdrBuff[24], 0xffffffff);
				len += 8;
			}
			if (file->uncompSize >= 0x100000000LL)
			{
				WriteUInt64(&hdrBuff[hdrLen + 4 + len], file->compSize);
				WriteUInt32(&hdrBuff[20], 0xffffffff);
				len += 8;
			}
			if (file->uncompSize >= 0x100000000LL)
			{
				WriteUInt64(&hdrBuff[hdrLen + 4 + len], file->fileOfst);
				WriteUInt32(&hdrBuff[42], 0xffffffff);
				len += 8;
			}
			WriteUInt16(&hdrBuff[hdrLen + 2], len);
			hdrLen += 4 + len;
		}
		#endif
		if (!file->fileModTime.IsNull() && !file->fileAccessTime.IsNull() && !file->fileCreateTime.IsNull())
		{
			if (minVer < 45)
				minVer = 45;
			WriteUInt16(&hdrBuff[hdrLen], 10);
			WriteUInt16(&hdrBuff[hdrLen + 2], 32);
			WriteUInt32(&hdrBuff[hdrLen + 4], 0);
			WriteUInt16(&hdrBuff[hdrLen + 8], 1);
			WriteUInt16(&hdrBuff[hdrLen + 10], 24);
			file->fileModTime.ToFILETIME(&hdrBuff[hdrLen + 12]);
			file->fileAccessTime.ToFILETIME(&hdrBuff[hdrLen + 20]);
			file->fileCreateTime.ToFILETIME(&hdrBuff[hdrLen + 28]);
			hdrLen += 36;
		}
		WriteUInt16(&hdrBuff[30], hdrLen - 46 - file->fileName->leng);
		hdrBuff[6] = minVer;

		this->stm->Write(hdrBuff, hdrLen);
		cdLen += hdrLen;

		file->fileName->Release();
		MemFree(file);
		i++;
	}
	if (this->currOfst >= 0xffffffff)
	{
		UInt64 cdOfst = this->stm->GetPosition();
		WriteUInt32(&hdrBuff[0], 0x06064b50); //Record Type (Zip64 End of central directory record)
		WriteUInt64(&hdrBuff[4], 44); //Size of zip64 end of central directory record
		WriteUInt16(&hdrBuff[12], ZIPVER); //Version made by
		WriteUInt16(&hdrBuff[14], 45); //Version needed to extract
		WriteUInt32(&hdrBuff[16], 0); //Number of this disk
		WriteUInt32(&hdrBuff[20], 0); //Number of the disk with the start of the central directory
		WriteUInt64(&hdrBuff[24], j); //Total number of entries in the central directory on this disk
		WriteUInt64(&hdrBuff[32], j); //Total number of entries in the central directory
		WriteUInt32(&hdrBuff[40], (UInt32)cdLen); //Size of central directory
		WriteUInt32(&hdrBuff[48], (UInt32)this->currOfst); //Offset of start of central directory with respect to the starting disk number
		this->stm->Write(hdrBuff, 56);

		WriteUInt32(&hdrBuff[0], 0x07064b50); //Record Type (Zip64 end of central directory locator)
		WriteUInt32(&hdrBuff[4], 0); //Number of the disk with the start of the zip64 end of central directory
		WriteUInt64(&hdrBuff[8], cdOfst); //Relative offset of the zip64 end of central directory record
		WriteUInt32(&hdrBuff[16], 1); //Total number of disks
		this->stm->Write(hdrBuff, 20);

		WriteUInt32(&hdrBuff[0], 0x06054b50); //Record Type (End of central directory record)
		WriteUInt16(&hdrBuff[4], 0); //Number of this disk
		WriteUInt16(&hdrBuff[6], 0); //Disk where central directory starts
		WriteUInt16(&hdrBuff[8], j); //Number of central directory of this disk
		WriteUInt16(&hdrBuff[10], j); //Total number of central directory records
		WriteUInt32(&hdrBuff[12], (UInt32)cdLen); //Size of central directory
		WriteUInt32(&hdrBuff[16], 0xffffffff); //Offset of start of central directory
		WriteUInt16(&hdrBuff[20], 0); //Comment Length
		this->stm->Write(hdrBuff, 22);
	}
	else
	{
		WriteUInt32(&hdrBuff[0], 0x06054b50); //Record Type (End of central directory record)
		WriteUInt16(&hdrBuff[4], 0); //Number of this disk
		WriteUInt16(&hdrBuff[6], 0); //Disk where central directory starts
		WriteUInt16(&hdrBuff[8], j); //Number of central directory of this disk
		WriteUInt16(&hdrBuff[10], j); //Total number of central directory records
		WriteUInt32(&hdrBuff[12], (UInt32)cdLen); //Size of central directory
		WriteUInt32(&hdrBuff[16], (UInt32)this->currOfst); //Offset of start of central directory
		WriteUInt16(&hdrBuff[20], 0); //Comment Length
		this->stm->Write(hdrBuff, 22);
	}
}

Bool IO::ZIPBuilder::AddFile(Text::CStringNN fileName, const UInt8 *fileContent, UOSInt fileSize, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, Data::Compress::Inflate::CompressionLevel compLevel, UInt32 unixAttr)
{
	UInt8 hdrBuff[512];
	UOSInt hdrLen;
	UInt8 *outBuff = MemAlloc(UInt8, fileSize + 32);
	UOSInt compSize;
	if (compLevel == Data::Compress::Inflate::CompressionLevel::NoCompression)
	{
		compSize = fileSize;
	}
	else
	{
		compSize = (UOSInt)Data::Compress::Inflate::Compress(fileContent, fileSize, outBuff, false, compLevel);
	}
	UInt32 crcVal = this->crc.CalcDirect(fileContent, fileSize);
	Data::DateTime dt(lastModTime.inst, lastModTime.tzQhr);
	WriteUInt32(&hdrBuff[0], 0x04034b50);
	hdrBuff[4] = 20; //Verison (2.0)
	hdrBuff[5] = (UInt8)this->osType;
	WriteUInt16(&hdrBuff[6], 0);
	WriteUInt16(&hdrBuff[8], 0x8);
	WriteUInt16(&hdrBuff[10], dt.ToMSDOSTime());
	WriteUInt16(&hdrBuff[12], dt.ToMSDOSDate());
	WriteUInt32(&hdrBuff[14], crcVal);
	WriteUInt32(&hdrBuff[18], (UInt32)compSize);
	WriteUInt32(&hdrBuff[22], (UInt32)fileSize);
	WriteUInt16(&hdrBuff[26], (UInt32)fileName.leng);
	WriteUInt16(&hdrBuff[28], 0);
	MemCopyNO(&hdrBuff[30], fileName.v, fileName.leng);
	hdrLen = 30 + fileName.leng;
	#if _OSINT_SIZE > 32
	if (compSize >= 0x100000000LL || fileSize >= 0x100000000LL)
	{
		WriteUInt16(&hdrBuff[28], 20);
		WriteUInt16(&hdrBuff[hdrLen], 1);
		WriteUInt16(&hdrBuff[hdrLen + 2], 16);
		WriteUInt64(&hdrBuff[hdrLen + 4], compSize);
		WriteUInt64(&hdrBuff[hdrLen + 12], fileSize);
		if (compSize >= fileSize)
		{
			WriteUInt64(&hdrBuff[hdrLen + 4], fileSize);
		}
		WriteUInt32(&hdrBuff[18], 0xffffffff);
		WriteUInt32(&hdrBuff[22], 0xffffffff);
		hdrLen += 20;
	}
	#endif

	IO::ZIPBuilder::FileInfo *file;
	Bool succ = false;
	file = MemAlloc(IO::ZIPBuilder::FileInfo, 1);
	file->fileName = Text::String::New(fileName);
	file->fileModTime = lastModTime;
	file->fileCreateTime = createTime;
	file->fileAccessTime = lastAccessTime;
	file->crcVal = crcVal;
	file->uncompSize = fileSize;
	file->compMeth = 8;
	file->compSize = compSize;
	if (this->osType == IO::ZIPOS::UNIX)
	{
		file->fileAttr = unixAttr << 16;
	}
	else if (unixAttr == 0)
	{
		file->fileAttr = 0;
	}
	else
	{
		if (unixAttr & 0x200)
		{
			file->fileAttr = 0;
		}
		else
		{
			file->fileAttr = 1;
		}
	}
	Sync::MutexUsage mutUsage(this->mut);
	file->fileOfst = this->currOfst;
	this->files.Add(file);
	if (compSize >= fileSize)
	{
		UOSInt writeSize;
		WriteInt16(&hdrBuff[8], 0x0);
		WriteInt32(&hdrBuff[18], (Int32)fileSize);
		file->compMeth = 0;
		file->compSize = fileSize;
		writeSize = this->stm->Write(hdrBuff, hdrLen);
		writeSize += this->stm->Write(fileContent, fileSize);
		this->currOfst += writeSize;
		succ = writeSize == (hdrLen + fileSize);
	}
	else
	{
		UOSInt writeSize;
		writeSize = this->stm->Write(hdrBuff, hdrLen);
		writeSize += this->stm->Write(outBuff, compSize);
		this->currOfst += writeSize;
		succ = writeSize == (hdrLen + compSize);
	}
	MemFree(outBuff);
	return succ;
}

Bool IO::ZIPBuilder::AddDir(Text::CStringNN dirName, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, UInt32 unixAttr)
{
	if (!dirName.EndsWith('/'))
		return false;

	UInt8 hdrBuff[512];
	UOSInt hdrLen;
	Data::DateTime dt(lastModTime.inst, lastModTime.tzQhr);
	WriteUInt32(&hdrBuff[0], 0x04034b50);
	hdrBuff[4] = 45;
	hdrBuff[5] = (UInt8)this->osType;
	WriteUInt16(&hdrBuff[6], 0);
	WriteUInt16(&hdrBuff[8], 0);
	WriteUInt16(&hdrBuff[10], dt.ToMSDOSTime());
	WriteUInt16(&hdrBuff[12], dt.ToMSDOSDate());
	WriteUInt32(&hdrBuff[14], 0);
	WriteUInt32(&hdrBuff[18], 0);
	WriteUInt32(&hdrBuff[22], 0);
	WriteUInt16(&hdrBuff[26], (Int32)dirName.leng);
	WriteUInt16(&hdrBuff[28], 0);
	MemCopyNO(&hdrBuff[30], dirName.v, dirName.leng);
	hdrLen = 30 + dirName.leng;

	IO::ZIPBuilder::FileInfo *file;
	file = MemAlloc(IO::ZIPBuilder::FileInfo, 1);
	file->fileName = Text::String::New(dirName);
	file->fileModTime = lastModTime;
	file->fileCreateTime = createTime;
	file->fileAccessTime = lastAccessTime;
	file->crcVal = 0;
	file->uncompSize = 0;
	file->compMeth = 0;
	file->compSize = 0;
	if (this->osType == IO::ZIPOS::UNIX)
	{
		file->fileAttr = (unixAttr << 16) | 0x10;
	}
	else if (unixAttr == 0)
	{
		file->fileAttr = 0x10;
	}
	else
	{
		if (unixAttr & 0x200)
		{
			file->fileAttr = 0x10;
		}
		else
		{
			file->fileAttr = 0x11;
		}
	}
	Sync::MutexUsage mutUsage(this->mut);
	file->fileOfst = this->currOfst;
	this->files.Add(file);
	UOSInt writeSize;
	writeSize = this->stm->Write(hdrBuff, hdrLen);
	this->currOfst += writeSize;
	return writeSize == hdrLen;
}
