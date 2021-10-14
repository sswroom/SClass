#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/ProtoDec/JTT808ProtocolDecoder.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::ProtoDec::JTT808ProtocolDecoder::JTT808ProtocolDecoder()
{
}

IO::ProtoDec::JTT808ProtocolDecoder::~JTT808ProtocolDecoder()
{
}

const UTF8Char *IO::ProtoDec::JTT808ProtocolDecoder::GetName()
{
	return (const UTF8Char*)"JTT808";
}

UOSInt IO::ProtoDec::JTT808ProtocolDecoder::ParseProtocol(ProtocolInfo hdlr, void *userObj, UInt64 fileOfst, UInt8 *buff, UOSInt buffSize)
{
	UInt8 proto[1024];
	UOSInt i;
	OSInt protoStart = -1;
	OSInt unkStart = -1;
	UOSInt protoSize;
	Text::StringBuilderUTF8 sb;
	i = 0;
	while (i < buffSize)
	{
		if (buff[i] == 0x7e)
		{
			if (unkStart >= 0)
			{
				hdlr(userObj, fileOfst + (UOSInt)unkStart, i - (UOSInt)unkStart, (const UTF8Char*)"Unknown Protocol");
				unkStart = -1;
			}
			if (protoStart < 0)
			{
				protoStart = (OSInt)i;
			}
			else if (i - (UOSInt)protoStart < 1025)
			{
				protoSize = Unpack(proto, &buff[protoStart], i - (UOSInt)protoStart + 1);
				if (protoSize >= 12)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"0x");
					sb.AppendHex16(ReadMUInt16(&proto[0]));
					sb.Append((const UTF8Char*)", ");
					switch (ReadMUInt16(&proto[0]))
					{
					case 0x0001:
						sb.Append((const UTF8Char*)"Terminal General Reply");
						break;
					case 0x0002:
						sb.Append((const UTF8Char*)"Keep-Alive");
						break;
					case 0x0003:
						sb.Append((const UTF8Char*)"Terminal Unregister");
						break;
					case 0x0100:
						sb.Append((const UTF8Char*)"Terminal Register");
						break;
					case 0x0102:
						sb.Append((const UTF8Char*)"Terminal Autherize");
						break;
					case 0x0104:
						sb.Append((const UTF8Char*)"Query Terminal Parameter Reply");
						break;
					case 0x0107:
						sb.Append((const UTF8Char*)"Query Terminal Attribute Reply");
						break;
					case 0x0108:
						sb.Append((const UTF8Char*)"OTA Update Reply");
						break;
					case 0x0200:
						sb.Append((const UTF8Char*)"Location Message");
						break;
					case 0x0201:
						sb.Append((const UTF8Char*)"Qusery Location Message Reply");
						break;
					case 0x0301:
						sb.Append((const UTF8Char*)"Event Report");
						break;
					case 0x0704:
						sb.Append((const UTF8Char*)"Batch Location Upload");
						break;
					case 0x0900:
						sb.Append((const UTF8Char*)"Pass through data upload");
						break;
					case 0x8001:
						sb.Append((const UTF8Char*)"Platform General Reply");
						break;
					case 0x8003:
						sb.Append((const UTF8Char*)"Request Resend");
						break;
					case 0x8100:
						sb.Append((const UTF8Char*)"Terminal Register Reply");
						break;
					case 0x8103:
						sb.Append((const UTF8Char*)"Set Terminal Parameter");
						break;
					case 0x8104:
						sb.Append((const UTF8Char*)"Query Terminal Parameter");
						break;
					case 0x8105:
						sb.Append((const UTF8Char*)"Terminal Control");
						break;
					case 0x8106:
						sb.Append((const UTF8Char*)"Query Specific Terminal Parameter");
						break;
					case 0x8107:
						sb.Append((const UTF8Char*)"Query Terminal Attribute");
						break;
					case 0x8108:
						sb.Append((const UTF8Char*)"OTA Update Packet");
						break;
					case 0x8201:
						sb.Append((const UTF8Char*)"Query Location Message");
						break;
					case 0x8202:
						sb.Append((const UTF8Char*)"Temporary Location Tracking");
						break;
					case 0x8203:
						sb.Append((const UTF8Char*)"Temporary Location Tracking");
						break;
					case 0x8300:
						sb.Append((const UTF8Char*)"Document Message Download");
						break;
					case 0x8301:
						sb.Append((const UTF8Char*)"Event Setting");
						break;
					default:
						sb.Append((const UTF8Char*)"Unknown Protocol");
						break;
					}
					hdlr(userObj, fileOfst + (UOSInt)protoStart, i - (UOSInt)protoStart + 1, sb.ToString());
				}
				else
				{
					hdlr(userObj, fileOfst + (UOSInt)protoStart, i - (UOSInt)protoStart, (const UTF8Char*)"Unknown Protocol");
				}
				protoStart = -1;
			}
			else
			{
				hdlr(userObj, fileOfst + (UOSInt)protoStart, i - (UOSInt)protoStart, (const UTF8Char*)"Unknown Protocol");
				protoStart = (OSInt)i;
			}
		}
		else
		{
			if (protoStart < 0 && unkStart < 0)
			{
				unkStart = (OSInt)i;
			}
		}
		i++;
	}
	if (unkStart >= 0)
		return (UOSInt)unkStart;
	if (protoStart >= 0)
		return (UOSInt)protoStart;
	return buffSize;
}

Bool IO::ProtoDec::JTT808ProtocolDecoder::GetProtocolDetail(UInt8 *buff, UOSInt buffSize, Text::StringBuilderUTF *sb)
{
	UInt8 proto[1024];
	UOSInt protoSize;
	if (buff[0] != 0x7e || buff[buffSize - 1] != 0x7e)
		return false;
	protoSize = Unpack(proto, buff, buffSize);
	if (protoSize < 13)
		return false;
	Bool valid = false;
	UOSInt msgLeng;
	sb->Append((const UTF8Char*)"Protocol Size=");
	sb->AppendUOSInt(protoSize);
	sb->Append((const UTF8Char*)"\r\nMessage Flag=0x");
	sb->AppendHex16(ReadMUInt16(&proto[2]));
	sb->Append((const UTF8Char*)"\r\nMessage Length=");
	msgLeng = ReadMUInt16(&proto[2]) & 0x3ff;
	sb->AppendU16(ReadMUInt16(&proto[2]) & 0x3ff);
	sb->Append((const UTF8Char*)"\r\nTerminal Id=");
	sb->AppendHexBuff(&proto[4], 6, 0, Text::LineBreakType::None);
	sb->Append((const UTF8Char*)"\r\nSeq=");
	sb->AppendU16(ReadMUInt16(&proto[10]));
	if (msgLeng == protoSize - 1 - 12)
	{
		UInt8 chk = 0;
		UOSInt i;
		sb->Append((const UTF8Char*)"\r\nCheckDigits=0x");
		sb->AppendHex8(proto[protoSize - 1]);
		i = protoSize;
		while (i-- > 0)
		{
			chk ^= proto[i];
		}
		if (chk == 0)
		{
			sb->Append((const UTF8Char*)", valid");
			valid = true;
		}
		else
		{
			sb->Append((const UTF8Char*)", invalid");
		}
	}
	sb->Append((const UTF8Char*)"\r\nMessage Id=0x");
	sb->AppendHex16(ReadMUInt16(&proto[0]));
	if (valid)
	{
		sb->Append((const UTF8Char*)", ");
		switch (ReadMUInt16(&proto[0]))
		{
		case 0x0001:
			sb->Append((const UTF8Char*)"Terminal General Reply");
			break;
		case 0x0002:
			sb->Append((const UTF8Char*)"Keep-Alive");
			break;
		case 0x0003:
			sb->Append((const UTF8Char*)"Terminal Unregister");
			break;
		case 0x0100:
			sb->Append((const UTF8Char*)"Terminal Register");
			break;
		case 0x0102:
			sb->Append((const UTF8Char*)"Terminal Autherize");
			sb->Append((const UTF8Char*)"\r\nAuther Code=");
			sb->AppendC(&proto[12], msgLeng);
			break;
		case 0x0104:
			sb->Append((const UTF8Char*)"Query Terminal Parameter Reply");
			break;
		case 0x0107:
			sb->Append((const UTF8Char*)"Query Terminal Attribute Reply");
			break;
		case 0x0108:
			sb->Append((const UTF8Char*)"OTA Update Reply");
			break;
		case 0x0200:
			sb->Append((const UTF8Char*)"Location Message");
			this->ParseLocation(&proto[12], msgLeng, sb);
			break;
		case 0x0201:
			sb->Append((const UTF8Char*)"Qusery Location Message Reply");
			break;
		case 0x0301:
			sb->Append((const UTF8Char*)"Event Report");
			break;
		case 0x0704:
			sb->Append((const UTF8Char*)"Batch Location Upload");
			sb->Append((const UTF8Char*)"\r\nData Count=");
			sb->AppendU16(ReadMUInt16(&proto[12]));
			sb->Append((const UTF8Char*)"\r\nBlind Zone=");
			sb->AppendU16(proto[14]);
			{
				UOSInt i = ReadMUInt16(&proto[12]);
				UOSInt j = 15;
				UOSInt locSize;
				while (i > 0 && j + 2 <= msgLeng + 12)
				{
					i--;
					locSize = ReadMUInt16(&proto[j]);
					sb->Append((const UTF8Char*)"\r\nLocation Size=");
					sb->AppendUOSInt(locSize);

					if (j + 2 + locSize <= msgLeng + 12)
					{
						this->ParseLocation(&proto[j + 2], locSize, sb);
					}
					j += 2 + locSize;
				}
			}
			break;
		case 0x0900:
			sb->Append((const UTF8Char*)"Pass through data upload");
			sb->Append((const UTF8Char*)"\r\nMessage Type=");
			sb->AppendU16(proto[12]);
			sb->Append((const UTF8Char*)"\r\nContent=");
			sb->AppendHexBuff(&proto[13], msgLeng - 1, ' ', Text::LineBreakType::CRLF);
			break;
		case 0x8001:
			sb->Append((const UTF8Char*)"Platform General Reply");
			break;
		case 0x8003:
			sb->Append((const UTF8Char*)"Request Resend");
			break;
		case 0x8100:
			sb->Append((const UTF8Char*)"Terminal Register Reply");
			break;
		case 0x8103:
			sb->Append((const UTF8Char*)"Set Terminal Parameter");
			break;
		case 0x8104:
			sb->Append((const UTF8Char*)"Query Terminal Parameter");
			break;
		case 0x8105:
			sb->Append((const UTF8Char*)"Terminal Control");
			break;
		case 0x8106:
			sb->Append((const UTF8Char*)"Query Specific Terminal Parameter");
			break;
		case 0x8107:
			sb->Append((const UTF8Char*)"Query Terminal Attribute");
			break;
		case 0x8108:
			sb->Append((const UTF8Char*)"OTA Update Packet");
			break;
		case 0x8201:
			sb->Append((const UTF8Char*)"Query Location Message");
			break;
		case 0x8202:
			sb->Append((const UTF8Char*)"Temporary Location Tracking");
			break;
		case 0x8203:
			sb->Append((const UTF8Char*)"Temporary Location Tracking");
			break;
		case 0x8300:
			sb->Append((const UTF8Char*)"Document Message Download");
			break;
		case 0x8301:
			sb->Append((const UTF8Char*)"Event Setting");
			break;
		default:
			sb->Append((const UTF8Char*)"Unknown Protocol");
			break;
		}
	}
	return true;
}

Bool IO::ProtoDec::JTT808ProtocolDecoder::IsValid(UInt8 *buff, UOSInt buffSize)
{
	return buff[0] == 0x7e;
}

UOSInt IO::ProtoDec::JTT808ProtocolDecoder::Unpack(UInt8 *buff, const UInt8 *proto, UOSInt protoSize)
{
	UOSInt retSize = protoSize - 2;
	if (proto[0] != 0x7e || proto[protoSize - 1] != 0x7e)
		return 0;
	protoSize -= 2;
	proto++;
	while (protoSize-- > 0)
	{
		if (proto[0] == 0x7d && proto[1] == 1)
		{
			*buff++ = 0x7d;
			proto += 2;
			protoSize--;
			retSize--;
		}
		else if (proto[0] == 0x7d && proto[1] == 2)
		{
			*buff++ = 0x7e;
			proto += 2;
			protoSize--;
			retSize--;
		}
		else
		{
			*buff++ = *proto++;
		}
	}
	return retSize;
}

Bool IO::ProtoDec::JTT808ProtocolDecoder::ParseLocation(const UTF8Char *loc, UOSInt locSize, Text::StringBuilderUTF *sb)
{
	UInt32 uVal;
	UOSInt i;
	if (locSize >= 28)
	{
		uVal = ReadMUInt32(&loc[0]);
		sb->Append((const UTF8Char*)"\r\nAlert Flags=0x");
		sb->AppendHex32(uVal);
		if (uVal & 0x00000001) sb->Append((const UTF8Char*)", SOS Alert");
		if (uVal & 0x00000002) sb->Append((const UTF8Char*)", Overspeed Alert");
		if (uVal & 0x00000004) sb->Append((const UTF8Char*)", Tired Driving");
		if (uVal & 0x00000008) sb->Append((const UTF8Char*)", Danger Pre-Alert");
		if (uVal & 0x00000010) sb->Append((const UTF8Char*)", GNSS Module Error");
		if (uVal & 0x00000020) sb->Append((const UTF8Char*)", GNSS Antenna Error");
		if (uVal & 0x00000040) sb->Append((const UTF8Char*)", GNSS Antenna Short Circuit");
		if (uVal & 0x00000080) sb->Append((const UTF8Char*)", Main Unit Low Voltage");
		if (uVal & 0x00000100) sb->Append((const UTF8Char*)", Main Unit No Power");
		if (uVal & 0x00000200) sb->Append((const UTF8Char*)", LCD Monitor Failure");
		if (uVal & 0x00000400) sb->Append((const UTF8Char*)", TTS Module Failure");
		if (uVal & 0x00000800) sb->Append((const UTF8Char*)", Camera Failure");
		if (uVal & 0x00001000) sb->Append((const UTF8Char*)", IC Card Module Failure");
		if (uVal & 0x00002000) sb->Append((const UTF8Char*)", Overspeed Pre-Alert");
		if (uVal & 0x00004000) sb->Append((const UTF8Char*)", Tired Driving Pre-Alert");
		if (uVal & 0x00008000) sb->Append((const UTF8Char*)", Reserved");
		if (uVal & 0x00010000) sb->Append((const UTF8Char*)", Reserved");
		if (uVal & 0x00020000) sb->Append((const UTF8Char*)", Reserved");
		if (uVal & 0x00040000) sb->Append((const UTF8Char*)", Daily Driving Overtime");
		if (uVal & 0x00080000) sb->Append((const UTF8Char*)", Parking Overtime");
		if (uVal & 0x00100000) sb->Append((const UTF8Char*)", Zone Alert");
		if (uVal & 0x00200000) sb->Append((const UTF8Char*)", Route Alert");
		if (uVal & 0x00400000) sb->Append((const UTF8Char*)", Route Duration Alert");
		if (uVal & 0x00800000) sb->Append((const UTF8Char*)", Route Offset Alert");
		if (uVal & 0x01000000) sb->Append((const UTF8Char*)", VSS Failure");
		if (uVal & 0x02000000) sb->Append((const UTF8Char*)", Fuel Abnormal");
		if (uVal & 0x04000000) sb->Append((const UTF8Char*)", Armed Alert");
		if (uVal & 0x08000000) sb->Append((const UTF8Char*)", Illegal ACC");
		if (uVal & 0x10000000) sb->Append((const UTF8Char*)", Illegal Move");
		if (uVal & 0x20000000) sb->Append((const UTF8Char*)", Collision Pre-Alert");
		if (uVal & 0x40000000) sb->Append((const UTF8Char*)", Turn over Pre-Alert");
		if (uVal & 0x80000000) sb->Append((const UTF8Char*)", Door Alert");

		uVal = ReadMUInt32(&loc[4]);
		sb->Append((const UTF8Char*)"\r\nStatus=0x");
		sb->AppendHex32(uVal);
		if (uVal & 0x00000001) sb->Append((const UTF8Char*)", ACC On");
		if (uVal & 0x00000002) sb->Append((const UTF8Char*)", Valid");
		if (uVal & 0x00000004) sb->Append((const UTF8Char*)", South");
		if (uVal & 0x00000008) sb->Append((const UTF8Char*)", West");
		if (uVal & 0x00000010) sb->Append((const UTF8Char*)", Non-operation");
		if (uVal & 0x00000020) sb->Append((const UTF8Char*)", Lat/Lon Encrypted");
		if (uVal & 0x00000040) sb->Append((const UTF8Char*)", Reserved");
		if (uVal & 0x00000080) sb->Append((const UTF8Char*)", Reserved");
		if ((uVal & 0x300) == 0x000) sb->Append((const UTF8Char*)", Empty");
		if ((uVal & 0x300) == 0x100) sb->Append((const UTF8Char*)", Half-Load");
		if ((uVal & 0x300) == 0x200) sb->Append((const UTF8Char*)", Reserved");
		if ((uVal & 0x300) == 0x300) sb->Append((const UTF8Char*)", Full-Load");
		if (uVal & 0x00000400) sb->Append((const UTF8Char*)", Fuel broken");
		if (uVal & 0x00000800) sb->Append((const UTF8Char*)", Power broken");
		if (uVal & 0x00001000) sb->Append((const UTF8Char*)", Door locked");
		if (uVal & 0x00002000) sb->Append((const UTF8Char*)", Door 1 opened");
		if (uVal & 0x00004000) sb->Append((const UTF8Char*)", Door 2 opened");
		if (uVal & 0x00008000) sb->Append((const UTF8Char*)", Door 3 opened");
		if (uVal & 0x00010000) sb->Append((const UTF8Char*)", Door 4 opened");
		if (uVal & 0x00020000) sb->Append((const UTF8Char*)", Door 5 opened");
		if (uVal & 0x00040000) sb->Append((const UTF8Char*)", GPS Positioning");
		if (uVal & 0x00080000) sb->Append((const UTF8Char*)", Beitou Positioning");
		if (uVal & 0x00100000) sb->Append((const UTF8Char*)", GLONASS Positioning ");
		if (uVal & 0x00200000) sb->Append((const UTF8Char*)", Galileo Positioning");

		sb->Append((const UTF8Char*)"\r\nLatitude=");
		Text::SBAppendF64(sb, ReadMUInt32(&loc[8]) / 1000000.0);
		sb->Append((const UTF8Char*)"\r\nLongitude=");
		Text::SBAppendF64(sb, ReadMUInt32(&loc[12]) / 1000000.0);
		sb->Append((const UTF8Char*)"\r\nAltitude=");
		sb->AppendU16(ReadMUInt16(&loc[16]));
		sb->Append((const UTF8Char*)"\r\nSpeed=");
		Text::SBAppendF64(sb, ReadMUInt16(&loc[18]) / 10.0);
		sb->Append((const UTF8Char*)"\r\nDirection=");
		sb->AppendU16(ReadMUInt16(&loc[20]));
		sb->Append((const UTF8Char*)"\r\nTime=20");
		sb->AppendHex8(loc[22]);
		sb->Append((const UTF8Char*)"-");
		sb->AppendHex8(loc[23]);
		sb->Append((const UTF8Char*)"-");
		sb->AppendHex8(loc[24]);
		sb->Append((const UTF8Char*)" ");
		sb->AppendHex8(loc[25]);
		sb->Append((const UTF8Char*)":");
		sb->AppendHex8(loc[26]);
		sb->Append((const UTF8Char*)":");
		sb->AppendHex8(loc[27]);
		sb->Append((const UTF8Char*)"+0800");
	}
	else
	{
		return false;
	}
	i = 28;
	while (i + 2 <= locSize)
	{
		sb->Append((const UTF8Char*)"\r\nExtra data ID=");
		sb->AppendU16(loc[i]);
		sb->Append((const UTF8Char*)", size=");
		sb->AppendU16(loc[i + 1]);
		if (i + 2 + loc[i + 1] <= locSize)
		{
			switch (loc[i])
			{
			case 1:
				if (loc[i + 1] >= 4)
				{
					sb->Append((const UTF8Char*)"\r\nMileage=");
					Text::SBAppendF64(sb, ReadMUInt32(&loc[i + 2]) * 0.1);
					sb->Append((const UTF8Char*)"km");
				}
				break;
			case 2:
				if (loc[i + 1] >= 2)
				{
					sb->Append((const UTF8Char*)"\r\nFuel level=");
					Text::SBAppendF64(sb, ReadMUInt16(&loc[i + 2]) * 0.1);
					sb->Append((const UTF8Char*)"L");
				}
				break;
			case 3:
				if (loc[i + 1] >= 2)
				{
					sb->Append((const UTF8Char*)"\r\nSpeed=");
					Text::SBAppendF64(sb, ReadMUInt16(&loc[i + 2]) * 0.1);
					sb->Append((const UTF8Char*)"km/h");
				}
				break;
			case 4:
				if (loc[i + 1] >= 2)
				{
					sb->Append((const UTF8Char*)"\r\nWait for confirm event=");
					sb->AppendU16(ReadMUInt16(&loc[i + 2]));
				}
				break;
			case 0x11:
				if (loc[i + 1] >= 1)
				{
					sb->Append((const UTF8Char*)"\r\nOverspeed alert location type=");
					sb->AppendU16(loc[i + 2]);
					if ((loc[i + 2] == 1 || loc[i + 2] == 2 || loc[i + 2] == 3) && loc[i + 1] >= 5)
					{
						sb->Append((const UTF8Char*)"\r\nZone Id=");
						sb->AppendU32(ReadMUInt32(&loc[i + 3]));
					}
				}
				break;
			case 0x12:
				if (loc[i + 1] >= 6)
				{
					sb->Append((const UTF8Char*)"\r\nZone alert zone type=");
					sb->AppendU16(loc[i + 2]);
					sb->Append((const UTF8Char*)"\r\nZone Id=");
					sb->AppendU32(ReadMUInt32(&loc[i + 3]));
					sb->Append((const UTF8Char*)"\r\nOutZone=");
					sb->AppendU16(loc[i + 7]);
				}
				break;
			case 0x13:
				if (loc[i + 1] >= 7)
				{
					sb->Append((const UTF8Char*)"\r\nRoute Driving Time Alert Route Id=");
					sb->AppendU32(ReadMUInt32(&loc[i + 2]));
					sb->Append((const UTF8Char*)"\r\nDriving Time=");
					sb->AppendU16(ReadMUInt16(&loc[i + 6]));
					sb->Append((const UTF8Char*)"s\r\nType=");
					sb->AppendU16(loc[i + 8]);
				}
				break;
			case 0x25:
				if (loc[i + 1] >= 4)
				{
					sb->Append((const UTF8Char*)"\r\nSignal Status=0x");
					sb->AppendHex32(ReadMUInt32(&loc[i + 2]));
				}
				break;
			case 0x2a:
				if (loc[i + 1] >= 2)
				{
					sb->Append((const UTF8Char*)"\r\nIO Status=0x");
					sb->AppendHex16(ReadMUInt16(&loc[i + 2]));
				}
				break;
			case 0x2b:
				if (loc[i + 1] >= 4)
				{
					sb->Append((const UTF8Char*)"\r\nADC Status, AD0=");
					sb->AppendU16(ReadMUInt16(&loc[i + 2]));
					sb->Append((const UTF8Char*)"\r\nAD1=");
					sb->AppendU16(ReadMUInt16(&loc[i + 4]));
				}
				break;
			case 0x30:
				if (loc[i + 1] >= 1)
				{
					sb->Append((const UTF8Char*)"\r\nWireless Network Signal Strength=");
					sb->AppendU16(loc[i + 2]);
				}
				break;
			case 0x31:
				if (loc[i + 1] >= 1)
				{
					sb->Append((const UTF8Char*)"\r\nGNSS Sate Count=");
					sb->AppendU16(loc[i + 2]);
				}
				break;
			}
		}
		i += 2 + (UOSInt)loc[i + 1];
	}
	return true;
}
