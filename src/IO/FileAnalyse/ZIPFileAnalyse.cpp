#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/ZIPFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

Text::CString IO::FileAnalyse::ZIPFileAnalyse::GetTagName(UInt32 tagType)
{
	switch (tagType)
	{
	case 0:
		return CSTR("File Data");
	case 0x504B0102:
		return CSTR("Central directory file header");
	case 0x504B0304:
		return CSTR("Local File Header");
	case 0x504B0505:
		return CSTR("Signature Header");
	case 0x504B0506:
		return CSTR("End of central directory record");
	case 0x504B0606:
		return CSTR("Zip64 End of central directory record");
	case 0x504B0607:
		return CSTR("Zip64 end of central directory locator");
	case 0x504B0708:
		return CSTR("Data Descriptor");
	}
	return CSTR_NULL;
}

Text::CString IO::FileAnalyse::ZIPFileAnalyse::GetCompName(UInt16 comp)
{
	switch (comp)
	{
	case 0:
		return CSTR("Store");
	case 1:
		return CSTR("Shrunk");
	case 2:
		return CSTR("Reduced with compression factor 1");
	case 3:
		return CSTR("Reduced with compression factor 2");
	case 4:
		return CSTR("Reduced with compression factor 3");
	case 5:
		return CSTR("Reduced with compression factor 4");
	case 6:
		return CSTR("Imploded");
	case 7:
		return CSTR("Tokenizing compression algorithm");
	case 8:
		return CSTR("Deflate");
	case 9:
		return CSTR("Deflate64");
	case 10:
		return CSTR("PKWARE Data Compression Library Imploding");
	case 11:
		return CSTR("Reserved by PKWARE");
	case 12:
		return CSTR("BZIP2 algorithm");
	case 13:
		return CSTR("Reserved by PKWARE");
	case 14:
		return CSTR("LZMA");
	case 15:
		return CSTR("Reserved by PKWARE");
	case 16:
		return CSTR("IBM z/OS CMPSC Compression");
	case 17:
		return CSTR("Reserved by PKWARE");
	case 18:
		return CSTR("IBM TERSE");
	case 19:
		return CSTR("IBM LZ77 z Architecture");
	case 20:
		return CSTR("deprecated");
	case 93:
		return CSTR("Zstandard (zstd)");
	case 94:
		return CSTR("MP3");
	case 95:
		return CSTR("XZ");
	case 96:
		return CSTR("JPEG variant");
	case 97:
		return CSTR("WavPack");
	case 98:
		return CSTR("PPMd version I, Rev 1");
	case 99:
		return CSTR("AE-x encryption marker");
	}
	return CSTR_NULL;
}

UOSInt IO::FileAnalyse::ZIPFileAnalyse::ParseCentDir(const UInt8 *buff, UOSInt buffSize, UInt64 ofst)
{
	UInt64 compSize;
	UInt64 uncompSize;
	UInt16 fnameLen;
	UInt16 extraLen;
	UInt16 commentLen;
	UInt32 recType;
	IO::FileAnalyse::ZIPFileAnalyse::ZIPRecord *rec;
	UOSInt i;
	i = 0;
	while (i < buffSize)
	{
		if (i + 46 > buffSize)
		{
			return i;
		}
		recType = ReadMUInt32(&buff[i]);
		if (recType != 0x504B0102)
		{
			break;
		}
		compSize = ReadUInt32(&buff[i + 20]);
		uncompSize = ReadUInt32(&buff[i + 24]);
		fnameLen = ReadUInt16(&buff[i + 28]);
		extraLen = ReadUInt16(&buff[i + 30]);
		commentLen = ReadUInt16(&buff[i + 32]);
		ofst = ReadUInt32(&buff[i + 42]);

		if (i + 46 + (UOSInt)fnameLen + extraLen + commentLen > buffSize)
		{
			return i;
		}
		if (extraLen > 0)
		{
			const UInt8 *extraBuff = &buff[i + 46 + (UOSInt)fnameLen];
			UOSInt j = 0;
			UInt16 extraTag;
			UInt16 extraSize;
			while (j + 4 <= extraLen)
			{
				extraTag = ReadUInt16(&extraBuff[j]);
				extraSize = ReadUInt16(&extraBuff[j + 2]);
				if (extraTag == 1)
				{
					const UInt8 *zip64Info = &extraBuff[j + 4];
					if (uncompSize == 0xffffffff)
					{
						uncompSize = ReadUInt64(zip64Info);
						zip64Info += 8;
					}
					if (compSize == 0xffffffff)
					{
						compSize = ReadUInt64(zip64Info);
						zip64Info += 8;
					}
					if (ofst == 0xffffffff)
					{
						ofst = ReadUInt64(zip64Info);
						zip64Info += 8;
					}
				}
				j += 4 + (UOSInt)extraSize;
			}
		}
		rec = MemAlloc(ZIPRecord, 1);
		rec->tagType = 0x504B0304;
		rec->ofst = ofst;
		rec->size = 30 + (UOSInt)fnameLen;
		this->tags.Add(rec);

		rec = MemAlloc(ZIPRecord, 1);
		rec->tagType = 0;
		rec->ofst = ofst + 30 + fnameLen;
		rec->size = compSize;
		this->tags.Add(rec);

		i += 46 + (UOSInt)fnameLen + extraLen + commentLen;
	}
	return i;
}

UOSInt IO::FileAnalyse::ZIPFileAnalyse::AddCentDir(const UInt8 *buff, UOSInt buffSize, UInt64 ofst)
{
	UInt32 recType;
	UInt16 fnameLen;
	UInt16 extraLen;
	UInt16 commentLen;
	IO::FileAnalyse::ZIPFileAnalyse::ZIPRecord *rec;
	UOSInt i;
	i = 0;
	while (i < buffSize)
	{
		if (i + 46 > buffSize)
		{
			return i;
		}
		recType = ReadMUInt32(&buff[i]);
		if (recType != 0x504B0102)
		{
			break;
		}
		fnameLen = ReadUInt16(&buff[i + 28]);
		extraLen = ReadUInt16(&buff[i + 30]);
		commentLen = ReadUInt16(&buff[i + 32]);

		if (i + 46 + (UOSInt)fnameLen + extraLen + commentLen > buffSize)
		{
			return i;
		}

		rec = MemAlloc(ZIPRecord, 1);
		rec->tagType = recType;
		rec->ofst = ofst + i;
		rec->size = 46 + (UOSInt)fnameLen + extraLen + commentLen;
		this->tags.Add(rec);

		i += 46 + (UOSInt)fnameLen + extraLen + commentLen;
	}
	return i;
}

UInt32 __stdcall IO::FileAnalyse::ZIPFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::ZIPFileAnalyse *me = (IO::FileAnalyse::ZIPFileAnalyse*)userObj;
	UInt64 dataSize;
	UInt64 ofst;
	UInt8 recHdr[64];
	UInt8 z64eocdl[20];
	UInt8 z64eocd[56];
	UInt32 recType;
	UInt32 compSize;
	UInt32 uncompSize;
	UInt16 fnameLen;
	UInt16 extraLen;
	UInt16 commentLen;

	UOSInt i;
	IO::FileAnalyse::ZIPFileAnalyse::ZIPRecord *rec;
	me->threadRunning = true;
	me->threadStarted = true;
	ofst = 0;
	dataSize = me->fd->GetDataSize();
	if (me->fd->GetRealData(dataSize - 22, 22, recHdr) == 22)
	{
		recType = ReadMUInt32(recHdr);
		if (recType == 0x504B0506)
		{
			UInt32 sizeOfDir = ReadUInt32(&recHdr[12]);
			UInt32 ofstOfDir = ReadUInt32(&recHdr[16]);
			if (sizeOfDir == 0xffffffff || ofstOfDir == 0xffffffff)
			{
				me->fd->GetRealData(dataSize - 42, 20, z64eocdl);

				if (ReadMUInt32(z64eocdl) == 0x504B0607)
				{
					UInt64 z64eocdOfst = ReadUInt64(&z64eocdl[8]);
					me->fd->GetRealData(z64eocdOfst, 56, z64eocd);
					if (ReadMUInt32(z64eocd) == 0x504B0606)
					{
						UInt64 cdSize = ReadUInt64(&z64eocd[40]);
						UInt64 cdOfst = ReadUInt64(&z64eocd[48]);
						UInt8 *cdBuff;
						if (cdSize <= 1048576)
						{
							cdBuff = MemAlloc(UInt8, (UOSInt)cdSize);
							me->fd->GetRealData(cdOfst, (UOSInt)cdSize, cdBuff);
							me->ParseCentDir(cdBuff, (UOSInt)cdSize, cdOfst);
							me->AddCentDir(cdBuff, (UOSInt)cdSize, cdOfst);
							MemFree(cdBuff);
						}
						else
						{
							cdBuff = MemAlloc(UInt8, 1048576);
							UOSInt buffSize = 0;
							ofst = 0;
							while (ofst < cdSize)
							{
								if (1048576 < cdSize - ofst)
								{
									i = me->fd->GetRealData(cdOfst + ofst + buffSize, 1048576 - buffSize, &cdBuff[buffSize]);
									buffSize += i;
								}
								else
								{
									i = me->fd->GetRealData(cdOfst + ofst + buffSize, (UOSInt)(cdSize - ofst), &cdBuff[buffSize]);
									buffSize += i;
								}
								if (i == 0)
								{
									break;
								}
								i = me->ParseCentDir(cdBuff, buffSize, cdOfst + ofst);
								if (i == 0)
								{
									break;
								}
								if (i == buffSize)
								{
									ofst += i;
									buffSize = 0;
								}
								else
								{
									ofst += i;
									MemCopyO(cdBuff, &cdBuff[i], buffSize - i);
									buffSize -= i;
								}
							}

							buffSize = 0;
							ofst = 0;
							while (ofst < cdSize)
							{
								if (1048576 < cdSize - ofst)
								{
									i = me->fd->GetRealData(cdOfst + ofst + buffSize, 1048576 - buffSize, &cdBuff[buffSize]);
									buffSize += i;
								}
								else
								{
									i = me->fd->GetRealData(cdOfst + ofst + buffSize, (UOSInt)(cdSize - ofst), &cdBuff[buffSize]);
									buffSize += i;
								}
								if (i == 0)
								{
									break;
								}
								i = me->AddCentDir(cdBuff, buffSize, cdOfst + ofst);
								if (i == 0)
								{
									break;
								}
								if (i == buffSize)
								{
									ofst += i;
									buffSize = 0;
								}
								else
								{
									ofst += i;
									MemCopyO(cdBuff, &cdBuff[i], buffSize - i);
									buffSize -= i;
								}
							}
							MemFree(cdBuff);
						}
						
						rec = MemAlloc(ZIPRecord, 1);
						rec->tagType = 0x504B0606;
						rec->ofst = z64eocdOfst;
						rec->size = 56;
						me->tags.Add(rec);

						rec = MemAlloc(ZIPRecord, 1);
						rec->tagType = 0x504B0607;
						rec->ofst = dataSize - 42;
						rec->size = 20;
						me->tags.Add(rec);

						commentLen = ReadUInt16(&recHdr[20]);
						rec = MemAlloc(ZIPRecord, 1);
						rec->tagType = 0x504B0506;
						rec->ofst = dataSize - 22;
						rec->size = 22 + (UOSInt)commentLen;
						me->tags.Add(rec);

						me->threadRunning = false;
						return 0;
					}
				}
			}
			else
			{
				UInt8 *centDir = MemAlloc(UInt8, sizeOfDir);
				if (me->fd->GetRealData(ofstOfDir, sizeOfDir, centDir) == sizeOfDir)
				{
					me->ParseCentDir(centDir, sizeOfDir, 0);
					me->AddCentDir(centDir, sizeOfDir, ofstOfDir);
					MemFree(centDir);

					commentLen = ReadUInt16(&recHdr[20]);
					rec = MemAlloc(ZIPRecord, 1);
					rec->tagType = 0x504B0506;
					rec->ofst = dataSize - 22;
					rec->size = 22 + (UOSInt)commentLen;
					me->tags.Add(rec);

					me->threadRunning = false;
					return 0;
				}
				else
				{
					MemFree(centDir);
				}
			}
		}
	}
	
	printf("ZIPFileAnalyse: Scanning\r\n");
	ofst = 0;
	while (ofst < dataSize - 12 && !me->threadToStop)
	{
		if (me->fd->GetRealData(ofst, 64, recHdr) < 12)
			break;
		
		recType = ReadMUInt32(recHdr);
		if (recType == 0x504B0304)
		{
			compSize = ReadUInt32(&recHdr[18]);
			uncompSize = ReadUInt32(&recHdr[22]);
			fnameLen = ReadUInt16(&recHdr[26]);
			extraLen = ReadUInt16(&recHdr[28]);
			if (compSize == 0xFFFFFFFF || uncompSize == 0xFFFFFFFF)
			{
				break;
			}
			else
			{
				rec = MemAlloc(ZIPRecord, 1);
				rec->tagType = recType;
				rec->ofst = ofst;
				rec->size = 30 + (UOSInt)fnameLen + extraLen;
				me->tags.Add(rec);

				rec = MemAlloc(ZIPRecord, 1);
				rec->tagType = 0;
				rec->ofst = ofst + 30 + fnameLen + extraLen;
				rec->size = compSize;
				me->tags.Add(rec);
				ofst += 30 + (UOSInt)fnameLen + extraLen + compSize;
			}
		}
		else if (recType == 0x504B0102)
		{
			compSize = ReadUInt32(&recHdr[20]);
			uncompSize = ReadUInt32(&recHdr[24]);
			fnameLen = ReadUInt16(&recHdr[28]);
			extraLen = ReadUInt16(&recHdr[30]);
			commentLen = ReadUInt16(&recHdr[32]);
			if (compSize == 0xFFFFFFFF || uncompSize == 0xFFFFFFFF)
			{
				break;
			}
			else
			{
				rec = MemAlloc(ZIPRecord, 1);
				rec->tagType = recType;
				rec->ofst = ofst;
				rec->size = 46 + (UOSInt)fnameLen + extraLen + commentLen;
				me->tags.Add(rec);
				ofst += 46 + (UOSInt)fnameLen + extraLen + commentLen;
			}
		}
		else if (recType == 0x504B0506)
		{
			UInt16 commentLen = ReadUInt16(&recHdr[20]);
			rec = MemAlloc(ZIPRecord, 1);
			rec->tagType = recType;
			rec->ofst = ofst;
			rec->size = 22 + (UOSInt)commentLen;
			me->tags.Add(rec);
			ofst += 22 + (UOSInt)commentLen;
		}
		else
		{
			break;
		}
	}
	
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::ZIPFileAnalyse::ZIPFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	UInt64 fileLength = fd->GetDataSize();
	fd->GetRealData(fileLength - 22, 22, buff);
	if (ReadMInt32(buff) != 0x504B0506)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());

	Sync::Thread::Create(ParseThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
}

IO::FileAnalyse::ZIPFileAnalyse::~ZIPFileAnalyse()
{
	if (this->threadRunning)
	{
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}

	SDEL_CLASS(this->fd);
	LIST_FREE_FUNC(&this->tags, MemFree);
}

Text::CString IO::FileAnalyse::ZIPFileAnalyse::GetFormatName()
{
	return CSTR("ZIP");
}

UOSInt IO::FileAnalyse::ZIPFileAnalyse::GetFrameCount()
{
	return this->tags.GetCount();
}

Bool IO::FileAnalyse::ZIPFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::ZIPFileAnalyse::ZIPRecord *tag = this->tags.GetItem(index);
	Text::CString name;
	if (tag == 0)
		return false;
	sb->AppendU64(tag->ofst);
	sb->AppendC(UTF8STRC(": Type=0x"));
	sb->AppendHex32(tag->tagType);
	name = GetTagName(tag->tagType);
	if (name.v)
	{
		sb->AppendC(UTF8STRC(" ("));
		sb->Append(name);
		sb->AppendC(UTF8STRC(")"));
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUOSInt(tag->size);
	return true;
}


UOSInt IO::FileAnalyse::ZIPFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->tags.GetCount() - 1;
	OSInt k;
	ZIPRecord *pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->tags.GetItem((UOSInt)k);
		if (ofst < pack->ofst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->ofst + pack->size)
		{
			i = k + 1;
		}
		else
		{
			return (UOSInt)k;
		}
	}
	return INVALID_INDEX;
}

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::ZIPFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::FrameDetail *frame;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	UInt8 *tagData;
	IO::FileAnalyse::ZIPFileAnalyse::ZIPRecord *tag = this->tags.GetItem(index);
	if (tag == 0)
		return 0;
	
	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(tag->ofst, (UInt32)tag->size));
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Packet ")), index);
	frame->AddHeader(CSTRP(sbuff, sptr));

	if (tag->tagType == 0)
	{
		frame->AddField(0, tag->size, CSTR("File Data"), CSTR(""));
		return frame;
	}
	UInt32 compSize;
	UInt32 uncompSize;
	UInt16 fnameLen;
	UInt16 extraLen;
	UInt16 commentLen;
	UInt32 ofst;
	UOSInt i;
	tagData = MemAlloc(UInt8, tag->size);
	this->fd->GetRealData(tag->ofst, tag->size, tagData);
	frame->AddField(0, 4, CSTR("Record Type"), GetTagName(tag->tagType));
	switch (tag->tagType)
	{
	case 0x504B0304:
		frame->AddUInt(4, 2, CSTR("Version needed to extract"), ReadUInt16(&tagData[4]));
		frame->AddHex16(6, CSTR("General purpose bit flag"), ReadUInt16(&tagData[6]));
		frame->AddUIntName(8, 2, CSTR("Compression method"), ReadUInt16(&tagData[8]), GetCompName(ReadUInt16(&tagData[8])));
		frame->AddUInt(10, 2, CSTR("File last modification time"), ReadUInt16(&tagData[10]));
		frame->AddUInt(12, 2, CSTR("File last modification date"), ReadUInt16(&tagData[12]));
		frame->AddHex32(14, CSTR("CRC-32 of uncompressed data"), ReadUInt32(&tagData[14]));
		frame->AddUInt(18, 4, CSTR("Compressed size"), ReadUInt32(&tagData[18]));
		frame->AddUInt(22, 4, CSTR("Uncompressed size"), ReadUInt32(&tagData[22]));
		frame->AddUInt(26, 2, CSTR("File name length"), ReadUInt16(&tagData[26]));
		frame->AddUInt(28, 2, CSTR("Extra field length"), ReadUInt16(&tagData[28]));
		frame->AddStrC(30, ReadUInt16(&tagData[26]), CSTR("File name"), &tagData[30]);
		break;
	case 0x504B0102:
		frame->AddUInt(4, 2, CSTR("Version made by"), ReadUInt16(&tagData[4]));
		frame->AddUInt(6, 2, CSTR("Version needed to extract"), ReadUInt16(&tagData[6]));
		frame->AddHex16(8, CSTR("General purpose bit flag"), ReadUInt16(&tagData[8]));
		frame->AddUIntName(10, 2, CSTR("Compression method"), ReadUInt16(&tagData[10]), GetCompName(ReadUInt16(&tagData[10])));
		frame->AddUInt(12, 2, CSTR("File last modification time"), ReadUInt16(&tagData[12]));
		frame->AddUInt(14, 2, CSTR("File last modification date"), ReadUInt16(&tagData[14]));
		frame->AddHex32(16, CSTR("CRC-32 of uncompressed data"), ReadUInt32(&tagData[16]));
		compSize = ReadUInt32(&tagData[20]);
		if (compSize == 0xffffffff)
		{
			frame->AddField(20, 4, CSTR("Compressed size"), CSTR("Use Zip64 value"));
		}
		else
		{
			frame->AddUInt(20, 4, CSTR("Compressed size"), compSize);
		}
		uncompSize = ReadUInt32(&tagData[24]);
		if (uncompSize == 0xffffffff)
		{
			frame->AddField(24, 4, CSTR("Uncompressed size"), CSTR("Use Zip64 value"));
		}
		else
		{
			frame->AddUInt(24, 4, CSTR("Uncompressed size"), uncompSize);
		}
		frame->AddUInt(28, 2, CSTR("File name length"), fnameLen = ReadUInt16(&tagData[28]));
		frame->AddUInt(30, 2, CSTR("Extra field length"), extraLen = ReadUInt16(&tagData[30]));
		frame->AddUInt(32, 2, CSTR("File comment length"), commentLen = ReadUInt16(&tagData[32]));
		frame->AddUInt(34, 2, CSTR("Disk number where file starts"), ReadUInt16(&tagData[34]));
		frame->AddHex16(36, CSTR("Internal file attributes"), ReadUInt16(&tagData[36]));
		frame->AddHex32(38, CSTR("External file attributes"), ReadUInt32(&tagData[38]));
		ofst = ReadUInt32(&tagData[42]);
		if (ofst == 0xffffffff)
		{
			frame->AddField(42, 4, CSTR("Relative offset of local file header"), CSTR("Use Zip64 value"));
		}
		else
		{
			frame->AddUInt(42, 4, CSTR("Relative offset of local file header"), ofst);
		}
		frame->AddStrC(46, fnameLen, CSTR("File name"), &tagData[46]);
		i = 46 + (UOSInt)fnameLen;
		if (extraLen)
		{
			UInt16 extraTag;
			UInt16 extraSize;
			UOSInt j = 0;
			UOSInt k;
			UOSInt l;
			while (j + 4 <= extraLen)
			{
				extraTag = ReadUInt16(&tagData[i + j]);
				extraSize = ReadUInt16(&tagData[i + j + 2]);
				frame->AddUInt(i + j, 2, CSTR("Extra Tag"), extraTag);
				frame->AddUInt(i + j + 2, 2, CSTR("Extra Size"), extraSize);
				if (extraTag == 1)
				{
					k = i + j + 4;
					if (uncompSize == 0xffffffff)
					{
						frame->AddUInt64(k, CSTR("Original Size"), ReadUInt64(&tagData[k]));
						k += 8;
					}
					if (compSize == 0xffffffff)
					{
						frame->AddUInt64(k, CSTR("Compressed Size"), ReadUInt64(&tagData[k]));
						k += 8;
					}
					if (ofst == 0xffffffff)
					{
						frame->AddUInt64(k, CSTR("Relative Header Offset"), ReadUInt64(&tagData[k]));
						k += 8;
					}
				}
				else if (extraTag == 10)
				{
					UInt16 ntfsTag;
					UInt16 ntfsSize;
					k = i + j + 4;
					l = k + extraSize;
					frame->AddUInt(k, 4, CSTR("Reserved"), ReadUInt32(&tagData[k]));
					k += 4;
					while (k + 4 <= l)
					{
						ntfsTag = ReadUInt16(&tagData[k]);
						ntfsSize = ReadUInt16(&tagData[k + 2]);
						frame->AddUInt(k, 2, CSTR("NTFS attribute tag"), ntfsTag);
						frame->AddUInt(k + 2, 2, CSTR("Size of attribute"), ntfsSize);
						k += 4;
						if (ntfsTag == 1 && ntfsSize == 24)
						{
							frame->AddUInt64(k, CSTR("File last modification time"), ReadUInt64(&tagData[k]));
							frame->AddUInt64(k + 8, CSTR("File last access time"), ReadUInt64(&tagData[k + 8]));
							frame->AddUInt64(k + 16, CSTR("File creation time"), ReadUInt64(&tagData[k + 16]));
						}
						k += ntfsSize;
					}
				}
				j += 4 + (UOSInt)extraSize;
			}
			i += extraLen;
		}
		if (commentLen)
		{
			frame->AddStrC(i, commentLen, CSTR("File comment"), &tagData[i]);
		}
		break;
	case 0x504B0506:
		frame->AddUInt(4, 2, CSTR("Number of this disk"), ReadUInt16(&tagData[4]));
		frame->AddUInt(6, 2, CSTR("Disk where central directory starts"), ReadUInt16(&tagData[6]));
		frame->AddUInt(8, 2, CSTR("Number of central directory records on this disk"), ReadUInt16(&tagData[8]));
		frame->AddUInt(10, 2, CSTR("Total number of central directory records"), ReadUInt16(&tagData[10]));
		frame->AddUInt(12, 4, CSTR("Size of central directory"), ReadUInt32(&tagData[12]));
		frame->AddUInt(16, 4, CSTR("Offset of start of central directory"), ReadUInt32(&tagData[16]));
		frame->AddUInt(20, 2, CSTR("Comment length"), commentLen = ReadUInt16(&tagData[20]));
		if (commentLen)
		{
			frame->AddStrC(22, commentLen, CSTR("Comment"), &tagData[22]);
		}
		break;
	case 0x504B0606:
		frame->AddUInt64(4, CSTR("Size of zip64 end of central directory record"), ReadUInt64(&tagData[4]));
		frame->AddUInt(12, 2, CSTR("Version made by"), ReadUInt16(&tagData[12]));
		frame->AddUInt(14, 2, CSTR("Version needed to extract"), ReadUInt16(&tagData[14]));
		frame->AddUInt(16, 4, CSTR("Number of this disk"), ReadUInt32(&tagData[16]));
		frame->AddUInt(20, 4, CSTR("Number of the disk with the start of the central directory"), ReadUInt32(&tagData[20]));
		frame->AddUInt64(24, CSTR("Total number of entries in the central directory on this disk"), ReadUInt64(&tagData[24]));
		frame->AddUInt64(32, CSTR("Total number of entries in the central directory"), ReadUInt64(&tagData[32]));
		frame->AddUInt64(40, CSTR("Size of the central directory"), ReadUInt64(&tagData[40]));
		frame->AddUInt64(48, CSTR("Offset of start of central directory with respect to the starting disk number"), ReadUInt64(&tagData[48]));
		break;
	case 0x504B0607:
		frame->AddUInt(4, 4, CSTR("Number of the disk with the start of the zip64 end of central directory"), ReadUInt32(&tagData[4]));
		frame->AddUInt64(8, CSTR("Relative offset of the zip64 end of central directory record"), ReadUInt64(&tagData[8]));
		frame->AddUInt(16, 4, CSTR("Total number of disks"), ReadUInt32(&tagData[16]));
		break;
	}
	MemFree(tagData);
	return frame;
}

Bool IO::FileAnalyse::ZIPFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::ZIPFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::ZIPFileAnalyse::TrimPadding(Text::CString outputFile)
{
	return false;
}
