#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/JMVL01FileAnalyse.h"
#include "Text/Encoding.h"
#include "Text/StringBuilderUTF8.h"

Text::CString IO::FileAnalyse::JMVL01FileAnalyse::GetTagName(UInt8 tagType)
{
	switch (tagType)
	{
	case 0x01:
		return CSTR("Login information");
	case 0x12:
		return CSTR("Positioning Data");
	case 0x13:
		return CSTR("Status information");
	case 0x15:
		return CSTR("Online command response");
	case 0x16:
		return CSTR("Alarm Data");
	case 0x17:
		return CSTR("LBS Address Query Packet");
	case 0x18:
		return CSTR("LBS Multiple Base Station Extended Information Packet");
	case 0x1A:
		return CSTR("GPS Address Query Packet");
	case 0x80:
		return CSTR("Command sent by server to the terminal");
	case 0x8A:
		return CSTR("Time Calibration Packet");
	case 0x94:
		return CSTR("Information transmission packet");
	case 0x95:
		return CSTR("Alarm packet");
	case 0xA0:
		return CSTR("Location packet");
	case 0xA4:
		return CSTR("Multi-fence alarm packet");
	case 0XC3:
		return CSTR("WiFi information collection package");
	}
	return CSTR_NULL;
}

void __stdcall IO::FileAnalyse::JMVL01FileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::JMVL01FileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::JMVL01FileAnalyse>();
	UInt64 dataSize;
	UInt64 ofst;
	UInt8 tagHdr[5];
	NN<IO::FileAnalyse::JMVL01FileAnalyse::JMVL01Tag> tag;
	ofst = 0;
	dataSize = me->fd->GetDataSize();
	
	while (ofst < dataSize - 10 && !thread->IsStopping())
	{
		if (me->fd->GetRealData(ofst, 5, BYTEARR(tagHdr)) != 5)
			break;
		
		if (tagHdr[0] == 0x78 && tagHdr[1] == 0x78)
		{
			tag = MemAllocNN(IO::FileAnalyse::JMVL01FileAnalyse::JMVL01Tag);
			tag->ofst = ofst;
			tag->size = (UOSInt)tagHdr[2] + 5;
			tag->tagType = tagHdr[3];
			me->tags.Add(tag);
			ofst += tag->size;
		}
		else if (tagHdr[0] == 0x79 && tagHdr[1] == 0x79)
		{
			tag = MemAllocNN(IO::FileAnalyse::JMVL01FileAnalyse::JMVL01Tag);
			tag->ofst = ofst;
			tag->size = (UOSInt)ReadMUInt16(&tagHdr[2]) + 6;
			tag->tagType = tagHdr[4];
			me->tags.Add(tag);
			ofst += tag->size;
		}
		else
		{
			break;
		}
	}
}

IO::FileAnalyse::JMVL01FileAnalyse::JMVL01FileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("JMVL01FileAnaly"))
{
	UInt8 buff[256];
	this->fd = 0;
	this->pauseParsing = false;
	fd->GetRealData(0, 256, BYTEARR(buff));
	if (buff[0] == 0x78 && buff[1] == 0x78 && buff[2] == 0x11 && buff[3] == 0x01 && buff[20] == 13 && buff[21] == 10)
	{
	}
	else if (buff[0] == 0x78 && buff[1] == 0x78 && buff[2] == 0xd && buff[3] == 0x01 && buff[16] == 13 && buff[17] == 10)
	{
	}
	else
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::JMVL01FileAnalyse::~JMVL01FileAnalyse()
{
	this->thread.Stop();
	SDEL_CLASS(this->fd);
	this->tags.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::JMVL01FileAnalyse::GetFormatName()
{
	return CSTR("JM-VL01");
}

UOSInt IO::FileAnalyse::JMVL01FileAnalyse::GetFrameCount()
{
	return this->tags.GetCount();
}

Bool IO::FileAnalyse::JMVL01FileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::JMVL01FileAnalyse::JMVL01Tag> tag;
	Text::CString name;
	if (!this->tags.GetItem(index).SetTo(tag))
		return false;
	sb->AppendU64(tag->ofst);
	sb->AppendC(UTF8STRC(": Type=0x"));
	sb->AppendHex8(tag->tagType);
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


UOSInt IO::FileAnalyse::JMVL01FileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->tags.GetCount() - 1;
	OSInt k;
	NN<JMVL01Tag> pack;
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::JMVL01FileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	NN<IO::FileAnalyse::JMVL01FileAnalyse::JMVL01Tag> tag;
	if (!this->tags.GetItem(index).SetTo(tag))
		return 0;
	
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(tag->ofst, tag->size));
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Packet ")), index);
	frame->AddHeader(CSTRP(sbuff, sptr));

	Data::ByteBuffer tagData(tag->size);
	this->fd->GetRealData(tag->ofst, tag->size, tagData);
	frame->AddHexBuff(0, CSTR("Start of Packet"), tagData.WithSize(2), false);
	UOSInt frameSize;
	UOSInt currOfst;
	Bool parsed = false;
	if (tagData[0] == 0x79 && tagData[1] == 0x79)
	{
		frameSize = ReadMUInt16(&tagData[2]);
		frame->AddUInt(2, 2, CSTR("Packet Length"), frameSize);
		currOfst = 4;
	}
	else
	{
		frameSize = tagData[2];
		frame->AddUInt(2, 1, CSTR("Packet Length"), frameSize);
		currOfst = 3;
	}
	frame->AddHex8Name(currOfst, CSTR("Protocol Number"), tagData[currOfst], GetTagName(tagData[currOfst]));
	switch (tagData[currOfst])
	{
	case 0x01: //Login information
		if (frameSize == 13 || frameSize == 17)
		{
			frame->AddHex64(currOfst + 1, CSTR("IMEI"), ReadMUInt64(&tagData[currOfst + 1]));
			if (frameSize == 17)
			{
				frame->AddHex16(currOfst + 9, CSTR("Type identification code"), ReadMUInt16(&tagData[currOfst + 9]));
				UOSInt tz = (UOSInt)ReadMUInt16(&tagData[currOfst + 11]) >> 4;
				if (tagData[currOfst + 12] & 8)
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("GMT-"));
				}
				else
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("GMT+"));
				}
				sptr = Text::StrUOSInt(sptr, tz / 100);
				*sptr++ = ':';
				tz = (tz % 100) * 3 / 5;
				if (tz < 10)
				{
					*sptr++ = '0';
				}
				sptr = Text::StrUOSInt(sptr, (tz % 100) * 3 / 5);
				frame->AddField(currOfst + 11, 2, CSTR("Time Zone"), CSTRP(sbuff, sptr));
			}
			parsed = true;
		}
		break;
	case 0x12: //GPS Position
		if (frameSize == 31)
		{
			Data::DateTime dt;
			dt.SetValue((UInt16)(2000 + tagData[currOfst + 1]), tagData[currOfst + 2], tagData[currOfst + 3], tagData[currOfst + 4], tagData[currOfst + 5], tagData[currOfst + 6], 0, 0);
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			frame->AddField(currOfst + 1, 6, CSTR("Date Time"), CSTRP(sbuff, sptr));
			frame->AddUInt(currOfst + 7, 1, CSTR("Length of GPS information"), (UOSInt)tagData[currOfst + 7] >> 4);
			frame->AddUInt(currOfst + 7, 1, CSTR("Quantity of positioning satellites"), tagData[currOfst + 7] & 15);
			Double lat = ReadMUInt32(&tagData[currOfst + 8]) / 1800000.0;
			Double lon = ReadMUInt32(&tagData[currOfst + 12]) / 1800000.0;
			if ((tagData[currOfst + 17] & 4) == 0)
			{
				lat = -lat;
			}
			if (tagData[currOfst + 17] & 8)
			{
				lon = -lon;
			}
			sptr = Text::StrDouble(sbuff, lat);
			frame->AddField(currOfst + 8, 4, CSTR("Latitude"), CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, lon);
			frame->AddField(currOfst + 12, 4, CSTR("Longitude"), CSTRP(sbuff, sptr));
			frame->AddUInt(currOfst + 16, 1, CSTR("Speed (km/h)"), tagData[currOfst + 16]);
			frame->AddUInt(currOfst + 17, 2, CSTR("Course"), ReadMUInt16(&tagData[currOfst + 17]) & 0x3FF);
			frame->AddUInt(currOfst + 17, 1, CSTR("GPS differential positioning"), (tagData[currOfst + 17] & 0x20) >> 5);
			frame->AddUInt(currOfst + 17, 1, CSTR("GPS having been positioning"), (tagData[currOfst + 17] & 0x10) >> 4);
			frame->AddUInt(currOfst + 17, 1, CSTR("West Longitude"), (tagData[currOfst + 17] & 0x8) >> 3);
			frame->AddUInt(currOfst + 17, 1, CSTR("North Latitude"), (tagData[currOfst + 17] & 0x4) >> 2);
			frame->AddUInt(currOfst + 19, 2, CSTR("MCC"), ReadMUInt16(&tagData[currOfst + 19]) & 0x7fff);

			UOSInt i;
			if (tagData[currOfst + 19] & 0x80)
			{
				frame->AddUInt(currOfst + 21, 2, CSTR("MNC"), ReadMUInt16(&tagData[currOfst + 21]));
				i = currOfst + 23;
			}
			else
			{
				frame->AddUInt(currOfst + 21, 1, CSTR("MNC"), tagData[currOfst + 21]);
				i = currOfst + 22;
			}
			frame->AddUInt(i, 2, CSTR("LAC"), ReadMUInt16(&tagData[i]));
			frame->AddUInt(i + 2, 3, CSTR("Cell ID"), ReadMUInt24(&tagData[i + 2]));
			parsed = true;
		}
		break;
	case 0x13: //Status information
		if (frameSize == 10)
		{
			frame->AddHex8(currOfst + 1, CSTR("Terminal Information"), tagData[currOfst + 1]);
			frame->AddUInt(currOfst + 1, 1, CSTR("Oil and Electricity disconnected"), (tagData[currOfst + 1] & 0x80) >> 7);
			frame->AddUInt(currOfst + 1, 1, CSTR("GPS tracking is on"), (tagData[currOfst + 1] & 0x40) >> 6);
			frame->AddUInt(currOfst + 1, 1, CSTR("Charge On"), (tagData[currOfst + 1] & 0x4) >> 2);
			frame->AddUInt(currOfst + 1, 1, CSTR("ACC"), (tagData[currOfst + 1] & 0x2) >> 1);
			frame->AddUInt(currOfst + 1, 1, CSTR("Defense Activated"), (tagData[currOfst + 1] & 1));
			frame->AddUInt(currOfst + 2, 1, CSTR("Voltage Level"), tagData[currOfst + 2]);
			frame->AddUInt(currOfst + 3, 1, CSTR("GSM Signal Strength"), tagData[currOfst + 3]);
			frame->AddUInt(currOfst + 4, 2, CSTR("Language/Extended Port Status"), ReadMUInt16(&tagData[currOfst + 4]));
			parsed = true;
		}
		break;
	case 0x80: //Command sent by server to the terminal
		break;
	case 0x94: //Information transmission packet
		{
			UInt8 infoType = tagData[currOfst + 1];
			frame->AddHex8(currOfst + 1, CSTR("Information Type"), infoType);
			parsed = true;
		}
		break;
	case 0x95: //Alarm packet
		break;
	case 0xA0: //Location packet
		if (frameSize >= 41)
		{
			Data::DateTime dt;
			dt.SetValue((UInt16)(2000 + tagData[currOfst + 1]), tagData[currOfst + 2], tagData[currOfst + 3], tagData[currOfst + 4], tagData[currOfst + 5], tagData[currOfst + 6], 0, 0);
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			frame->AddField(currOfst + 1, 6, CSTR("Date Time"), CSTRP(sbuff, sptr));
			frame->AddUInt(currOfst + 7, 1, CSTR("Length of GPS information"), (UOSInt)tagData[currOfst + 7] >> 4);
			frame->AddUInt(currOfst + 7, 1, CSTR("Quantity of positioning satellites"), tagData[currOfst + 7] & 15);
			Double lat = ReadMUInt32(&tagData[currOfst + 8]) / 1800000.0;
			Double lon = ReadMUInt32(&tagData[currOfst + 12]) / 1800000.0;
			if ((tagData[currOfst + 17] & 4) == 0)
			{
				lat = -lat;
			}
			if (tagData[currOfst + 17] & 8)
			{
				lon = -lon;
			}
			sptr = Text::StrDouble(sbuff, lat);
			frame->AddField(currOfst + 8, 4, CSTR("Latitude"), CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, lon);
			frame->AddField(currOfst + 12, 4, CSTR("Longitude"), CSTRP(sbuff, sptr));
			frame->AddUInt(currOfst + 16, 1, CSTR("Speed (km/h)"), tagData[currOfst + 16]);
			frame->AddUInt(currOfst + 17, 2, CSTR("Course"), ReadMUInt16(&tagData[currOfst + 17]) & 0x3FF);
			frame->AddUInt(currOfst + 17, 1, CSTR("GPS differential positioning"), (tagData[currOfst + 17] & 0x20) >> 5);
			frame->AddUInt(currOfst + 17, 1, CSTR("GPS having been positioning"), (tagData[currOfst + 17] & 0x10) >> 4);
			frame->AddUInt(currOfst + 17, 1, CSTR("West Longitude"), (tagData[currOfst + 17] & 0x8) >> 3);
			frame->AddUInt(currOfst + 17, 1, CSTR("North Latitude"), (tagData[currOfst + 17] & 0x4) >> 2);
			frame->AddUInt(currOfst + 19, 2, CSTR("MCC"), ReadMUInt16(&tagData[currOfst + 19]) & 0x7fff);

			UOSInt i;
			if (tagData[currOfst + 19] & 0x80)
			{
				frame->AddUInt(currOfst + 21, 2, CSTR("MNC"), ReadMUInt16(&tagData[currOfst + 21]));
				i = currOfst + 23;
			}
			else
			{
				frame->AddUInt(currOfst + 21, 1, CSTR("MNC"), tagData[currOfst + 21]);
				i = currOfst + 22;
			}
			frame->AddUInt(i, 4, CSTR("LAC"), ReadMUInt32(&tagData[i]));
			frame->AddUInt64(i + 4, CSTR("Cell ID"), ReadMUInt64(&tagData[i + 4]));
			frame->AddUInt(i + 12, 1, CSTR("ACC"), tagData[i + 12]);
			frame->AddUInt(i + 13, 1, CSTR("Data upload mode"), tagData[i + 13]);
			frame->AddUInt(i + 14, 1, CSTR("GPS real-time reupload"), tagData[i + 14]);
			if (frameSize >= 45)
			{
				frame->AddUInt(i + 15, 4, CSTR("Mileage"), ReadMUInt32(&tagData[i + 15]));
			}
			parsed = false;
		}
		break;
	case 0xA4: //Multi-fence alarm packet
		break;
	case 0XC3: //WiFi information collection package
		break;
	}
	///////////////////////////////////
	if (!parsed)
	{
//		frame->AddHexBuff(currOfst + 1, frameSize - 5, CSTR("Packet Data"), &tagData[currOfst + 1], true);
	}
	frame->AddUInt(currOfst + frameSize - 4, 2, CSTR("Information Serial Number"), ReadMUInt16(&tagData[currOfst + frameSize - 4]));
	frame->AddHex16(currOfst + frameSize - 2, CSTR("Error Check"), ReadMUInt16(&tagData[currOfst + frameSize - 2]));
	frame->AddHexBuff(currOfst + frameSize, 2, CSTR("Stop Bit"), &tagData[currOfst + frameSize], false);
	return frame;
}

Bool IO::FileAnalyse::JMVL01FileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::JMVL01FileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::JMVL01FileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
