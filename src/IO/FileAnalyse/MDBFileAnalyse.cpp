#include "Stdafx.h"
#include "Crypto/Encrypt/RC4Cipher.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/FileAnalyse/MDBFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/XLSUtil.h"

UInt32 __stdcall IO::FileAnalyse::MDBFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::MDBFileAnalyse *me = (IO::FileAnalyse::MDBFileAnalyse*)userObj;
	UInt8 readBuff[4096];
	UInt64 readOfst;
	UOSInt readSize;
	IO::FileAnalyse::MDBFileAnalyse::PackInfo *pack;

	me->threadRunning = true;
	me->threadStarted = true;
	readOfst = 0;
	while (!me->threadToStop)
	{
		if (me->pauseParsing)
		{
			Sync::Thread::Sleep(100);
		}
		else
		{
			readSize = me->fd->GetRealData(readOfst, 4096, readBuff);
			if (readSize != 4096)
			{
				break;
			}
			pack = MemAlloc(PackInfo, 1);
			pack->fileOfst = readOfst;
			pack->packSize = 4096;
			pack->packType = ReadUInt16(readBuff);
			me->packs->Add(pack);
			readOfst += 4096;
		}
	}
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::MDBFileAnalyse::MDBFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	NEW_CLASS(this->packs, Data::SyncArrayList<PackInfo*>());
	fd->GetRealData(0, 256, buff);
	if (ReadInt32(buff) != 0x00000100)
	{
		return;
	}
	if (Text::StrEquals((const UTF8Char*)"Standard Jet DB", &buff[4]))
	{
		this->fileFormat = 0;
	}
	else if (Text::StrEquals((const UTF8Char*)"Standard ACE DB", &buff[4]))
	{
		this->fileFormat = 1;
	}
	else
	{
		return;
	}
	this->fileVer = ReadUInt32(&buff[20]);
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	Sync::Thread::Create(ParseThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
}

IO::FileAnalyse::MDBFileAnalyse::~MDBFileAnalyse()
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
	LIST_FREE_FUNC(this->packs, MemFree);
	DEL_CLASS(this->packs);
}

const UTF8Char *IO::FileAnalyse::MDBFileAnalyse::GetFormatName()
{
	return (const UTF8Char*)"MDB";
}

UOSInt IO::FileAnalyse::MDBFileAnalyse::GetFrameCount()
{
	return this->packs->GetCount();
}

Bool IO::FileAnalyse::MDBFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::MDBFileAnalyse::PackInfo *pack;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->Append((const UTF8Char*)": Num=");
	sb->AppendUOSInt(index);
	sb->Append((const UTF8Char*)", Type=0x");
	sb->AppendHex16(pack->packType);
	switch (pack->packType)
	{
	case 0x100:
		sb->Append((const UTF8Char*)" (File Header)");
		break;
	case 0x101:
		sb->Append((const UTF8Char*)" (Data Page)");
		break;
	case 0x102:
		sb->Append((const UTF8Char*)" (Table Definition)");
		break;
	case 0x103:
		sb->Append((const UTF8Char*)" (Intermediate Index pages)");
		break;
	case 0x104:
		sb->Append((const UTF8Char*)" (Leaf Index pages)");
		break;
	case 0x105:
		sb->Append((const UTF8Char*)" (Page Usage Bitmaps)");
		break;
	}
	return true;
}

UOSInt IO::FileAnalyse::MDBFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	UOSInt index = (UOSInt)(ofst >> 12);
	if (index >= this->packs->GetCount())
	{
		return INVALID_INDEX;
	}
	return index;
}

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::MDBFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::FrameDetail *frame;
	IO::FileAnalyse::MDBFileAnalyse::PackInfo *pack;
	UTF8Char sbuff[64];
	UTF8Char sbuff2[256];
	UTF8Char *sptr;
	UInt8 packBuff[4096];
	UInt8 decBuff[128];
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return 0;

	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, (UInt32)pack->packSize));
	this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
	frame->AddHex16(0, "Frame Type", ReadUInt16(&packBuff[0]));
	frame->AddUInt(2, 2, "Free Space", ReadUInt16(&packBuff[2]));
	switch (pack->packType)
	{
	case 0x100:
		frame->AddStrS(4, 16, "File Format", &packBuff[4]);
		frame->AddHex32(20, "File Version", ReadUInt32(&packBuff[20]));
		{
			WriteUInt32(decBuff, 0x6b39dac7);
			Crypto::Encrypt::RC4Cipher rc4(decBuff, 4);
			if (this->fileVer == 0)
			{
				rc4.Decrypt(&packBuff[24], 126, decBuff, 0);
				Double ddays = ReadDouble(&decBuff[0x5A]);
				frame->AddHexBuff(24, 0x22, "Unknown", decBuff, true);
				frame->AddUInt(24 + 0x22, 2, "System Collation", ReadUInt16(&decBuff[0x22]));
				frame->AddUInt(24 + 0x24, 2, "System Codepage", ReadUInt16(&decBuff[0x24]));
				frame->AddHex32(24 + 0x26, "Database Key", ReadUInt32(&decBuff[0x26]));
				frame->AddStrS(24 + 0x2A, 20, "Database password", &decBuff[0x2A]);
				frame->AddHexBuff(24 + 0x3E, 24, "Unknown", &decBuff[0x3E], true);
				frame->AddUInt(24 + 0x56, 2, "Unknown", ReadUInt16(&decBuff[0x56]));
				Data::DateTime dt;
				Text::XLSUtil::Number2Date(&dt, ddays);
				dt.ToLocalTime();
				Text::StrConcat(dt.ToString(Text::StrConcat(Text::StrDouble(sbuff, ddays), (const UTF8Char*)" ("), "yyyy-MM-dd HH:mm:ss.fff"), (const UTF8Char*)")");
				frame->AddField(24 + 0x5A, 8, (const UTF8Char*)"Creation date", sbuff);
				frame->AddHexBuff(24 + 0x62, 28, "Unknown", &decBuff[0x62], true);
			}
			else
			{
				rc4.Decrypt(&packBuff[24], 128, decBuff, 0);
				Double ddays = ReadDouble(&decBuff[0x5A]);
				UInt32 ndays = (UInt32)ddays;
				frame->AddHexBuff(24, 0x24, "Unkonwn", decBuff, true);
				frame->AddUInt(24 + 0x24, 2, "System Codepage", ReadUInt16(&decBuff[0x24]));
				frame->AddHex32(24 + 0x26, "Database Key", ReadUInt32(&decBuff[0x26]));
				WriteUInt32(&decBuff[0x2A], ReadUInt32(&decBuff[0x2A]) ^ ndays);
				WriteUInt32(&decBuff[0x2E], ReadUInt32(&decBuff[0x2E]) ^ ndays);
				WriteUInt32(&decBuff[0x32], ReadUInt32(&decBuff[0x32]) ^ ndays);
				WriteUInt32(&decBuff[0x36], ReadUInt32(&decBuff[0x36]) ^ ndays);
				WriteUInt32(&decBuff[0x3A], ReadUInt32(&decBuff[0x3A]) ^ ndays);
				WriteUInt32(&decBuff[0x3E], ReadUInt32(&decBuff[0x3E]) ^ ndays);
				WriteUInt32(&decBuff[0x42], ReadUInt32(&decBuff[0x42]) ^ ndays);
				WriteUInt32(&decBuff[0x46], ReadUInt32(&decBuff[0x46]) ^ ndays);
				WriteUInt32(&decBuff[0x4A], ReadUInt32(&decBuff[0x4A]) ^ ndays);
				WriteUInt32(&decBuff[0x4E], ReadUInt32(&decBuff[0x4E]) ^ ndays);
				Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&decBuff[0x2A], 20)[0] = 0;
				frame->AddField(24 + 0x2A, 40, (const UTF8Char*)"Database Password", sbuff);
				frame->AddHex32(24 + 0x52, "Unknown", ReadUInt32(&decBuff[0x52]));
				frame->AddUInt(24 + 0x56, 4, "System Collation", ReadUInt32(&decBuff[0x56]));
				Data::DateTime dt;
				Text::XLSUtil::Number2Date(&dt, ddays);
				dt.ToLocalTime();
				Text::StrConcat(dt.ToString(Text::StrConcat(Text::StrDouble(sbuff, ddays), (const UTF8Char*)" ("), "yyyy-MM-dd HH:mm:ss.fff"), (const UTF8Char*)")");
				frame->AddField(24 + 0x5A, 8, (const UTF8Char*)"Creation date", sbuff);
				frame->AddHexBuff(24 + 0x62, 30, "Unknown", &decBuff[0x62], true);

				frame->AddUInt(152, 4, "Unknown", ReadUInt32(&packBuff[152]));
				frame->AddStrS(156, 4, "Unknown", &packBuff[156]);
			}
		}
		break;
	case 0x101:
		frame->AddUInt(4, 4, "Data Owner", ReadUInt32(&packBuff[4]));
		frame->AddUInt(8, 4, "Unknown", ReadUInt32(&packBuff[8]));
		frame->AddUInt(12, 2, "Record Count", ReadUInt16(&packBuff[12]));
		{
			UOSInt i = 0;
			UOSInt j = ReadUInt16(&packBuff[12]);
			UOSInt thisOfst;
			UOSInt lastOfst = 4096;
			while (i < j)
			{
				thisOfst = ReadUInt16(&packBuff[14 + i * 2]);
				sptr = Text::StrUOSInt(sbuff, thisOfst & 0xFFF);
				if (thisOfst & 0x4000)
				{
					sptr = Text::StrConcat(sptr, (const UTF8Char*)" (overflow)");
				}
				if (thisOfst & 0x8000)
				{
					sptr = Text::StrConcat(sptr, (const UTF8Char*)" (deleted)");
				}
				frame->AddField(14 + i * 2, 2, (const UTF8Char*)"Record Offset", sbuff);
				Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Record "), i);
				thisOfst &= 0xfff;
				frame->AddHexBuff(thisOfst, lastOfst - thisOfst, (const Char*)sbuff, &packBuff[thisOfst], true);
				lastOfst = thisOfst;
				i++;
			}
			frame->AddHexBuff(14 + j * 2, ReadUInt16(&packBuff[2]), "Free Space", &packBuff[14 + j * 2], true);
		}
		break;
	case 0x102:
		if (this->fileVer == 0)
		{
			frame->AddUInt(4, 4, "Next Page", ReadUInt32(&packBuff[4]));
			frame->AddUInt(8, 4, "Table Length", ReadUInt32(&packBuff[8]));
			frame->AddUInt(12, 4, "Number of records", ReadUInt32(&packBuff[12]));
			frame->AddUInt(16, 4, "Next Autonunber", ReadUInt32(&packBuff[16]));

			UOSInt freeSpace = ReadUInt16(&packBuff[2]);
			frame->AddHexBuff(4096 - freeSpace, freeSpace, "Free Space", &packBuff[4096 - freeSpace], true);
		}
		else //1
		{
			frame->AddUInt(4, 4, "Next Page", ReadUInt32(&packBuff[4]));
			frame->AddUInt(8, 4, "Table Length", ReadUInt32(&packBuff[8]));
			frame->AddUInt(12, 4, "Unknown", ReadUInt32(&packBuff[12]));
			frame->AddUInt(16, 4, "Number of records", ReadUInt32(&packBuff[16]));
			frame->AddUInt(20, 4, "Next Autonunber", ReadUInt32(&packBuff[20]));
			frame->AddHex32(24, "Autonunber Flags", ReadUInt32(&packBuff[24]));
			frame->AddUInt(28, 4, "CT Autonunber", ReadUInt32(&packBuff[28]));
			frame->AddUInt64(32, "Unknown", ReadUInt64(&packBuff[32]));
			frame->AddStrC(40, 1, "Table Type", &packBuff[40]);
			frame->AddUInt(41, 2, "Maximum column", ReadUInt16(&packBuff[41]));
			frame->AddUInt(43, 2, "Number of Variable Columns", ReadUInt16(&packBuff[43]));
			frame->AddUInt(45, 2, "Number of columns", ReadUInt16(&packBuff[45]));
			frame->AddUInt(47, 4, "Number of logical index", ReadUInt32(&packBuff[47]));
			frame->AddUInt(51, 4, "Number of index", ReadUInt32(&packBuff[51]));
			frame->AddHex8(55, "Usage Flags", packBuff[55]);
			frame->AddUInt(56, 3, "Usage Page", ReadUInt24(&packBuff[56]));
			frame->AddHex8(59, "Free Flags", packBuff[59]);
			frame->AddUInt(60, 3, "Free Page", ReadUInt24(&packBuff[60]));

			UOSInt ofst = 63;
			UOSInt i = 0;
			UOSInt j = ReadUInt32(&packBuff[51]);
			while (i < j)
			{
				frame->AddUInt(ofst, 4, "Index Unknown", ReadUInt32(&packBuff[ofst]));
				frame->AddUInt64(ofst + 4, "Number of index rows", ReadUInt64(&packBuff[ofst + 4]));
				ofst += 12;
				i++;
			}

			i = 0;
			j = ReadUInt16(&packBuff[45]);
			while (i < j)
			{
				frame->AddUIntName(ofst, 1, "Column Type", packBuff[ofst], ColumnTypeGetName(packBuff[ofst]));
				frame->AddUInt(ofst + 1, 4, "Unknown", ReadUInt32(&packBuff[ofst + 1]));
				frame->AddUInt(ofst + 5, 2, "Total Column Number", ReadUInt16(&packBuff[ofst + 5]));
				frame->AddUInt(ofst + 7, 2, "Offset of Variable Length Column", ReadUInt16(&packBuff[ofst + 7]));
				frame->AddUInt(ofst + 9, 2, "Column Number", ReadUInt16(&packBuff[ofst + 9]));
				frame->AddHex32(ofst + 11, "Misc", ReadUInt32(&packBuff[ofst + 11]));
				frame->AddHex8(ofst + 15, "Flags1", packBuff[ofst + 15]);
				frame->AddHex8(ofst + 16, "Flags2", packBuff[ofst + 16]);
				frame->AddUInt(ofst + 17, 4, "Unknown", ReadUInt32(&packBuff[ofst + 17]));
				frame->AddUInt(ofst + 21, 2, "Offset of Fixed Column", ReadUInt16(&packBuff[ofst + 21]));
				frame->AddUInt(ofst + 23, 2, "Column Length", ReadUInt16(&packBuff[ofst + 23]));
				ofst += 25;
				i++;
			}
			i = 0;
			while (i < j)
			{
				UOSInt colSize = ReadUInt16(&packBuff[ofst]);
				Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Column Name Size "), i);
				frame->AddUInt(ofst, 2, (const Char*)sbuff, colSize);
				Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Column Name "), i);
				Text::StrUTF16_UTF8C(sbuff2, (const UTF16Char*)&packBuff[ofst + 2], colSize >> 1)[0] = 0;
				frame->AddField(ofst + 2, colSize, sbuff, sbuff2);
				ofst += 2 + colSize;
				i++;
			}
			i = 0;
			j = ReadUInt32(&packBuff[51]);
			while (i < j)
			{
				frame->AddUInt(ofst, 4, "Unknown", ReadUInt32(&packBuff[ofst]));
				frame->AddUInt(ofst + 4, 2, "Column number0", ReadUInt16(&packBuff[ofst + 4]));
				frame->AddUInt(ofst + 6, 1, "Column order0", packBuff[ofst + 6]);
				frame->AddUInt(ofst + 7, 2, "Column number1", ReadUInt16(&packBuff[ofst + 7]));
				frame->AddUInt(ofst + 9, 1, "Column order1", packBuff[ofst + 9]);
				frame->AddUInt(ofst + 10, 2, "Column number2", ReadUInt16(&packBuff[ofst + 10]));
				frame->AddUInt(ofst + 12, 1, "Column order2", packBuff[ofst + 12]);
				frame->AddUInt(ofst + 13, 2, "Column number3", ReadUInt16(&packBuff[ofst + 13]));
				frame->AddUInt(ofst + 15, 1, "Column order3", packBuff[ofst + 15]);
				frame->AddUInt(ofst + 16, 2, "Column number4", ReadUInt16(&packBuff[ofst + 16]));
				frame->AddUInt(ofst + 18, 1, "Column order4", packBuff[ofst + 18]);
				frame->AddUInt(ofst + 19, 2, "Column number5", ReadUInt16(&packBuff[ofst + 19]));
				frame->AddUInt(ofst + 21, 1, "Column order5", packBuff[ofst + 21]);
				frame->AddUInt(ofst + 22, 2, "Column number6", ReadUInt16(&packBuff[ofst + 22]));
				frame->AddUInt(ofst + 24, 1, "Column order6", packBuff[ofst + 24]);
				frame->AddUInt(ofst + 25, 2, "Column number7", ReadUInt16(&packBuff[ofst + 25]));
				frame->AddUInt(ofst + 27, 1, "Column order7", packBuff[ofst + 27]);
				frame->AddUInt(ofst + 28, 2, "Column number8", ReadUInt16(&packBuff[ofst + 28]));
				frame->AddUInt(ofst + 30, 1, "Column order8", packBuff[ofst + 30]);
				frame->AddUInt(ofst + 31, 2, "Column number9", ReadUInt16(&packBuff[ofst + 31]));
				frame->AddUInt(ofst + 33, 1, "Column order9", packBuff[ofst + 33]);
				frame->AddHex8(ofst + 34, "Page Flag", packBuff[ofst + 34]);
				frame->AddUInt(ofst + 35, 3, "Used Page", ReadUInt24(&packBuff[ofst + 35]));
				frame->AddUInt(ofst + 38, 4, "First Index", ReadUInt32(&packBuff[ofst + 38]));
				frame->AddHex8(ofst + 42, "Index Flag", packBuff[ofst + 42]);
				frame->AddHexBuff(ofst + 43, 9, "Unknown", &packBuff[ofst + 43], false);
				ofst += 52;
				i++;
			}
			i = 0;
			j = ReadUInt32(&packBuff[47]);
			while (i < j)
			{
				frame->AddUInt(ofst, 4, "Unknown", ReadUInt32(&packBuff[ofst]));
				frame->AddUInt(ofst + 4, 4, "Index Number", ReadUInt32(&packBuff[ofst + 4]));
				frame->AddUInt(ofst + 8, 4, "Index Number2", ReadUInt32(&packBuff[ofst + 8]));
				frame->AddUInt(ofst + 12, 1, "Rel Table Type", packBuff[ofst + 12]);
				frame->AddInt(ofst + 13, 4, "Rel Index Num", ReadInt32(&packBuff[ofst + 13]));
				frame->AddInt(ofst + 17, 4, "Rel Table Page", ReadInt32(&packBuff[ofst + 17]));
				frame->AddUInt(ofst + 21, 1, "Cascade Updates", packBuff[ofst + 21]);
				frame->AddUInt(ofst + 22, 1, "Cascade Deletes", packBuff[ofst + 22]);
				frame->AddUInt(ofst + 23, 1, "Index Type", packBuff[ofst + 23]);
				ofst += 28;
				i++;
			}
			i = 0;
			while (i < j)
			{
				UOSInt colSize = ReadUInt16(&packBuff[ofst]);
				Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Index Name Size "), i);
				frame->AddUInt(ofst, 2, (const Char*)sbuff, colSize);
				Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Index Name "), i);
				Text::StrUTF16_UTF8C(sbuff2, (const UTF16Char*)&packBuff[ofst + 2], colSize >> 1)[0] = 0;
				frame->AddField(ofst + 2, colSize, sbuff, sbuff2);
				ofst += 2 + colSize;
				i++;
			}
			UOSInt freeSpace = ReadUInt16(&packBuff[2]);
			UOSInt endOfst = 4096 - freeSpace;
			while (ofst < endOfst)
			{
				Int16 colNum = ReadInt16(&packBuff[ofst]);
				UInt32 usedPages = ReadUInt32(&packBuff[ofst + 2]);
				UInt32 freePages = ReadUInt32(&packBuff[ofst + 6]);
				frame->AddInt(ofst, 2, "Column Number", colNum);
				frame->AddUInt(ofst + 2, 4, "Used Pages", usedPages);
				frame->AddUInt(ofst + 6, 4, "Free Pages", freePages);
				if (colNum == -1)
				{
					break;
				}
				ofst += 10;
			}
			frame->AddHexBuff(4096 - freeSpace, freeSpace, "Free Space", &packBuff[4096 - freeSpace], true);
		}
		break;
	}
	return frame;
}

Bool IO::FileAnalyse::MDBFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::MDBFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::MDBFileAnalyse::TrimPadding(const UTF8Char *outputFile)
{
	return false;
}

const UTF8Char *IO::FileAnalyse::MDBFileAnalyse::ColumnTypeGetName(UInt8 colType)
{
	switch (colType)
	{
	case 1:
		return (const UTF8Char*)"BOOL";
	case 2:
		return (const UTF8Char*)"BYTE";
	case 3:
		return (const UTF8Char*)"INT";
	case 4:
		return (const UTF8Char*)"LONGINT";
	case 5:
		return (const UTF8Char*)"MONEY";
	case 6:
		return (const UTF8Char*)"FLOAT";
	case 7:
		return (const UTF8Char*)"DOUBLE";
	case 8:
		return (const UTF8Char*)"DATETIME";
	case 9:
		return (const UTF8Char*)"BINARY";
	case 0xA:
		return (const UTF8Char*)"TEXT";
	case 0xB:
		return (const UTF8Char*)"OLE"; //Long Binary
	case 0xC:
		return (const UTF8Char*)"MEMO"; //Long Text
	case 0xF:
		return (const UTF8Char*)"REPID";
	case 0x10:
		return (const UTF8Char*)"NUMERIC";
	default:
		return (const UTF8Char*)"Unknown";
	}
}