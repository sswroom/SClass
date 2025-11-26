#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "Data/DWGUtil.h"
#include "Data/UUID.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/FileAnalyse/DWGFileAnalyse.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

class DWGFileAnalyseComparator : public Data::Comparator<NN<IO::FileAnalyse::DWGFileAnalyse::PackInfo>>
{
public:
	virtual ~DWGFileAnalyseComparator(){};

	virtual OSInt Compare(NN<IO::FileAnalyse::DWGFileAnalyse::PackInfo> a, NN<IO::FileAnalyse::DWGFileAnalyse::PackInfo> b) const
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

void __stdcall IO::FileAnalyse::DWGFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::DWGFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::DWGFileAnalyse>();
	UInt8 buff[256];
	NN<IO::StreamData> fd;
	NN<IO::FileAnalyse::DWGFileAnalyse::PackInfo> pack;
	if (!me->fd.SetTo(fd))
	{
		return;
	}
	if (me->fileVer == 12 || me->fileVer == 14 || me->fileVer == 15)
	{
		fd->GetRealData(0, 256, BYTEARR(buff));
		UOSInt sectionCnt = ReadUInt32(&buff[21]);
		UOSInt i;
		UOSInt ofst;
		pack = MemAllocNN(IO::FileAnalyse::DWGFileAnalyse::PackInfo);
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
					pack = MemAllocNN(IO::FileAnalyse::DWGFileAnalyse::PackInfo);
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
		fd->GetRealData(imgAddr, 256, BYTEARR(buff));
		UInt32 imgSize = ReadUInt32(&buff[16]);
		pack = MemAllocNN(IO::FileAnalyse::DWGFileAnalyse::PackInfo);
		pack->fileOfst = imgAddr;
		pack->packSize = imgSize + 20 + 16;
		pack->packType = PackType::PreviewImage;
		me->packs.Add(pack);

		DWGFileAnalyseComparator comparator;
		Sync::MutexUsage mutUsage;
		Data::Sort::ArtificialQuickSort::Sort<NN<PackInfo>>(me->packs.GetArrayList(mutUsage), comparator);
		mutUsage.EndUse();
	}
	else if (me->fileVer == 27)
	{
		fd->GetRealData(0, 256, BYTEARR(buff));
		pack = MemAllocNN(IO::FileAnalyse::DWGFileAnalyse::PackInfo);
		pack->fileOfst = 0;
		pack->packSize = 256;
		pack->packType = PackType::R2004FileHeader;
		me->packs.Add(pack);
	}
}

IO::FileAnalyse::DWGFileAnalyse::DWGFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("DWGFileAnalyse"))
{
	UInt8 buff[8];
	this->fd = 0;
	this->pauseParsing = false;
	this->fileVer = 0;
	fd->GetRealData(0, 6, BYTEARR(buff));
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
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::DWGFileAnalyse::~DWGFileAnalyse()
{
	this->thread.Stop();
	this->fd.Delete();
	this->packs.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::DWGFileAnalyse::GetFormatName()
{
	return CSTR("DWG");
}

UOSInt IO::FileAnalyse::DWGFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::DWGFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::DWGFileAnalyse::PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
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
	NN<PackInfo> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItemNoCheck((UOSInt)k);
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::DWGFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::DWGFileAnalyse::PackInfo> pack;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::StreamData> fd;
	if (!this->packs.GetItem(index).SetTo(pack))
		return 0;
	if (!this->fd.SetTo(fd))
		return 0;

	UInt8 buff[128];
	Data::UUID uuid;
	NN<IO::FileAnalyse::FrameDetail> frame;
	UOSInt nSection;
	UOSInt ofst;
	UOSInt i;
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSize));
	sptr = PackTypeGetName(pack->packType).ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Type=")));
	frame->AddText(0, CSTRP(sbuff, sptr));
	sptr = Text::StrUInt64(Text::StrConcatC(sbuff, UTF8STRC("Size=")), pack->packSize);
	frame->AddText(0, CSTRP(sbuff, sptr));

	switch (pack->packType)
	{
	case PackType::FileHeaderV1:
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		frame->AddStrC(0, 4, CSTR("Magic number"), &packBuff[0]);
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
		break;
	}
	case PackType::R2004FileHeader:
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);
		frame->AddStrC(0, 4, CSTR("Magic number"), &packBuff[0]);
		frame->AddUInt(4, 2, CSTR("File Version"), this->fileVer);
		frame->AddHexBuff(6, 5, CSTR("All Zero"), &packBuff[6], false);
		frame->AddUInt(11, 1, CSTR("Maintenance release version"), packBuff[11]);
		frame->AddUInt(12, 1, CSTR("Unknown"), packBuff[12]);
		frame->AddHex32(13, CSTR("Preview Image Address"), ReadUInt32(&packBuff[13]));
		frame->AddUInt(0x11, 1, CSTR("Application version"), packBuff[17]);
		frame->AddUInt(0x12, 1, CSTR("Application maintenance release version"), packBuff[18]);
		frame->AddUInt(0x13, 2, CSTR("Codepage"), ReadUInt16(&packBuff[19]));
		frame->AddHexBuff(0x15, 3, CSTR("All Zero"), &packBuff[21], false);
		frame->AddHex32(0x18, CSTR("Security flags"), ReadUInt32(&packBuff[0x18]));
		frame->AddBit(0x18, CSTR("Encrypt data"), packBuff[0x18] & 1, 0);
		frame->AddBit(0x18, CSTR("Encrypt properties"), packBuff[0x18] & 2, 1);
		frame->AddBit(0x18, CSTR("Sign data"), packBuff[0x18] & 16, 4);
		frame->AddBit(0x18, CSTR("Add timestamp"), packBuff[0x18] & 32, 5);
		frame->AddUInt(0x1C, 4, CSTR("Unknown long"), ReadUInt32(&packBuff[0x1C]));
		frame->AddHex32(0x20, CSTR("Summary info Address"), ReadUInt32(&packBuff[0x20]));
		frame->AddHex32(0x24, CSTR("VBA Project Address"), ReadUInt32(&packBuff[0x24]));
		frame->AddHex32(0x28, CSTR("Unknown"), ReadUInt32(&packBuff[0x28]));
		frame->AddHexBuff(0x2C, 0x54, CSTR("All Zero"), &packBuff[0x2C], true);
		Data::DWGUtil::HeaderDecrypt(&packBuff[0x80], buff, 0x6c);
		frame->AddHexBuff(0x80, CSTR("Decrypted block"), Data::ByteArrayR(buff, 0x6c), true);
		break;
	}
	case PackType::PreviewImage:
	{
		Data::ByteBuffer packBuff((UOSInt)pack->packSize);
		fd->GetRealData(pack->fileOfst, (UOSInt)pack->packSize, packBuff);

		uuid.SetValue(packBuff.Arr());
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

		uuid.SetValue(&packBuff[(UOSInt)pack->packSize - 16]);
		sptr = uuid.ToString(sbuff);
		frame->AddField((UOSInt)pack->packSize - 16, 16, CSTR("Section Type"), CSTRP(sbuff, sptr));
		break;
	}
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
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::DWGFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::DWGFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}

Text::CStringNN IO::FileAnalyse::DWGFileAnalyse::PackTypeGetName(PackType packType)
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
	case PackType::R2004FileHeader:
		return CSTR("R2004 File Header");
	case PackType::Unknown:
	default:
		return CSTR("Unknown");
	}
}
