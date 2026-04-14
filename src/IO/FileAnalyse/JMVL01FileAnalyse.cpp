#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
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
	case 0x21:
		return CSTR("Response to Online Command by Terminal");
	case 0x22:
		return CSTR("GPS location packet (UTC)");
	case 0x26:
		return CSTR("Alarm Data (UTC)");
	case 0x28:
		return CSTR("LBS Multi-base Extended Information Packet");
	case 0x2A:
		return CSTR("GPS Address Request Packet (UTC)");
	case 0x80:
		return CSTR("Command sent by server to the terminal");
	case 0x8A:
		return CSTR("Time Calibration Packet");
	case 0x94:
		return CSTR("Information transmission packet");
	case 0x95:
		return CSTR("Alarm packet");
	case 0x97:
		return CSTR("English Address Packet");
	case 0xA0:
		return CSTR("GPS Location Packet (UTC, 4G Base Station Data)");
	case 0xA1:
		return CSTR("LBS Multi-base Extended Information Packet (4G)");
	case 0xA4:
		return CSTR("Multi-fence alarm packet");
	case 0XC3:
		return CSTR("WiFi information collection package");
	}
	return nullptr;
}

void __stdcall IO::FileAnalyse::JMVL01FileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::JMVL01FileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::JMVL01FileAnalyse>();
	UInt64 dataSize;
	UInt64 ofst;
	UInt8 tagHdr[5];
	NN<IO::FileAnalyse::JMVL01FileAnalyse::JMVL01Tag> tag;
	NN<IO::StreamData> fd;
	if (!me->fd.SetTo(fd))
		return;
	ofst = 0;
	dataSize = fd->GetDataSize();
	
	while (ofst < dataSize - 10 && !thread->IsStopping())
	{
		if (fd->GetRealData(ofst, 5, BYTEARR(tagHdr)) != 5)
			break;
		
		if (tagHdr[0] == 0x78 && tagHdr[1] == 0x78)
		{
			tag = MemAllocNN(IO::FileAnalyse::JMVL01FileAnalyse::JMVL01Tag);
			tag->ofst = ofst;
			tag->size = (UIntOS)tagHdr[2] + 5;
			tag->tagType = tagHdr[3];
			me->tags.Add(tag);
			ofst += tag->size;
		}
		else if (tagHdr[0] == 0x79 && tagHdr[1] == 0x79)
		{
			tag = MemAllocNN(IO::FileAnalyse::JMVL01FileAnalyse::JMVL01Tag);
			tag->ofst = ofst;
			tag->size = (UIntOS)ReadMUInt16(&tagHdr[2]) + 6;
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
	this->fd = nullptr;
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
	this->fd.Delete();
	this->tags.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::JMVL01FileAnalyse::GetFormatName()
{
	return CSTR("JM-VL01");
}

UIntOS IO::FileAnalyse::JMVL01FileAnalyse::GetFrameCount()
{
	return this->tags.GetCount();
}

Bool IO::FileAnalyse::JMVL01FileAnalyse::GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::JMVL01FileAnalyse::JMVL01Tag> tag;
	Text::CStringNN name;
	if (!this->tags.GetItem(index).SetTo(tag))
		return false;
	sb->AppendU64(tag->ofst);
	sb->AppendC(UTF8STRC(": Type=0x"));
	sb->AppendHex8(tag->tagType);
	if (GetTagName(tag->tagType).SetTo(name))
	{
		sb->AppendC(UTF8STRC(" ("));
		sb->Append(name);
		sb->AppendC(UTF8STRC(")"));
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUIntOS(tag->size);
	return true;
}


UIntOS IO::FileAnalyse::JMVL01FileAnalyse::GetFrameIndex(UInt64 ofst)
{
	IntOS i = 0;
	IntOS j = (IntOS)this->tags.GetCount() - 1;
	IntOS k;
	NN<JMVL01Tag> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->tags.GetItemNoCheck((UIntOS)k);
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
			return (UIntOS)k;
		}
	}
	return INVALID_INDEX;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::JMVL01FileAnalyse::GetFrameDetail(UIntOS index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::FileAnalyse::JMVL01FileAnalyse::JMVL01Tag> tag;
	NN<IO::StreamData> fd;
	if (!this->tags.GetItem(index).SetTo(tag))
		return nullptr;
	if (!this->fd.SetTo(fd))
		return nullptr;
	
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(tag->ofst, tag->size));
	sptr = Text::StrUIntOS(Text::StrConcatC(sbuff, UTF8STRC("Packet ")), index);
	frame->AddHeader(CSTRP(sbuff, sptr));

	Data::ByteBuffer tagData(tag->size);
	fd->GetRealData(tag->ofst, tag->size, tagData);
	frame->AddHexBuff(0, CSTR("Start of Packet"), tagData.WithSize(2), false);
	UIntOS frameSize;
	UIntOS currOfst;
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
				UIntOS tz = (UIntOS)ReadMUInt16(&tagData[currOfst + 11]) >> 4;
				if (tagData[currOfst + 12] & 8)
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("GMT-"));
				}
				else
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("GMT+"));
				}
				sptr = Text::StrUIntOS(sptr, tz / 100);
				*sptr++ = ':';
				tz = (tz % 100) * 3 / 5;
				if (tz < 10)
				{
					*sptr++ = '0';
				}
				sptr = Text::StrUIntOS(sptr, (tz % 100) * 3 / 5);
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
			frame->AddUInt(currOfst + 7, 1, CSTR("Length of GPS information"), (UIntOS)tagData[currOfst + 7] >> 4);
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

			UIntOS i;
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
			Text::CStringNN infoName = CSTR("Unknown");
			switch (infoType)
			{
			case 0x00:
				infoName = CSTR("External battery voltage");
				break;
			case 0x01:
			case 0x02:
			case 0x03:
				infoName = CSTR("Custom");
				break;
			case 0x04:
				infoName = CSTR("Terminal status synchronization");
				break;
			case 0x05:
				infoName = CSTR("Door status");
				break;
			case 0x08:
				infoName = CSTR("Self-check parameters");
				break;
			case 0x09:
				infoName = CSTR("Information of visible satellites");
				break;
			case 0x0A:
				infoName = CSTR("ICCID information");
				break;
			case 0x1B:
				infoName = CSTR("RFID");
				break;
			}
			frame->AddHex8Name(currOfst + 1, CSTR("Information Type"), infoType, infoName);
			if (infoType == 4)
			{
				UIntOS fieldOfst = currOfst + 2;
				UIntOS i;
				UIntOS fieldEnd = currOfst + frameSize - 4;
				while (fieldOfst < fieldEnd)
				{
					i = fieldOfst;
					while (i < fieldEnd)
					{
						if (tagData[i] == ';')
						{
							i++;
							break;
						}
						i++;
					}
					frame->AddStrC(fieldOfst, i - fieldOfst, CSTR("Information"), &tagData[fieldOfst]);
					fieldOfst = i;
				}
			}
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
			frame->AddUInt(currOfst + 7, 1, CSTR("Length of GPS information"), (UIntOS)tagData[currOfst + 7] >> 4);
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

			UIntOS i;
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
		{
			Data::DateTime dt;
			dt.SetValue((UInt16)(2000 + tagData[currOfst + 1]), tagData[currOfst + 2], tagData[currOfst + 3], tagData[currOfst + 4], tagData[currOfst + 5], tagData[currOfst + 6], 0, 0);
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			frame->AddField(currOfst + 1, 6, CSTR("Date Time"), CSTRP(sbuff, sptr));
			frame->AddUInt(currOfst + 7, 1, CSTR("GPS information length"), (UIntOS)(tagData[currOfst + 7] >> 4));
			frame->AddUInt(currOfst + 7, 1, CSTR("Number of Satellites used"), tagData[currOfst + 7] & 15);
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
			frame->AddUInt(currOfst + 19, 1, CSTR("LBS length"), tagData[currOfst + 19]);
			frame->AddUInt(currOfst + 20, 2, CSTR("MCC"), ReadMUInt16(&tagData[currOfst + 20]) & 0x7fff);
			UIntOS i;
			if (tagData[currOfst + 20] & 0x80)
			{
				frame->AddUInt(currOfst + 22, 2, CSTR("MNC"), ReadMUInt16(&tagData[currOfst + 22]));
				i = currOfst + 24;
			}
			else
			{
				frame->AddUInt(currOfst + 22, 1, CSTR("MNC"), tagData[currOfst + 22]);
				i = currOfst + 23;
			}
			frame->AddUInt(i, 4, CSTR("LAC"), ReadMUInt32(&tagData[i]));
			frame->AddUInt64(i + 4, CSTR("Cell ID"), ReadMUInt64(&tagData[i + 4]));
			i += 12;
			frame->AddHex8(i, CSTR("Terminal information"), tagData[i]);
			frame->AddBit(i, CSTR("Defense"), tagData[i], 0);
			frame->AddBit(i, CSTR("ACC"), tagData[i], 1);
			frame->AddBit(i, CSTR("Charging"), tagData[i], 2);
			frame->AddUInt(i, 1, CSTR("Battery status"), (tagData[i] >> 3) & 7);
			frame->AddBit(i, CSTR("Position fixed"), tagData[i], 6);
			frame->AddBit(i, CSTR("Cut off fuel/power"), tagData[i], 7);
			frame->AddUInt(i + 1, 1, CSTR("Voltage Level"), tagData[i + 1]);
			frame->AddUInt(i + 2, 1, CSTR("GSM Signal Strength"), tagData[i + 2]);
			UInt16 alarm = ReadMUInt16(&tagData[i + 3]) >> 4;
			Text::CStringNN alarmName = CSTR("Unknown");
			switch (alarm)
			{
			case 0x00:
				alarmName = CSTR("Normal");
				break;
			case 0x01:
				alarmName = CSTR("SOS alert");
				break;
			case 0x02:
				alarmName = CSTR("Power cut alert");
				break;
			case 0x03:
				alarmName = CSTR("Vibrating alert");
				break;
			case 0x04:
				alarmName = CSTR("Entered fence alert");
				break;
			case 0x05:
				alarmName = CSTR("Left fence alert");
				break;
			case 0x06:
				alarmName = CSTR("Speed alert");
				break;
			case 0x09:
				alarmName = CSTR("Tow/theft alert");
				break;
			case 0x0A:
				alarmName = CSTR("Entered GPS blind spot alert");
				break;
			case 0x0B:
				alarmName = CSTR("Left GPS blind spot alert");
				break;
			case 0x0C:
				alarmName = CSTR("Powered on alert");
				break;
			case 0x0D:
				alarmName = CSTR("GPS first fix alert");
				break;
			case 0x0E:
				alarmName = CSTR("Low external battery alert");
				break;
			case 0x0F:
				alarmName = CSTR("External battery low voltage protection alert");
				break;
			case 0x11:
				alarmName = CSTR("Powered off alert");
				break;
			case 0x15:
				alarmName = CSTR("Powered off due to low battery");
				break;
			case 0x19:
				alarmName = CSTR("Low internal battery alert");
				break;
			case 0x29:
				alarmName = CSTR("Harsh acceleration");
				break;
			case 0x2A:
				alarmName = CSTR("Sharp left cornering alert");
				break;
			case 0x2B:
				alarmName = CSTR("Sharp right cornering alert");
				break;
			case 0x2C:
				alarmName = CSTR("Collision alert");
				break;
			case 0x2D:
				alarmName = CSTR("Rollover alert");
				break;
			case 0x30:
				alarmName = CSTR("Harsh braking");
				break;
			case 0x32:
				alarmName = CSTR("Device unplugged alert");
				break;
			case 0xC5:
				alarmName = CSTR("Engine is already turned on");
				break;
			case 0xC6:
				alarmName = CSTR("Engine is already turned off");
				break;
			case 0xC7:
				alarmName = CSTR("Driver has been driving extendedly");
				break;
			case 0xC8:
				alarmName = CSTR("Extended driving of driver is already known");
				break;
			case 0xC9:
				alarmName = CSTR("Idling alert");
				break;
			case 0xFE:
				alarmName = CSTR("ACC ON");
				break;
			case 0xFF:
				alarmName = CSTR("ACC OFF");
				break;
			case 0x0107:
				alarmName = CSTR("4G/LTE jamming range entry alert");
				break;
			case 0x010A:
				alarmName = CSTR("4G/LTE jamming range exit alert");
				break;
			case 0x010B:
				alarmName = CSTR("GPS jamming range entry alert");
				break;
			case 0x010C:
				alarmName = CSTR("GPS jamming range exit alert");
				break;
			}
			frame->AddHex16Name(i + 3, CSTR("Alarm Type"), alarm, alarmName);
			UInt8 lang = tagData[i + 4] & 15;
			Text::CStringNN langName = CSTR("Unknown");
			switch (lang)
			{
			case 0:
				langName = CSTR("No response from the platform is required");
				break;
			case 1:
				langName = CSTR("Chinese");
				break;
			case 2:
				langName = CSTR("English");
				break;
			}
			frame->AddUIntName(i + 4, 1, CSTR("Language"), lang, langName);
			frame->AddUInt(i + 5, 1, CSTR("Fence No."), tagData[i + 5]);
			parsed = true;
		}
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
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::JMVL01FileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::JMVL01FileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
