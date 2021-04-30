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

const UTF8Char *IO::ProtoDec::TK109ProtocolDecoder::GetName()
{
	return (const UTF8Char*)"TK109";
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
				hdlr(userObj, fileOfst + j, i - j, (const UTF8Char*)"Unknown Protocol");
			}
			j = i;
			protoSize = ReadMUInt16(&buff[i + 3]);
			if (i + protoSize + 5 > buffSize)
				return j;
			sb.ClearStr();
			sb.Append((const UTF8Char*)"0x");
			sb.AppendHex8(buff[i + 2]);
			sb.Append((const UTF8Char*)", ");
			switch (buff[i + 2])
			{
			case 1:
				sb.Append((const UTF8Char*)"Login Data");
				break;
			case 2:
				sb.Append((const UTF8Char*)"GPS Data");
				break;
			case 3:
				sb.Append((const UTF8Char*)"Keep Alive");
				break;
			case 4:
				sb.Append((const UTF8Char*)"Alert Data");
				break;
			case 5:
				sb.Append((const UTF8Char*)"Terminal Status");
				break;
			case 6:
				sb.Append((const UTF8Char*)"Short Message Command Upload");
				break;
			case 7:
				sb.Append((const UTF8Char*)"OBD Data");
				break;
			case 8:
				sb.Append((const UTF8Char*)"Pass Through Data");
				break;
			case 9:
				sb.Append((const UTF8Char*)"OBD Error Data");
				break;
			case 0x80:
				sb.Append((const UTF8Char*)"Short Message Command Download");
				break;
			case 0x81:
				sb.Append((const UTF8Char*)"Normal Message Download");
				break;
			case 0x88:
				sb.Append((const UTF8Char*)"Pass-through from Server to Client");
				break;
			case 0xe:
				sb.Append((const UTF8Char*)"Photo Info Data");
				break;
			case 0xf:
				sb.Append((const UTF8Char*)"Photo Content");
				break;
			default:
				sb.Append((const UTF8Char*)"Unknown Protocol");
				break;
			}
			hdlr(userObj, fileOfst + j, protoSize + 5, sb.ToString());
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

Bool IO::ProtoDec::TK109ProtocolDecoder::GetProtocolDetail(UInt8 *buff, UOSInt buffSize, Text::StringBuilderUTF *sb)
{
	if (buffSize < 7)
		return false;
	UInt32 protoSize = ReadMUInt16(&buff[3]);
	if (buff[0] != 0x67 || buff[1] != 0x67)
		return false;
	sb->Append((const UTF8Char*)"Protocol Size=");
	sb->AppendU32(protoSize);
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"Seq=");
	sb->AppendU16(ReadMUInt16(&buff[5]));
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"Protocol Type=");
	sb->AppendU16(buff[2]);
	sb->Append((const UTF8Char*)", ");
	switch (buff[2])
	{
	case 1:
		{
			sb->Append((const UTF8Char*)"Login Data");
			sb->Append((const UTF8Char*)"\r\n");
			if (protoSize >= 10)
			{
				sb->Append((const UTF8Char*)"IMEI=");
				sb->AppendHexBuff(&buff[7], 8, 0, Text::LBT_NONE);
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (protoSize >= 11)
			{
				sb->Append((const UTF8Char*)"Terminal Language=");
				sb->AppendU16(buff[15]);
				sb->Append((const UTF8Char*)", ");
				switch (buff[15])
				{
				case 0:
					sb->Append((const UTF8Char*)"Simplified Chinese");
					break;
				case 1:
					sb->Append((const UTF8Char*)"English");
					break;
				default:
					sb->Append((const UTF8Char*)"Unknown");
					break;
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			if (protoSize >= 12)
			{
				sb->Append((const UTF8Char*)"Time Zone=");
				if (buff[16] == 0)
				{
					sb->Append((const UTF8Char*)"+0000");
				}
				else if (buff[16] & 0x80)
				{
					Int32 tz = (-buff[16]) & 0xff;
					if (tz < 40)
					{
						sb->Append((const UTF8Char*)"-0");
						sb->AppendI32((tz / 4) * 100 + (tz & 3) * 15);
					}
					else
					{
						sb->Append((const UTF8Char*)"-");
						sb->AppendI32((tz / 4) * 100 + (tz & 3) * 15);
					}
				}
				else if (buff[16] < 40)
				{
					sb->Append((const UTF8Char*)"+0");
					sb->AppendU16((UInt16)((buff[16] / 4) * 100 + (buff[16] & 3) * 15));
				}
				else
				{
					sb->Append((const UTF8Char*)"+");
					sb->AppendU16((UInt16)((buff[16] / 4) * 100 + (buff[16] & 3) * 15));
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
		}
		break;
	case 2:
		sb->Append((const UTF8Char*)"GPS Data");
		sb->Append((const UTF8Char*)"\r\n");
		if (protoSize >= 27)
		{
			Data::DateTime dt;
			dt.SetUnixTimestamp(ReadMUInt32(&buff[7]));
			dt.ToLocalTime();
			sb->Append((const UTF8Char*)"GPS Time=");
			sb->AppendDate(&dt);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Latitude=");
			Text::SBAppendF64(sb, ReadMInt32(&buff[11]) / 1800000.0);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Longitude=");
			Text::SBAppendF64(sb, ReadMInt32(&buff[15]) / 1800000.0);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Speed=");
			sb->AppendU16(buff[19]);
			sb->Append((const UTF8Char*)"km/h\r\n");
			sb->Append((const UTF8Char*)"Direction=");
			sb->AppendU16(ReadMUInt16(&buff[20]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"MCC=");
			sb->AppendU16(ReadMUInt16(&buff[22]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"MNC=");
			sb->AppendU16(ReadMUInt16(&buff[24]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"LAC=");
			sb->AppendU16(ReadMUInt16(&buff[26]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"CI=");
			sb->AppendU32(ReadMUInt24(&buff[28]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Location Status=0x");
			sb->AppendHex8(buff[31]);
			if (buff[31] & 1)
			{
				sb->Append((const UTF8Char*)" (GPS Active)");
			}
			else
			{
				sb->Append((const UTF8Char*)" (GPS Void)");
			}
			sb->Append((const UTF8Char*)"\r\n");
		}
		if (protoSize >= 29)
		{
			this->AppendDevStatus(sb, ReadMUInt16(&buff[32]));
		}
		if (protoSize >= 31)
		{
			sb->Append((const UTF8Char*)"Battery Voltage=");
			sb->AppendU16(ReadMUInt16(&buff[34]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		if (protoSize >= 33)
		{
			sb->Append((const UTF8Char*)"Signal Quality=");
			sb->AppendI32(-110 + ReadMUInt16(&buff[36]));
			sb->Append((const UTF8Char*)"dB\r\n");
		}
		if (protoSize >= 35)
		{
			sb->Append((const UTF8Char*)"Analog Input 1=");
			sb->AppendU16(ReadMUInt16(&buff[38]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		if (protoSize >= 37)
		{
			sb->Append((const UTF8Char*)"Analog Input 2=");
			sb->AppendU16(ReadMUInt16(&buff[40]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 3:
		sb->Append((const UTF8Char*)"Keep Alive");
		sb->Append((const UTF8Char*)"\r\n");
		if (protoSize >= 4)
		{
			this->AppendDevStatus(sb, ReadMUInt16(&buff[7]));
		}
		break;
	case 4:
		sb->Append((const UTF8Char*)"Alert Data");
		sb->Append((const UTF8Char*)"\r\n");
		break;
	case 5:
		sb->Append((const UTF8Char*)"Terminal Status");
		sb->Append((const UTF8Char*)"\r\n");
		if (protoSize >= 27)
		{
			Data::DateTime dt;
			dt.SetUnixTimestamp(ReadMUInt32(&buff[7]));
			dt.ToLocalTime();
			sb->Append((const UTF8Char*)"GPS Time=");
			sb->AppendDate(&dt);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Latitude=");
			Text::SBAppendF64(sb, ReadMInt32(&buff[11]) / 1800000.0);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Longitude=");
			Text::SBAppendF64(sb, ReadMInt32(&buff[15]) / 1800000.0);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Speed=");
			sb->AppendU16(buff[19]);
			sb->Append((const UTF8Char*)"km/h\r\n");
			sb->Append((const UTF8Char*)"Direction=");
			sb->AppendU16(ReadMUInt16(&buff[20]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"MCC=");
			sb->AppendU16(ReadMUInt16(&buff[22]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"MNC=");
			sb->AppendU16(ReadMUInt16(&buff[24]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"LAC=");
			sb->AppendU16(ReadMUInt16(&buff[26]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"CI=");
			sb->AppendU32(ReadMUInt24(&buff[28]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Location Status=0x");
			sb->AppendHex8(buff[31]);
			if (buff[31] & 1)
			{
				sb->Append((const UTF8Char*)" (GPS Active)");
			}
			else
			{
				sb->Append((const UTF8Char*)" (GPS Void)");
			}
			sb->Append((const UTF8Char*)"\r\n");
		}
		if (protoSize >= 28)
		{
			Data::DateTime dt;
			sb->Append((const UTF8Char*)"Status Type=");
			switch (buff[32])
			{
			case 1:
				sb->Append((const UTF8Char*)"1, ACC On\r\n");
				if (protoSize >= 32)
				{
					dt.SetUnixTimestamp(ReadMUInt32(&buff[33]));
					dt.ToLocalTime();
					sb->Append((const UTF8Char*)"Status Time=");
					sb->AppendDate(&dt);
					sb->Append((const UTF8Char*)"\r\n");
				}
				if (protoSize >= 34)
				{
					this->AppendDevStatus(sb, ReadMUInt16(&buff[37]));
				}
				break;
			case 2:
				sb->Append((const UTF8Char*)"2, ACC Off\r\n");
				if (protoSize >= 32)
				{
					dt.SetUnixTimestamp(ReadMUInt32(&buff[33]));
					dt.ToLocalTime();
					sb->Append((const UTF8Char*)"Status Time=");
					sb->AppendDate(&dt);
					sb->Append((const UTF8Char*)"\r\n");
				}
				if (protoSize >= 34)
				{
					this->AppendDevStatus(sb, ReadMUInt16(&buff[37]));
				}
				break;
			case 3:
				sb->Append((const UTF8Char*)"3, Digital Input Changed\r\n");
				if (protoSize >= 32)
				{
					dt.SetUnixTimestamp(ReadMUInt32(&buff[33]));
					dt.ToLocalTime();
					sb->Append((const UTF8Char*)"Status Time=");
					sb->AppendDate(&dt);
					sb->Append((const UTF8Char*)"\r\n");
				}
				if (protoSize >= 34)
				{
					this->AppendDevStatus(sb, ReadMUInt16(&buff[37]));
				}
				break;
			default:
				sb->Append((const UTF8Char*)"0x");
				sb->AppendHex8(buff[32]);
				sb->Append((const UTF8Char*)", Unknown\r\n");
				break;
			}
		}
		break;
	case 6:
		sb->Append((const UTF8Char*)"Short Message Command Upload");
		sb->Append((const UTF8Char*)"\r\n");
		break;
	case 7:
		sb->Append((const UTF8Char*)"OBD Data");
		sb->Append((const UTF8Char*)"\r\n");
		break;
	case 8:
		sb->Append((const UTF8Char*)"Pass Through Data");
		sb->Append((const UTF8Char*)"\r\n");
		if (protoSize >= 27)
		{
			Data::DateTime dt;
			dt.SetUnixTimestamp(ReadMUInt32(&buff[7]));
			dt.ToLocalTime();
			sb->Append((const UTF8Char*)"GPS Time=");
			sb->AppendDate(&dt);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Latitude=");
			Text::SBAppendF64(sb, ReadMInt32(&buff[11]) / 1800000.0);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Longitude=");
			Text::SBAppendF64(sb, ReadMInt32(&buff[15]) / 1800000.0);
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Speed=");
			sb->AppendU16(buff[19]);
			sb->Append((const UTF8Char*)"km/h\r\n");
			sb->Append((const UTF8Char*)"Direction=");
			sb->AppendU16(ReadMUInt16(&buff[20]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"MCC=");
			sb->AppendU16(ReadMUInt16(&buff[22]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"MNC=");
			sb->AppendU16(ReadMUInt16(&buff[24]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"LAC=");
			sb->AppendU16(ReadMUInt16(&buff[26]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"CI=");
			sb->AppendU32(ReadMUInt24(&buff[28]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Location Status=0x");
			sb->AppendHex8(buff[31]);
			if (buff[31] & 1)
			{
				sb->Append((const UTF8Char*)" (GPS Active)");
			}
			else
			{
				sb->Append((const UTF8Char*)" (GPS Void)");
			}
			sb->Append((const UTF8Char*)"\r\n");
		}
		if (protoSize >= 37)
		{
			sb->Append((const UTF8Char*)"Pass through data=");
			sb->AppendHexBuff(&buff[32], 10, ' ', Text::LBT_NONE);
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 9:
		sb->Append((const UTF8Char*)"OBD Error Data");
		sb->Append((const UTF8Char*)"\r\n");
		break;
	case 0x80:
		sb->Append((const UTF8Char*)"Short Message Command Download");
		sb->Append((const UTF8Char*)"\r\n");
		break;
	case 0x81:
		sb->Append((const UTF8Char*)"Normal Message Download");
		sb->Append((const UTF8Char*)"\r\n");
		break;
	case 0x88:
		sb->Append((const UTF8Char*)"Pass-through from Server to Client");
		sb->Append((const UTF8Char*)"\r\n");
		if (protoSize >= 2)
		{
			sb->Append((const UTF8Char*)"Pass through data=");
			sb->AppendHexBuff(&buff[7], protoSize - 2, ' ', Text::LBT_NONE);
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 0xe:
		sb->Append((const UTF8Char*)"Photo Info Data");
		sb->Append((const UTF8Char*)"\r\n");
		break;
	case 0xf:
		sb->Append((const UTF8Char*)"Photo Content");
		sb->Append((const UTF8Char*)"\r\n");
		break;
	default:
		sb->Append((const UTF8Char*)"Unknown");
		sb->Append((const UTF8Char*)"\r\n");
		break;
	}
	return true;
}

Bool IO::ProtoDec::TK109ProtocolDecoder::IsValid(UInt8 *buff, UOSInt buffSize)
{
	return buff[0] == 0x67 && buff[1] == 0x67;
}

void IO::ProtoDec::TK109ProtocolDecoder::AppendDevStatus(Text::StringBuilderUTF *sb, UInt16 status)
{
	sb->Append((const UTF8Char*)"Device Status=0x");
	sb->AppendHex16(status);
	sb->Append((const UTF8Char*)"\r\n");
	if (status & 1)
	{
		sb->Append((const UTF8Char*)"-GPS Active\r\n");
	}
	else
	{
		sb->Append((const UTF8Char*)"-GPS Void\r\n");
	}
	if ((status & 6) == 0)
	{
		sb->Append((const UTF8Char*)"-No ACC\r\n");
	}
	else if ((status & 6) == 4)
	{
		sb->Append((const UTF8Char*)"-ACC Off\r\n");
	}
	else if ((status & 6) == 6)
	{
		sb->Append((const UTF8Char*)"-ACC On\r\n");
	}
	if ((status & 0x18) == 0)
	{
		sb->Append((const UTF8Char*)"-No Arming\r\n");
	}
	else if ((status & 0x18) == 0x10)
	{
		sb->Append((const UTF8Char*)"-Disarmed\r\n");
	}
	else if ((status & 0x18) == 0x18)
	{
		sb->Append((const UTF8Char*)"-Armed\r\n");
	}
	if ((status & 0x60) == 0)
	{
		sb->Append((const UTF8Char*)"-No Oil\r\n");
	}
	else if ((status & 0x60) == 0x40)
	{
		sb->Append((const UTF8Char*)"-Oil Disconnected\r\n");
	}
	else if ((status & 0x60) == 0x60)
	{
		sb->Append((const UTF8Char*)"-Oil Connected\r\n");
	}
	if ((status & 0x180) == 0)
	{
		sb->Append((const UTF8Char*)"-No Charger\r\n");
	}
	else if ((status & 0x180) == 0x100)
	{
		sb->Append((const UTF8Char*)"-Charger Disconnected\r\n");
	}
	else if ((status & 0x180) == 0x180)
	{
		sb->Append((const UTF8Char*)"-Charger Connected\r\n");
	}
	if (status & 0x1000)
	{
		sb->Append((const UTF8Char*)"-Input 1 Hi\r\n");
	}
	else
	{
		sb->Append((const UTF8Char*)"-Input 1 Lo\r\n");
	}
	if (status & 0x2000)
	{
		sb->Append((const UTF8Char*)"-Input 2 Hi\r\n");
	}
	else
	{
		sb->Append((const UTF8Char*)"-Input 2 Lo\r\n");
	}
	if (status & 0x4000)
	{
		sb->Append((const UTF8Char*)"-Input 3 Hi\r\n");
	}
	else
	{
		sb->Append((const UTF8Char*)"-Input 3 Lo\r\n");
	}
	if (status & 0x8000)
	{
		sb->Append((const UTF8Char*)"-Input 4 Hi\r\n");
	}
	else
	{
		sb->Append((const UTF8Char*)"-Input 4 Lo\r\n");
	}

}
