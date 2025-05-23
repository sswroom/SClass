#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/ZIPFileAnalyse.h"
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

UOSInt IO::FileAnalyse::ZIPFileAnalyse::ParseCentDir(UnsafeArray<const UInt8> buff, UOSInt buffSize, UInt64 ofst)
{
	UInt64 compSize;
	UInt64 uncompSize;
	UInt16 fnameLen;
	UInt16 extraLen;
	UInt16 commentLen;
	UInt32 recType;
	NN<IO::FileAnalyse::ZIPFileAnalyse::ZIPRecord> rec;
	UInt8 headerBuff[30];
	UOSInt i;
	i = 0;
	while (i < buffSize)
	{
		if (i + 46 > buffSize)
		{
			return i;
		}
		recType = ReadMUInt32(&buff[i]);
		if (recType != 0x504B0102 || this->thread.IsStopping())
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
		this->fd->GetRealData(ofst, 30, BYTEARR(headerBuff));
		UOSInt extraSize = ReadUInt16(&headerBuff[28]);
		rec = MemAllocNN(ZIPRecord);
		rec->tagType = 0x504B0304;
		rec->ofst = ofst;
		rec->size = 30 + extraSize + (UOSInt)fnameLen;
		rec->fileName = 0;
		this->tags.Add(rec);

		if (compSize > 0)
		{
			rec = MemAllocNN(ZIPRecord);
			rec->tagType = 0;
			rec->ofst = ofst + 30 + extraSize + fnameLen;
			rec->size = compSize;
			rec->fileName = Text::String::New(&buff[i + 46], fnameLen).Ptr();
			this->tags.Add(rec);
		}

		i += 46 + (UOSInt)fnameLen + extraLen + commentLen;
	}
	return i;
}

UOSInt IO::FileAnalyse::ZIPFileAnalyse::AddCentDir(UnsafeArray<const UInt8> buff, UOSInt buffSize, UInt64 ofst)
{
	UInt32 recType;
	UInt16 fnameLen;
	UInt16 extraLen;
	UInt16 commentLen;
	NN<IO::FileAnalyse::ZIPFileAnalyse::ZIPRecord> rec;
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

		rec = MemAllocNN(ZIPRecord);
		rec->tagType = recType;
		rec->ofst = ofst + i;
		rec->size = 46 + (UOSInt)fnameLen + extraLen + commentLen;
		rec->fileName = 0;
		this->tags.Add(rec);

		i += 46 + (UOSInt)fnameLen + extraLen + commentLen;
	}
	return i;
}

void __stdcall IO::FileAnalyse::ZIPFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::ZIPFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::ZIPFileAnalyse>();
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
	NN<IO::FileAnalyse::ZIPFileAnalyse::ZIPRecord> rec;
	ofst = 0;
	dataSize = me->fd->GetDataSize();
	if (me->fd->GetRealData(dataSize - 22, 22, BYTEARR(recHdr)) == 22)
	{
		recType = ReadMUInt32(recHdr);
		if (recType == 0x504B0506)
		{
			UInt32 sizeOfDir = ReadUInt32(&recHdr[12]);
			UInt32 ofstOfDir = ReadUInt32(&recHdr[16]);
			if (sizeOfDir == 0xffffffff || ofstOfDir == 0xffffffff)
			{
				me->fd->GetRealData(dataSize - 42, 20, BYTEARR(z64eocdl));

				if (ReadMUInt32(z64eocdl) == 0x504B0607)
				{
					UInt64 z64eocdOfst = ReadUInt64(&z64eocdl[8]);
					me->fd->GetRealData(z64eocdOfst, 56, BYTEARR(z64eocd));
					if (ReadMUInt32(z64eocd) == 0x504B0606)
					{
						UInt64 cdSize = ReadUInt64(&z64eocd[40]);
						UInt64 cdOfst = ReadUInt64(&z64eocd[48]);
						if (cdSize <= 1048576)
						{
							Data::ByteBuffer cdBuff((UOSInt)cdSize);
							me->fd->GetRealData(cdOfst, (UOSInt)cdSize, cdBuff);
							me->ParseCentDir(cdBuff.Arr(), (UOSInt)cdSize, cdOfst);
							me->AddCentDir(cdBuff.Arr(), (UOSInt)cdSize, cdOfst);
						}
						else
						{
							Data::ByteBuffer cdBuff(1048576);
							UOSInt buffSize = 0;
							ofst = 0;
							while (ofst < cdSize)
							{
								if (1048576 < cdSize - ofst)
								{
									i = me->fd->GetRealData(cdOfst + ofst + buffSize, 1048576 - buffSize, cdBuff.SubArray(buffSize));
									buffSize += i;
								}
								else
								{
									i = me->fd->GetRealData(cdOfst + ofst + buffSize, (UOSInt)(cdSize - ofst), cdBuff.SubArray(buffSize));
									buffSize += i;
								}
								if (i == 0)
								{
									break;
								}
								i = me->ParseCentDir(cdBuff.Arr(), buffSize, cdOfst + ofst);
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
									cdBuff.CopyInner(0, i, buffSize - i);
									buffSize -= i;
								}
							}

							buffSize = 0;
							ofst = 0;
							while (ofst < cdSize)
							{
								if (1048576 < cdSize - ofst)
								{
									i = me->fd->GetRealData(cdOfst + ofst + buffSize, 1048576 - buffSize, cdBuff.SubArray(buffSize));
									buffSize += i;
								}
								else
								{
									i = me->fd->GetRealData(cdOfst + ofst + buffSize, (UOSInt)(cdSize - ofst), cdBuff.SubArray(buffSize));
									buffSize += i;
								}
								if (i == 0)
								{
									break;
								}
								i = me->AddCentDir(cdBuff.Arr(), buffSize, cdOfst + ofst);
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
									cdBuff.CopyInner(0, i, buffSize - i);
									buffSize -= i;
								}
							}
						}
						
						rec = MemAllocNN(ZIPRecord);
						rec->tagType = 0x504B0606;
						rec->ofst = z64eocdOfst;
						rec->size = 56;
						rec->fileName = 0;
						me->tags.Add(rec);

						rec = MemAllocNN(ZIPRecord);
						rec->tagType = 0x504B0607;
						rec->ofst = dataSize - 42;
						rec->size = 20;
						rec->fileName = 0;
						me->tags.Add(rec);

						commentLen = ReadUInt16(&recHdr[20]);
						rec = MemAllocNN(ZIPRecord);
						rec->tagType = 0x504B0506;
						rec->ofst = dataSize - 22;
						rec->size = 22 + (UOSInt)commentLen;
						rec->fileName = 0;
						me->tags.Add(rec);
						return;
					}
				}
			}
			else
			{
				Data::ByteBuffer centDir(sizeOfDir);
				if (me->fd->GetRealData(ofstOfDir, sizeOfDir, centDir) == sizeOfDir)
				{
					me->ParseCentDir(centDir.Arr(), sizeOfDir, 0);
					me->AddCentDir(centDir.Arr(), sizeOfDir, ofstOfDir);

					commentLen = ReadUInt16(&recHdr[20]);
					rec = MemAllocNN(ZIPRecord);
					rec->tagType = 0x504B0506;
					rec->ofst = dataSize - 22;
					rec->size = 22 + (UOSInt)commentLen;
					rec->fileName = 0;
					me->tags.Add(rec);
					return;
				}
			}
		}
	}
	
	printf("ZIPFileAnalyse: Scanning\r\n");
	ofst = 0;
	while (ofst < dataSize - 12 && !thread->IsStopping())
	{
		if (me->fd->GetRealData(ofst, 64, BYTEARR(recHdr)) < 12)
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
				rec = MemAllocNN(ZIPRecord);
				rec->tagType = recType;
				rec->ofst = ofst;
				rec->size = 30 + (UOSInt)fnameLen + extraLen;
				rec->fileName = 0;
				me->tags.Add(rec);

				rec = MemAllocNN(ZIPRecord);
				rec->tagType = 0;
				rec->ofst = ofst + 30 + fnameLen + extraLen;
				rec->size = compSize;
				rec->fileName = 0;
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
				rec = MemAllocNN(ZIPRecord);
				rec->tagType = recType;
				rec->ofst = ofst;
				rec->size = 46 + (UOSInt)fnameLen + extraLen + commentLen;
				rec->fileName = 0;
				me->tags.Add(rec);
				ofst += 46 + (UOSInt)fnameLen + extraLen + commentLen;
			}
		}
		else if (recType == 0x504B0506)
		{
			UInt16 commentLen = ReadUInt16(&recHdr[20]);
			rec = MemAllocNN(ZIPRecord);
			rec->tagType = recType;
			rec->ofst = ofst;
			rec->size = 22 + (UOSInt)commentLen;
			rec->fileName = 0;
			me->tags.Add(rec);
			ofst += 22 + (UOSInt)commentLen;
		}
		else
		{
			break;
		}
	}
}

void IO::FileAnalyse::ZIPFileAnalyse::ParseExtraTag(NN<IO::FileAnalyse::FrameDetail> frame,Data::ByteArrayR tagData, UOSInt extraStart, UOSInt extraLen, UOSInt tagSize, UInt32 compSize, UInt32 uncompSize, UInt32 ofst)
{
	if (tagSize < extraStart + extraLen)
	{
		extraLen = tagSize - extraStart;
	}
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UInt64 t;
	UInt16 extraTag;
	UInt16 extraSize;
	UOSInt j = 0;
	UOSInt k;
	UOSInt l;
	while (j + 4 <= extraLen)
	{
		extraTag = ReadUInt16(&tagData[extraStart + j]);
		extraSize = ReadUInt16(&tagData[extraStart + j + 2]);
		frame->AddUInt(extraStart + j, 2, CSTR("Extra Tag"), extraTag);
		frame->AddUInt(extraStart + j + 2, 2, CSTR("Extra Size"), extraSize);
		if (extraTag == 1)
		{
			k = extraStart + j + 4;
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
			k = extraStart + j + 4;
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
					t = ReadUInt64(&tagData[k]);
					sptr = Data::Timestamp::FromFILETIME(&t, Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
					frame->AddUInt64Name(k, 8, CSTR("File last modification time"), t, CSTRP(sbuff, sptr));
					t = ReadUInt64(&tagData[k + 8]);
					sptr = Data::Timestamp::FromFILETIME(&t, Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
					frame->AddUInt64Name(k + 8, 8, CSTR("File last access time"), t, CSTRP(sbuff, sptr));
					t = ReadUInt64(&tagData[k + 16]);
					sptr = Data::Timestamp::FromFILETIME(&t, Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
					frame->AddUInt64Name(k + 16, 8, CSTR("File creation time"), t, CSTRP(sbuff, sptr));
				}
				k += ntfsSize;
			}
		}
		else if (extraTag == 0x5455)
		{
			k = extraStart + j + 4;
			l = k + extraSize;
			frame->AddUInt(k, 1, CSTR("Flags"), tagData[k]);
			if (k + 5 <= l)
			{
				t = ReadUInt64(&tagData[k + 1]);
				sptr = Data::Timestamp::FromEpochSec(ReadUInt32(&tagData[k + 1]), Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
				frame->AddUIntName(k + 1, 4, CSTR("File last modification time"), ReadUInt32(&tagData[k + 1]), CSTRP(sbuff, sptr));
			}
			if (k + 9 <= l)
			{
				t = ReadUInt64(&tagData[k + 5]);
				sptr = Data::Timestamp::FromEpochSec(ReadUInt32(&tagData[k + 5]), Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
				frame->AddUIntName(k + 5, 4, CSTR("File last access time"), ReadUInt32(&tagData[k + 5]), CSTRP(sbuff, sptr));
			}
			if (k + 13 <= l)
			{
				t = ReadUInt64(&tagData[k + 9]);
				sptr = Data::Timestamp::FromEpochSec(ReadUInt32(&tagData[k + 9]), Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
				frame->AddUIntName(k + 9, 4, CSTR("File creation time"), ReadUInt32(&tagData[k + 9]), CSTRP(sbuff, sptr));
			}
		}
		else if (extraTag == 0x7875)
		{
			k = extraStart + j + 4;
			frame->AddUInt(k, 1, CSTR("Version"), tagData[k]);
			k++;
			frame->AddUInt(k, 1, CSTR("UIDSize"), tagData[k]);
			frame->AddHexBuff(k + 1, CSTR("UID"), tagData.SubArray(k + 1, tagData[k]), false);
			k += 1 + (UOSInt)tagData[k];
			frame->AddUInt(k, 1, CSTR("GIDSize"), tagData[k]);
			frame->AddHexBuff(k + 1, CSTR("GID"), tagData.SubArray(k + 1, tagData[k]), false);
		}
		j += 4 + (UOSInt)extraSize;
	}
}

IO::FileAnalyse::ZIPFileAnalyse::ZIPFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("ZIPFileAnalyse"))
{
	UInt8 buff[256];
	this->fd = 0;
	this->pauseParsing = false;
	UInt64 fileLength = fd->GetDataSize();
	fd->GetRealData(fileLength - 22, 22, BYTEARR(buff));
	if (ReadMInt32(buff) != 0x504B0506)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::ZIPFileAnalyse::~ZIPFileAnalyse()
{
	this->thread.Stop();
	SDEL_CLASS(this->fd);
	UOSInt i = this->tags.GetCount();
	NN<ZIPRecord> tag;
	while (i-- > 0)
	{
		tag = this->tags.GetItemNoCheck(i);
		SDEL_STRING(tag->fileName);
		MemFreeNN(tag);
	}
}

Text::CStringNN IO::FileAnalyse::ZIPFileAnalyse::GetFormatName()
{
	return CSTR("ZIP");
}

UOSInt IO::FileAnalyse::ZIPFileAnalyse::GetFrameCount()
{
	return this->tags.GetCount();
}

Bool IO::FileAnalyse::ZIPFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::ZIPFileAnalyse::ZIPRecord> tag;
	Text::CStringNN name;
	if (!this->tags.GetItem(index).SetTo(tag))
		return false;
	sb->AppendU64(tag->ofst);
	sb->AppendC(UTF8STRC(": Type=0x"));
	sb->AppendHex32(tag->tagType);
	if (GetTagName(tag->tagType).SetTo(name))
	{
		sb->AppendC(UTF8STRC(" ("));
		sb->Append(name);
		sb->AppendC(UTF8STRC(")"));
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUOSInt(tag->size);
	if (tag->fileName)
	{
		sb->AppendC(UTF8STRC(", name="));
		sb->Append(tag->fileName);
	}
	return true;
}


UOSInt IO::FileAnalyse::ZIPFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->tags.GetCount() - 1;
	OSInt k;
	NN<ZIPRecord> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->tags.GetItemNoCheck((UOSInt)k);
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::ZIPFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::FileAnalyse::ZIPFileAnalyse::ZIPRecord> tag;
	if (!this->tags.GetItem(index).SetTo(tag))
		return 0;
	
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(tag->ofst, tag->size));
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
	Data::Timestamp ts;
	UOSInt i;
	Data::ByteBuffer tagData(tag->size);
	this->fd->GetRealData(tag->ofst, tag->size, tagData);
	frame->AddField(0, 4, CSTR("Record Type"), GetTagName(tag->tagType));
	switch (tag->tagType)
	{
	case 0x504B0304:
		frame->AddUInt(4, 2, CSTR("Version needed to extract"), ReadUInt16(&tagData[4]));
		frame->AddHex16(6, CSTR("General purpose bit flag"), ReadUInt16(&tagData[6]));
		frame->AddUIntName(8, 2, CSTR("Compression method"), ReadUInt16(&tagData[8]), GetCompName(ReadUInt16(&tagData[8])));
		ts = Data::Timestamp::FromMSDOSTime(ReadUInt16(&tagData[12]), ReadUInt16(&tagData[10]), Data::DateTimeUtil::GetLocalTzQhr());
		sptr = ts.ToString(sbuff, "HH:mm:ss");
		frame->AddUIntName(10, 2, CSTR("File last modification time"), ReadUInt16(&tagData[10]), CSTRP(sbuff, sptr));
		sptr = ts.ToString(sbuff, "yyyy-MM-dd");
		frame->AddUIntName(12, 2, CSTR("File last modification date"), ReadUInt16(&tagData[12]), CSTRP(sbuff, sptr));
		frame->AddHex32(14, CSTR("CRC-32 of uncompressed data"), ReadUInt32(&tagData[14]));
		frame->AddUInt(18, 4, CSTR("Compressed size"), compSize = ReadUInt32(&tagData[18]));
		frame->AddUInt(22, 4, CSTR("Uncompressed size"), uncompSize = ReadUInt32(&tagData[22]));
		frame->AddUInt(26, 2, CSTR("File name length"), fnameLen = ReadUInt16(&tagData[26]));
		frame->AddUInt(28, 2, CSTR("Extra field length"), extraLen = ReadUInt16(&tagData[28]));
		frame->AddStrC(30, fnameLen, CSTR("File name"), &tagData[30]);
		if (extraLen)
		{
			i = 30 + (UOSInt)fnameLen;
			ParseExtraTag(frame, tagData, i, extraLen, tag->size, compSize, uncompSize, 0);
		}
		break;
	case 0x504B0102:
		frame->AddUInt(4, 2, CSTR("Version made by"), ReadUInt16(&tagData[4]));
		frame->AddUInt(6, 2, CSTR("Version needed to extract"), ReadUInt16(&tagData[6]));
		frame->AddHex16(8, CSTR("General purpose bit flag"), ReadUInt16(&tagData[8]));
		frame->AddUIntName(10, 2, CSTR("Compression method"), ReadUInt16(&tagData[10]), GetCompName(ReadUInt16(&tagData[10])));
		ts = Data::Timestamp::FromMSDOSTime(ReadUInt16(&tagData[14]), ReadUInt16(&tagData[12]), Data::DateTimeUtil::GetLocalTzQhr());
		sptr = ts.ToString(sbuff, "HH:mm:ss");
		frame->AddUIntName(12, 2, CSTR("File last modification time"), ReadUInt16(&tagData[12]), CSTRP(sbuff, sptr));
		sptr = ts.ToString(sbuff, "yyyy-MM-dd");
		frame->AddUIntName(14, 2, CSTR("File last modification date"), ReadUInt16(&tagData[14]), CSTRP(sbuff, sptr));
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
			ParseExtraTag(frame, tagData, i, extraLen, tag->size, compSize, uncompSize, ofst);
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
	return frame;
}

Bool IO::FileAnalyse::ZIPFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::ZIPFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::ZIPFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
