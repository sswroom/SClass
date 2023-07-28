#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "Data/Compress/Inflate.h"
#include "IO/MinizZIP.h"
#include "IO/ZIPBuilder.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

IO::ZIPBuilder::ZIPBuilder(NotNullPtr<IO::SeekableStream> stm)
{
	this->stm = stm;
	this->baseOfst = this->stm->GetPosition();
	this->currOfst = 0;
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
	while (i < j)
	{
		file = this->files.GetItem(i);

		dt.SetTicks(file->fileTimeTicks);
		dt.ToLocalTime();
		WriteInt32(&hdrBuff[0], 0x02014b50);
		WriteInt16(&hdrBuff[4], 20);
		WriteInt16(&hdrBuff[6], 20);
		WriteInt16(&hdrBuff[8], 0x800);
		WriteInt16(&hdrBuff[10], file->compMeth);
		WriteInt16(&hdrBuff[12], dt.ToMSDOSTime());
		WriteInt16(&hdrBuff[14], dt.ToMSDOSDate());
		WriteUInt32(&hdrBuff[16], file->crcVal);
		WriteInt32(&hdrBuff[20], (Int32)file->compSize);
		WriteInt32(&hdrBuff[24], (Int32)file->uncompSize);
		WriteInt16(&hdrBuff[28], file->fileName->leng);
		WriteInt16(&hdrBuff[30], 0); //extra field length
		WriteInt16(&hdrBuff[32], 0); //File comment length
		WriteInt16(&hdrBuff[34], 0); //Disk number where file starts
		WriteInt16(&hdrBuff[36], 0); //Internal file attributes
		WriteInt32(&hdrBuff[38], 0); //External file attributes
		WriteInt32(&hdrBuff[42], (Int32)file->fileOfst);
		MemCopyNO(&hdrBuff[46], file->fileName->v, file->fileName->leng);
		hdrLen = 46 + file->fileName->leng;
		#if _OSINT_SIZE > 32
		if (file->compSize >= 0x100000000LL || file->uncompSize >= 0x100000000LL)
		{
			WriteInt16(&hdrBuff[28], 20);
			WriteInt16(&hdrBuff[hdrLen], 1);
			WriteInt16(&hdrBuff[hdrLen + 2], 16);
			WriteUInt64(&hdrBuff[hdrLen + 4], file->compSize);
			WriteUInt64(&hdrBuff[hdrLen + 12], file->uncompSize);
			if (file->compSize >= file->uncompSize)
			{
				WriteUInt64(&hdrBuff[hdrLen + 4], file->uncompSize);
			}
			hdrLen += 20;
		}
		#endif

		this->stm->Write(hdrBuff, hdrLen);
		cdLen += hdrLen;

		file->fileName->Release();
		MemFree(file);
		i++;
	}

	WriteInt32(&hdrBuff[0], 0x06054b50);
	WriteInt16(&hdrBuff[4], 0);
	WriteInt16(&hdrBuff[6], 0);
	WriteInt16(&hdrBuff[8], j);
	WriteInt16(&hdrBuff[10], j);
	WriteInt32(&hdrBuff[12], (Int32)cdLen);
	WriteInt32(&hdrBuff[16], (Int32)this->currOfst);
	WriteInt16(&hdrBuff[20], 0);
	this->stm->Write(hdrBuff, 22);
}

Bool IO::ZIPBuilder::AddFile(Text::CString fileName, const UInt8 *fileContent, UOSInt fileSize, Int64 fileTimeTicks, Data::Compress::Inflate::CompressionLevel compLevel)
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
	Data::DateTime dt;
	dt.SetTicks(fileTimeTicks);
	dt.ToLocalTime();
	WriteInt32(&hdrBuff[0], 0x04034b50);
	WriteInt16(&hdrBuff[4], 20);
	WriteInt16(&hdrBuff[6], 0x800);
	WriteInt16(&hdrBuff[8], 0x8);
	WriteInt16(&hdrBuff[10], dt.ToMSDOSTime());
	WriteInt16(&hdrBuff[12], dt.ToMSDOSDate());
	WriteUInt32(&hdrBuff[14], crcVal);
	WriteInt32(&hdrBuff[18], (Int32)compSize);
	WriteInt32(&hdrBuff[22], (Int32)fileSize);
	WriteInt16(&hdrBuff[26], (Int32)fileName.leng);
	WriteInt16(&hdrBuff[28], 0);
	MemCopyNO(&hdrBuff[30], fileName.v, fileName.leng);
	hdrLen = 30 + fileName.leng;
	#if _OSINT_SIZE > 32
	if (compSize >= 0x100000000LL || fileSize >= 0x100000000LL)
	{
		WriteInt16(&hdrBuff[28], 20);
		WriteInt16(&hdrBuff[hdrLen], 1);
		WriteInt16(&hdrBuff[hdrLen + 2], 16);
		WriteUInt64(&hdrBuff[hdrLen + 4], compSize);
		WriteUInt64(&hdrBuff[hdrLen + 12], fileSize);
		if (compSize >= fileSize)
		{
			WriteUInt64(&hdrBuff[hdrLen + 4], fileSize);
		}
		hdrLen += 20;
	}
	#endif

	IO::ZIPBuilder::FileInfo *file;
	Bool succ = false;
	file = MemAlloc(IO::ZIPBuilder::FileInfo, 1);
	file->fileName = Text::String::New(fileName);
	file->fileTimeTicks = fileTimeTicks;
	file->crcVal = crcVal;
	file->uncompSize = fileSize;
	file->compMeth = 8;
	file->compSize = compSize;
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
