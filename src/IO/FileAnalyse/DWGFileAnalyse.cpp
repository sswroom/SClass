#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/UUID.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/FileAnalyse/DWGFileAnalyse.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

class DWGFileAnalyseComparator : public Data::Comparator<IO::FileAnalyse::DWGFileAnalyse::PackInfo*>
{
public:
	virtual ~DWGFileAnalyseComparator(){};

	virtual OSInt Compare(IO::FileAnalyse::DWGFileAnalyse::PackInfo *a, IO::FileAnalyse::DWGFileAnalyse::PackInfo *b) const
	{
		if (a->fileOfst > b->fileOfst)
		{
			return 1;
		}
		else if (a->fileOfst < b->fileOfst)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
};

UInt32 __stdcall IO::FileAnalyse::DWGFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::DWGFileAnalyse *me = (IO::FileAnalyse::DWGFileAnalyse*)userObj;
	UInt8 buff[256];
	IO::FileAnalyse::DWGFileAnalyse::PackInfo *pack;
	me->threadRunning = true;
	me->threadStarted = true;
	if (me->fileVer == 12 || me->fileVer == 14 || me->fileVer == 15)
	{
		me->fd->GetRealData(0, 256, buff);
		UOSInt sectionCnt = ReadUInt32(&buff[21]);
		UOSInt i;
		UOSInt ofst;
		pack = MemAlloc(IO::FileAnalyse::DWGFileAnalyse::PackInfo, 1);
		pack->fileOfst = 0;
		pack->packSize = 25 + 9 * sectionCnt + 2 + 16;
		pack->packType = PackType::FileHeaderV1;
		me->packs.Add(pack);
		if (sectionCnt <= 6)
		{
			UInt32 secOfst;
			UInt32 secSize;
			ofst = 25;
			i = 0;
			while (i < sectionCnt)
			{
				secOfst = ReadUInt32(&buff[ofst + 1]);
				secSize = ReadUInt32(&buff[ofst + 5]);
				if (secOfst != 0 && secSize != 0)
				{
					pack = MemAlloc(IO::FileAnalyse::DWGFileAnalyse::PackInfo, 1);
					pack->fileOfst = secOfst;
					pack->packSize = secSize;
					switch (buff[ofst])
					{
					case 0:
						pack->packType = PackType::HeaderVariables;
						break;
					case 1:
						pack->packType = PackType::ClassSection;
						break;
					case 2:
						pack->packType = PackType::ObjectMap;
						break;
					case 3:
						pack->packType = PackType::UnknownTable;
						break;
					case 4:
						pack->packType = PackType::Measurement;
						break;
					default:
						pack->packType = PackType::Unknown;
						break;
					}
					me->packs.Add(pack);
				}
				i++;
				ofst += 9;
			}
		}

		UInt32 imgAddr = ReadUInt32(&buff[13]);
		me->fd->GetRealData(imgAddr, 256, buff);
		UInt32 imgSize = ReadUInt32(&buff[16]);
		pack = MemAlloc(IO::FileAnalyse::DWGFileAnalyse::PackInfo, 1);
		pack->fileOfst = imgAddr;
		pack->packSize = imgSize + 20 + 16;
		pack->packType = PackType::PreviewImage;
		me->packs.Add(pack);

		DWGFileAnalyseComparator comparator;
		Sync::MutexUsage mutUsage;
		Data::Sort::ArtificialQuickSort::Sort(me->packs.GetArrayList(&mutUsage), &comparator);
		mutUsage.EndUse();
	}
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::DWGFileAnalyse::DWGFileAnalyse(IO::StreamData *fd)
{
	UInt8 buff[8];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	this->fileVer = 0;
	fd->GetRealData(0, 6, buff);
	if (ReadNInt32(buff) != *(Int32*)"AC10")
	{
		return;
	}
	if (buff[4] < '0' || buff[4] > '9' || buff[5] < '0' || buff[5] > '9')
	{
		return;
	}
	this->fileVer = (UInt32)(buff[4] - '0') * 10 + (UInt32)buff[5] - '0';
	switch (fileVer)
	{
	case 12: //R13
	case 14: //R14
	case 15: //R2000
	case 18: //R2004
	case 21: //R2007
	case 24: //R2010
	case 27: //R2013
	case 32: //R2018
		break;
	default:
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	Sync::Thread::Create(ParseThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
}

IO::FileAnalyse::DWGFileAnalyse::~DWGFileAnalyse()
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
	LIST_FREE_FUNC(&this->packs, MemFree);
}

Text::CString IO::FileAnalyse::DWGFileAnalyse::GetFormatName()
{
	return CSTR("DWG");
}

UOSInt IO::FileAnalyse::DWGFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::DWGFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::DWGFileAnalyse::PackInfo *pack;
	pack = this->packs.GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->Append(PackTypeGetName(pack->packType));
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendU64(pack->packSize);
	return true;
}

UOSInt IO::FileAnalyse::DWGFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs.GetCount() - 1;
	OSInt k;
	PackInfo *pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItem((UOSInt)k);
		if (ofst < pack->fileOfst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->fileOfst + pack->packSize)
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

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::DWGFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::DWGFileAnalyse::PackInfo *pack;
	Text::CString vName;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UInt8 *packBuff;
	pack = this->packs.GetItem(index);
	if (pack == 0)
		return 0;

	Data::UUID uuid;
	IO::FileAnalyse::FrameDetail *frame;
	UOSInt nSection;
	UOSInt ofst;
	UOSInt i;
	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, (UInt32)pack->packSize));
	sptr = PackTypeGetName(pack->packType).ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Type=")));
	frame->AddText(0, CSTRP(sbuff, sptr));
	sptr = Text::StrUInt64(Text::StrConcatC(sbuff, UTF8STRC("Size=")), pack->packSize);
	frame->AddText(0, CSTRP(sbuff, sptr));

	switch (pack->packType)
	{
	case PackType::FileHeaderV1:
		packBuff = MemAlloc(UInt8, (UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		frame->AddStrC(0, 4, CSTR("Magic number"), packBuff);
		frame->AddUInt(4, 2, CSTR("File Version"), this->fileVer);
		frame->AddHexBuff(6, 5, CSTR("All Zero"), &packBuff[6], false);
		frame->AddUInt(11, 1, CSTR("Maintenance release version"), packBuff[11]);
		frame->AddUInt(12, 1, CSTR("Unknown"), packBuff[12]);
		frame->AddHex32(13, CSTR("Preview Image Address"), ReadUInt32(&packBuff[13]));
		frame->AddUInt(0x11, 1, CSTR("Application version"), packBuff[17]);
		frame->AddUInt(0x12, 1, CSTR("Application maintenance release version"), packBuff[18]);
		frame->AddUInt(0x13, 2, CSTR("Codepage"), ReadUInt16(&packBuff[19]));
		frame->AddUInt(0x15, 4, CSTR("Section Count"), nSection = ReadUInt32(&packBuff[21]));
		ofst = 0x19;
		i = 0;
		while (i < nSection)
		{
			sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Section ")), i), UTF8STRC(" type"));
			frame->AddUInt(ofst, 1, CSTRP(sbuff, sptr), packBuff[ofst]);
			sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Section ")), i), UTF8STRC(" offset"));
			frame->AddUInt(ofst + 1, 4, CSTRP(sbuff, sptr), ReadUInt32(&packBuff[ofst + 1]));
			sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Section ")), i), UTF8STRC(" size"));
			frame->AddUInt(ofst + 5, 4, CSTRP(sbuff, sptr), ReadUInt32(&packBuff[ofst + 5]));
			i++;
			ofst += 9;
		}
		frame->AddHex16(ofst, CSTR("CRC"), ReadUInt16(&packBuff[ofst]));
		uuid.SetValue(&packBuff[ofst + 2]);
		sptr = uuid.ToString(sbuff);
		frame->AddField(ofst + 2, 16, CSTR("Section Type"), CSTRP(sbuff, sptr));

		MemFree(packBuff);
		break;
	case PackType::PreviewImage:
		packBuff = MemAlloc(UInt8, (UOSInt)pack->packSize);
		this->fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);

		uuid.SetValue(packBuff);
		sptr = uuid.ToString(sbuff);
		frame->AddField(0, 16, CSTR("Image Type"), CSTRP(sbuff, sptr));
		frame->AddUInt(16, 4, CSTR("Overall Size"), ReadUInt32(&packBuff[16]));
		frame->AddUInt(20, 1, CSTR("Image Present"), packBuff[20]);
		frame->AddUInt(21, 1, CSTR("Code"), packBuff[21]);
		switch (packBuff[21])
		{
		case 1:
			frame->AddUInt(22, 4, CSTR("Header data start"), ReadUInt32(&packBuff[22]));
			frame->AddUInt(26, 4, CSTR("Header data size"), ReadUInt32(&packBuff[26]));
			frame->AddHexBuff(30, (UOSInt)pack->packSize - 46, CSTR("Header Data"), &packBuff[30], true);
			break;
		case 2:
			frame->AddUInt(22, 4, CSTR("Bmp start"), ReadUInt32(&packBuff[22]));
			frame->AddUInt(26, 4, CSTR("Bmp size"), ReadUInt32(&packBuff[26]));
			frame->AddHexBuff(30, (UOSInt)pack->packSize - 46, CSTR("Bmp Data"), &packBuff[30], true);
			break;
		case 3:
			frame->AddUInt(22, 4, CSTR("Wmf start"), ReadUInt32(&packBuff[22]));
			frame->AddUInt(26, 4, CSTR("Wmf size"), ReadUInt32(&packBuff[26]));
			frame->AddHexBuff(30, (UOSInt)pack->packSize - 46, CSTR("Wmf Data"), &packBuff[30], true);
			break;
		}

		uuid.SetValue(&packBuff[pack->packSize - 16]);
		sptr = uuid.ToString(sbuff);
		frame->AddField((UOSInt)pack->packSize - 16, 16, CSTR("Section Type"), CSTRP(sbuff, sptr));

		MemFree(packBuff);
		break;
	case PackType::HeaderVariables:
	case PackType::ClassSection:
	case PackType::ObjectMap:
	case PackType::UnknownTable:
	case PackType::Measurement:
	case PackType::Unknown:
		break;
	}
	return frame;
}

Bool IO::FileAnalyse::DWGFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::DWGFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::DWGFileAnalyse::TrimPadding(Text::CString outputFile)
{
	return false;
}

Text::CString IO::FileAnalyse::DWGFileAnalyse::PackTypeGetName(PackType packType)
{
	switch (packType)
	{
	case PackType::FileHeaderV1:
		return CSTR("File Header V1");
	case PackType::PreviewImage:
		return CSTR("Preview Image");
	case PackType::HeaderVariables:
		return CSTR("Header Variables");
	case PackType::ClassSection:
		return CSTR("Class Section");
	case PackType::ObjectMap:
		return CSTR("Object Map");
	case PackType::UnknownTable:
		return CSTR("Unknown Table");
	case PackType::Measurement:
		return CSTR("Measurement");
	case PackType::Unknown:
	default:
		return CSTR("Unknown");
	}
}
