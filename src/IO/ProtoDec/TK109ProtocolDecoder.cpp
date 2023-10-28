#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/ProtoDec/TK109ProtocolDecoder.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::ProtoDec::TK109ProtocolDecoder::TK109ProtocolDecoder()
{
}

IO::ProtoDec::TK109ProtocolDecoder::~TK109ProtocolDecoder()
{
}

Text::CStringNN IO::ProtoDec::TK109ProtocolDecoder::GetName() const
{
	return CSTR("TK109");
}

UOSInt IO::ProtoDec::TK109ProtocolDecoder::ParseProtocol(ProtocolInfo hdlr, void *userObj, UInt64 fileOfst, UInt8 *buff, UOSInt buffSize)
{
	UOSInt i;
	UOSInt j;
	UInt32 protoSize;
	Text::StringBuilderUTF8 sb;
	j = 0;
	i = 0;
	while (i < buffSize - 6)
	{
		if (buff[i] == 0x67 && buff[i + 1] == 0x67)
		{
			if (j != i)
			{
				hdlr(userObj, fileOfst + j, i - j, CSTR("Unknown Protocol"));
			}
			j = i;
			protoSize = ReadMUInt16(&buff[i + 3]);
			if (i + protoSize + 5 > buffSize)
				return j;
			sb.ClearStr();
			sb.AppendC(UTF8STRC("0x"));
			sb.AppendHex8(buff[i + 2]);
			sb.AppendC(UTF8STRC(", "));
			switch (buff[i + 2])
			{
			case 1:
				sb.AppendC(UTF8STRC("Login Data"));
				break;
			case 2:
				sb.AppendC(UTF8STRC("GPS Data"));
				break;
			case 3:
				sb.AppendC(UTF8STRC("Keep Alive"));
				break;
			case 4:
				sb.AppendC(UTF8STRC("Alert Data"));
				break;
			case 5:
				sb.AppendC(UTF8STRC("Terminal Status"));
				break;
			case 6:
				sb.AppendC(UTF8STRC("Short Message Command Upload"));
				break;
			case 7:
				sb.AppendC(UTF8STRC("OBD Data"));
				break;
			case 8:
				sb.AppendC(UTF8STRC("Pass Through Data"));
				break;
			case 9:
				sb.AppendC(UTF8STRC("OBD Error Data"));
				break;
			case 0x80:
				sb.AppendC(UTF8STRC("Short Message Command Download"));
				break;
			case 0x81:
				sb.AppendC(UTF8STRC("Normal Message Download"));
				break;
			case 0x88:
				sb.AppendC(UTF8STRC("Pass-through from Server to Client"));
				break;
			case 0xe:
				sb.AppendC(UTF8STRC("Photo Info Data"));
				break;
			case 0xf:
				sb.AppendC(UTF8STRC("Photo Content"));
				break;
			default:
				sb.AppendC(UTF8STRC("Unknown Protocol"));
				break;
			}
			hdlr(userObj, fileOfst + j, protoSize + 5, sb.ToCString());
			j += protoSize + 5;
			i = j;
		}
		else
		{
			i++;
		}
	}
	return j;
}

Bool IO::ProtoDec::TK109ProtocolDecoder::GetProtocolDetail(UInt8 *buff, UOSInt buffSize, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (buffSize < 7)
		return false;
	UInt32 protoSize = ReadMUInt16(&buff[3]);
	if (buff[0] != 0x67 || buff[1] != 0x67)
		return false;
	sb->AppendC(UTF8STRC("Protocol Size="));
	sb->AppendU32(protoSize);
	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendC(UTF8STRC("Seq="));
	sb->AppendU16(ReadMUInt16(&buff[5]));
	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendC(UTF8STRC("Protocol Type="));
	sb->AppendU16(buff[2]);
	sb->AppendC(UTF8STRC(", "));
	switch (buff[2])
	{
	case 1:
		{
			sb->AppendC(UTF8STRC("Login Data"));
			sb->AppendC(UTF8STRC("\r\n"));
			if (protoSize >= 10)
			{
				sb->AppendC(UTF8STRC("IMEI="));
				sb->AppendHexBuff(&buff[7], 8, 0, Text::LineBreakType::None);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (protoSize >= 11)
			{
				sb->AppendC(UTF8STRC("Terminal Language="));
				sb->AppendU16(buff[15]);
				sb->AppendC(UTF8STRC(", "));
				switch (buff[15])
				{
				case 0:
					sb->AppendC(UTF8STRC("Simplified Chinese"));
					break;
				case 1:
					sb->AppendC(UTF8STRC("English"));
					break;
				default:
					sb->AppendC(UTF8STRC("Unknown"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (protoSize >= 12)
			{
				sb->AppendC(UTF8STRC("Time Zone="));
				if (buff[16] == 0)
				{
					sb->AppendC(UTF8STRC("+0000"));
				}
				else if (buff[16] & 0x80)
				{
					Int32 tz = (-buff[16]) & 0xff;
					if (tz < 40)
					{
						sb->AppendC(UTF8STRC("-0"));
						sb->AppendI32((tz / 4) * 100 + (tz & 3) * 15);
					}
					else
					{
						sb->AppendC(UTF8STRC("-"));
						sb->AppendI32((tz / 4) * 100 + (tz & 3) * 15);
					}
				}
				else if (buff[16] < 40)
				{
					sb->AppendC(UTF8STRC("+0"));
					sb->AppendU16((UInt16)((buff[16] / 4) * 100 + (buff[16] & 3) * 15));
				}
				else
				{
					sb->AppendC(UTF8STRC("+"));
					sb->AppendU16((UInt16)((buff[16] / 4) * 100 + (buff[16] & 3) * 15));
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
		}
		break;
	case 2:
		sb->AppendC(UTF8STRC("GPS Data"));
		sb->AppendC(UTF8STRC("\r\n"));
		if (protoSize >= 27)
		{
			sb->AppendC(UTF8STRC("GPS Time="));
			sb->AppendTS(Data::Timestamp::FromEpochSec(ReadMUInt32(&buff[7]), Data::DateTimeUtil::GetLocalTzQhr()));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Latitude="));
			Text::SBAppendF64(sb, ReadMInt32(&buff[11]) / 1800000.0);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Longitude="));
			Text::SBAppendF64(sb, ReadMInt32(&buff[15]) / 1800000.0);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Speed="));
			sb->AppendU16(buff[19]);
			sb->AppendC(UTF8STRC("km/h\r\n"));
			sb->AppendC(UTF8STRC("Direction="));
			sb->AppendU16(ReadMUInt16(&buff[20]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("MCC="));
			sb->AppendU16(ReadMUInt16(&buff[22]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("MNC="));
			sb->AppendU16(ReadMUInt16(&buff[24]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("LAC="));
			sb->AppendU16(ReadMUInt16(&buff[26]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("CI="));
			sb->AppendU32(ReadMUInt24(&buff[28]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Location Status=0x"));
			sb->AppendHex8(buff[31]);
			if (buff[31] & 1)
			{
				sb->AppendC(UTF8STRC(" (GPS Active)"));
			}
			else
			{
				sb->AppendC(UTF8STRC(" (GPS Void)"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (protoSize >= 29)
		{
			this->AppendDevStatus(sb, ReadMUInt16(&buff[32]));
		}
		if (protoSize >= 31)
		{
			sb->AppendC(UTF8STRC("Battery Voltage="));
			sb->AppendU16(ReadMUInt16(&buff[34]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (protoSize >= 33)
		{
			sb->AppendC(UTF8STRC("Signal Quality="));
			sb->AppendI32(-110 + ReadMUInt16(&buff[36]));
			sb->AppendC(UTF8STRC("dB\r\n"));
		}
		if (protoSize >= 35)
		{
			sb->AppendC(UTF8STRC("Analog Input 1="));
			sb->AppendU16(ReadMUInt16(&buff[38]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (protoSize >= 37)
		{
			sb->AppendC(UTF8STRC("Analog Input 2="));
			sb->AppendU16(ReadMUInt16(&buff[40]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 3:
		sb->AppendC(UTF8STRC("Keep Alive"));
		sb->AppendC(UTF8STRC("\r\n"));
		if (protoSize >= 4)
		{
			this->AppendDevStatus(sb, ReadMUInt16(&buff[7]));
		}
		break;
	case 4:
		sb->AppendC(UTF8STRC("Alert Data"));
		sb->AppendC(UTF8STRC("\r\n"));
		break;
	case 5:
		sb->AppendC(UTF8STRC("Terminal Status"));
		sb->AppendC(UTF8STRC("\r\n"));
		if (protoSize >= 27)
		{
			sb->AppendC(UTF8STRC("GPS Time="));
			sb->AppendTS(Data::Timestamp::FromEpochSec(ReadMUInt32(&buff[7]), Data::DateTimeUtil::GetLocalTzQhr()));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Latitude="));
			Text::SBAppendF64(sb, ReadMInt32(&buff[11]) / 1800000.0);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Longitude="));
			Text::SBAppendF64(sb, ReadMInt32(&buff[15]) / 1800000.0);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Speed="));
			sb->AppendU16(buff[19]);
			sb->AppendC(UTF8STRC("km/h\r\n"));
			sb->AppendC(UTF8STRC("Direction="));
			sb->AppendU16(ReadMUInt16(&buff[20]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("MCC="));
			sb->AppendU16(ReadMUInt16(&buff[22]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("MNC="));
			sb->AppendU16(ReadMUInt16(&buff[24]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("LAC="));
			sb->AppendU16(ReadMUInt16(&buff[26]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("CI="));
			sb->AppendU32(ReadMUInt24(&buff[28]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Location Status=0x"));
			sb->AppendHex8(buff[31]);
			if (buff[31] & 1)
			{
				sb->AppendC(UTF8STRC(" (GPS Active)"));
			}
			else
			{
				sb->AppendC(UTF8STRC(" (GPS Void)"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (protoSize >= 28)
		{
			sb->AppendC(UTF8STRC("Status Type="));
			switch (buff[32])
			{
			case 1:
				sb->AppendC(UTF8STRC("1, ACC On\r\n"));
				if (protoSize >= 32)
				{
					sb->AppendC(UTF8STRC("Status Time="));
					sb->AppendTS(Data::Timestamp::FromEpochSec(ReadMUInt32(&buff[33]), Data::DateTimeUtil::GetLocalTzQhr()));
					sb->AppendC(UTF8STRC("\r\n"));
				}
				if (protoSize >= 34)
				{
					this->AppendDevStatus(sb, ReadMUInt16(&buff[37]));
				}
				break;
			case 2:
				sb->AppendC(UTF8STRC("2, ACC Off\r\n"));
				if (protoSize >= 32)
				{
					sb->AppendC(UTF8STRC("Status Time="));
					sb->AppendTS(Data::Timestamp::FromEpochSec(ReadMUInt32(&buff[33]), Data::DateTimeUtil::GetLocalTzQhr()));
					sb->AppendC(UTF8STRC("\r\n"));
				}
				if (protoSize >= 34)
				{
					this->AppendDevStatus(sb, ReadMUInt16(&buff[37]));
				}
				break;
			case 3:
				sb->AppendC(UTF8STRC("3, Digital Input Changed\r\n"));
				if (protoSize >= 32)
				{
					sb->AppendC(UTF8STRC("Status Time="));
					sb->AppendTS(Data::Timestamp::FromEpochSec(ReadMUInt32(&buff[33]), Data::DateTimeUtil::GetLocalTzQhr()));
					sb->AppendC(UTF8STRC("\r\n"));
				}
				if (protoSize >= 34)
				{
					this->AppendDevStatus(sb, ReadMUInt16(&buff[37]));
				}
				break;
			default:
				sb->AppendC(UTF8STRC("0x"));
				sb->AppendHex8(buff[32]);
				sb->AppendC(UTF8STRC(", Unknown\r\n"));
				break;
			}
		}
		break;
	case 6:
		sb->AppendC(UTF8STRC("Short Message Command Upload"));
		sb->AppendC(UTF8STRC("\r\n"));
		break;
	case 7:
		sb->AppendC(UTF8STRC("OBD Data"));
		sb->AppendC(UTF8STRC("\r\n"));
		break;
	case 8:
		sb->AppendC(UTF8STRC("Pass Through Data"));
		sb->AppendC(UTF8STRC("\r\n"));
		if (protoSize >= 27)
		{
			sb->AppendC(UTF8STRC("GPS Time="));
			sb->AppendTS(Data::Timestamp::FromEpochSec(ReadMUInt32(&buff[7]), Data::DateTimeUtil::GetLocalTzQhr()));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Latitude="));
			Text::SBAppendF64(sb, ReadMInt32(&buff[11]) / 1800000.0);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Longitude="));
			Text::SBAppendF64(sb, ReadMInt32(&buff[15]) / 1800000.0);
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Speed="));
			sb->AppendU16(buff[19]);
			sb->AppendC(UTF8STRC("km/h\r\n"));
			sb->AppendC(UTF8STRC("Direction="));
			sb->AppendU16(ReadMUInt16(&buff[20]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("MCC="));
			sb->AppendU16(ReadMUInt16(&buff[22]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("MNC="));
			sb->AppendU16(ReadMUInt16(&buff[24]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("LAC="));
			sb->AppendU16(ReadMUInt16(&buff[26]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("CI="));
			sb->AppendU32(ReadMUInt24(&buff[28]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Location Status=0x"));
			sb->AppendHex8(buff[31]);
			if (buff[31] & 1)
			{
				sb->AppendC(UTF8STRC(" (GPS Active)"));
			}
			else
			{
				sb->AppendC(UTF8STRC(" (GPS Void)"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (protoSize >= 37)
		{
			sb->AppendC(UTF8STRC("Pass through data="));
			sb->AppendHexBuff(&buff[32], 10, ' ', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 9:
		sb->AppendC(UTF8STRC("OBD Error Data"));
		sb->AppendC(UTF8STRC("\r\n"));
		break;
	case 0x80:
		sb->AppendC(UTF8STRC("Short Message Command Download"));
		sb->AppendC(UTF8STRC("\r\n"));
		break;
	case 0x81:
		sb->AppendC(UTF8STRC("Normal Message Download"));
		sb->AppendC(UTF8STRC("\r\n"));
		break;
	case 0x88:
		sb->AppendC(UTF8STRC("Pass-through from Server to Client"));
		sb->AppendC(UTF8STRC("\r\n"));
		if (protoSize >= 2)
		{
			sb->AppendC(UTF8STRC("Pass through data="));
			sb->AppendHexBuff(&buff[7], protoSize - 2, ' ', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 0xe:
		sb->AppendC(UTF8STRC("Photo Info Data"));
		sb->AppendC(UTF8STRC("\r\n"));
		break;
	case 0xf:
		sb->AppendC(UTF8STRC("Photo Content"));
		sb->AppendC(UTF8STRC("\r\n"));
		break;
	default:
		sb->AppendC(UTF8STRC("Unknown"));
		sb->AppendC(UTF8STRC("\r\n"));
		break;
	}
	return true;
}

Bool IO::ProtoDec::TK109ProtocolDecoder::IsValid(UInt8 *buff, UOSInt buffSize)
{
	return buff[0] == 0x67 && buff[1] == 0x67;
}

void IO::ProtoDec::TK109ProtocolDecoder::AppendDevStatus(NotNullPtr<Text::StringBuilderUTF8> sb, UInt16 status)
{
	sb->AppendC(UTF8STRC("Device Status=0x"));
	sb->AppendHex16(status);
	sb->AppendC(UTF8STRC("\r\n"));
	if (status & 1)
	{
		sb->AppendC(UTF8STRC("-GPS Active\r\n"));
	}
	else
	{
		sb->AppendC(UTF8STRC("-GPS Void\r\n"));
	}
	if ((status & 6) == 0)
	{
		sb->AppendC(UTF8STRC("-No ACC\r\n"));
	}
	else if ((status & 6) == 4)
	{
		sb->AppendC(UTF8STRC("-ACC Off\r\n"));
	}
	else if ((status & 6) == 6)
	{
		sb->AppendC(UTF8STRC("-ACC On\r\n"));
	}
	if ((status & 0x18) == 0)
	{
		sb->AppendC(UTF8STRC("-No Arming\r\n"));
	}
	else if ((status & 0x18) == 0x10)
	{
		sb->AppendC(UTF8STRC("-Disarmed\r\n"));
	}
	else if ((status & 0x18) == 0x18)
	{
		sb->AppendC(UTF8STRC("-Armed\r\n"));
	}
	if ((status & 0x60) == 0)
	{
		sb->AppendC(UTF8STRC("-No Oil\r\n"));
	}
	else if ((status & 0x60) == 0x40)
	{
		sb->AppendC(UTF8STRC("-Oil Disconnected\r\n"));
	}
	else if ((status & 0x60) == 0x60)
	{
		sb->AppendC(UTF8STRC("-Oil Connected\r\n"));
	}
	if ((status & 0x180) == 0)
	{
		sb->AppendC(UTF8STRC("-No Charger\r\n"));
	}
	else if ((status & 0x180) == 0x100)
	{
		sb->AppendC(UTF8STRC("-Charger Disconnected\r\n"));
	}
	else if ((status & 0x180) == 0x180)
	{
		sb->AppendC(UTF8STRC("-Charger Connected\r\n"));
	}
	if (status & 0x1000)
	{
		sb->AppendC(UTF8STRC("-Input 1 Hi\r\n"));
	}
	else
	{
		sb->AppendC(UTF8STRC("-Input 1 Lo\r\n"));
	}
	if (status & 0x2000)
	{
		sb->AppendC(UTF8STRC("-Input 2 Hi\r\n"));
	}
	else
	{
		sb->AppendC(UTF8STRC("-Input 2 Lo\r\n"));
	}
	if (status & 0x4000)
	{
		sb->AppendC(UTF8STRC("-Input 3 Hi\r\n"));
	}
	else
	{
		sb->AppendC(UTF8STRC("-Input 3 Lo\r\n"));
	}
	if (status & 0x8000)
	{
		sb->AppendC(UTF8STRC("-Input 4 Hi\r\n"));
	}
	else
	{
		sb->AppendC(UTF8STRC("-Input 4 Lo\r\n"));
	}

}
