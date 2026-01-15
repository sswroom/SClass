#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/ProtoDec/JTT808ProtocolDecoder.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::ProtoDec::JTT808ProtocolDecoder::JTT808ProtocolDecoder()
{
}

IO::ProtoDec::JTT808ProtocolDecoder::~JTT808ProtocolDecoder()
{
}

Text::CStringNN IO::ProtoDec::JTT808ProtocolDecoder::GetName() const
{
	return CSTR("JTT808");
}

UIntOS IO::ProtoDec::JTT808ProtocolDecoder::ParseProtocol(ProtocolInfo hdlr, AnyType userObj, UInt64 fileOfst, UnsafeArray<UInt8> buff, UIntOS buffSize)
{
	UInt8 proto[1024];
	UIntOS i;
	IntOS protoStart = -1;
	IntOS unkStart = -1;
	UIntOS protoSize;
	Text::StringBuilderUTF8 sb;
	i = 0;
	while (i < buffSize)
	{
		if (buff[i] == 0x7e)
		{
			if (unkStart >= 0)
			{
				hdlr(userObj, fileOfst + (UIntOS)unkStart, i - (UIntOS)unkStart, CSTR("Unknown Protocol"));
				unkStart = -1;
			}
			if (protoStart < 0)
			{
				protoStart = (IntOS)i;
			}
			else if (i - (UIntOS)protoStart < 1025)
			{
				protoSize = Unpack(proto, &buff[protoStart], i - (UIntOS)protoStart + 1);
				if (protoSize >= 12)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("0x"));
					sb.AppendHex16(ReadMUInt16(&proto[0]));
					sb.AppendC(UTF8STRC(", "));
					switch (ReadMUInt16(&proto[0]))
					{
					case 0x0001:
						sb.AppendC(UTF8STRC("Terminal General Reply"));
						break;
					case 0x0002:
						sb.AppendC(UTF8STRC("Keep-Alive"));
						break;
					case 0x0003:
						sb.AppendC(UTF8STRC("Terminal Unregister"));
						break;
					case 0x0100:
						sb.AppendC(UTF8STRC("Terminal Register"));
						break;
					case 0x0102:
						sb.AppendC(UTF8STRC("Terminal Autherize"));
						break;
					case 0x0104:
						sb.AppendC(UTF8STRC("Query Terminal Parameter Reply"));
						break;
					case 0x0107:
						sb.AppendC(UTF8STRC("Query Terminal Attribute Reply"));
						break;
					case 0x0108:
						sb.AppendC(UTF8STRC("OTA Update Reply"));
						break;
					case 0x0200:
						sb.AppendC(UTF8STRC("Location Message"));
						break;
					case 0x0201:
						sb.AppendC(UTF8STRC("Qusery Location Message Reply"));
						break;
					case 0x0301:
						sb.AppendC(UTF8STRC("Event Report"));
						break;
					case 0x0704:
						sb.AppendC(UTF8STRC("Batch Location Upload"));
						break;
					case 0x0900:
						sb.AppendC(UTF8STRC("Pass through data upload"));
						break;
					case 0x8001:
						sb.AppendC(UTF8STRC("Platform General Reply"));
						break;
					case 0x8003:
						sb.AppendC(UTF8STRC("Request Resend"));
						break;
					case 0x8100:
						sb.AppendC(UTF8STRC("Terminal Register Reply"));
						break;
					case 0x8103:
						sb.AppendC(UTF8STRC("Set Terminal Parameter"));
						break;
					case 0x8104:
						sb.AppendC(UTF8STRC("Query Terminal Parameter"));
						break;
					case 0x8105:
						sb.AppendC(UTF8STRC("Terminal Control"));
						break;
					case 0x8106:
						sb.AppendC(UTF8STRC("Query Specific Terminal Parameter"));
						break;
					case 0x8107:
						sb.AppendC(UTF8STRC("Query Terminal Attribute"));
						break;
					case 0x8108:
						sb.AppendC(UTF8STRC("OTA Update Packet"));
						break;
					case 0x8201:
						sb.AppendC(UTF8STRC("Query Location Message"));
						break;
					case 0x8202:
						sb.AppendC(UTF8STRC("Temporary Location Tracking"));
						break;
					case 0x8203:
						sb.AppendC(UTF8STRC("Temporary Location Tracking"));
						break;
					case 0x8300:
						sb.AppendC(UTF8STRC("Document Message Download"));
						break;
					case 0x8301:
						sb.AppendC(UTF8STRC("Event Setting"));
						break;
					default:
						sb.AppendC(UTF8STRC("Unknown Protocol"));
						break;
					}
					hdlr(userObj, fileOfst + (UIntOS)protoStart, i - (UIntOS)protoStart + 1, sb.ToCString());
				}
				else
				{
					hdlr(userObj, fileOfst + (UIntOS)protoStart, i - (UIntOS)protoStart, CSTR("Unknown Protocol"));
				}
				protoStart = -1;
			}
			else
			{
				hdlr(userObj, fileOfst + (UIntOS)protoStart, i - (UIntOS)protoStart, CSTR("Unknown Protocol"));
				protoStart = (IntOS)i;
			}
		}
		else
		{
			if (protoStart < 0 && unkStart < 0)
			{
				unkStart = (IntOS)i;
			}
		}
		i++;
	}
	if (unkStart >= 0)
		return (UIntOS)unkStart;
	if (protoStart >= 0)
		return (UIntOS)protoStart;
	return buffSize;
}

Bool IO::ProtoDec::JTT808ProtocolDecoder::GetProtocolDetail(UnsafeArray<UInt8> buff, UIntOS buffSize, NN<Text::StringBuilderUTF8> sb)
{
	UInt8 proto[1024];
	UIntOS protoSize;
	if (buff[0] != 0x7e || buff[buffSize - 1] != 0x7e)
		return false;
	protoSize = Unpack(proto, buff, buffSize);
	if (protoSize < 13)
		return false;
	Bool valid = false;
	UIntOS msgLeng;
	sb->AppendC(UTF8STRC("Protocol Size="));
	sb->AppendUIntOS(protoSize);
	sb->AppendC(UTF8STRC("\r\nMessage Flag=0x"));
	sb->AppendHex16(ReadMUInt16(&proto[2]));
	sb->AppendC(UTF8STRC("\r\nMessage Length="));
	msgLeng = ReadMUInt16(&proto[2]) & 0x3ff;
	sb->AppendU16(ReadMUInt16(&proto[2]) & 0x3ff);
	sb->AppendC(UTF8STRC("\r\nTerminal Id="));
	sb->AppendHexBuff(&proto[4], 6, 0, Text::LineBreakType::None);
	sb->AppendC(UTF8STRC("\r\nSeq="));
	sb->AppendU16(ReadMUInt16(&proto[10]));
	if (msgLeng == protoSize - 1 - 12)
	{
		UInt8 chk = 0;
		UIntOS i;
		sb->AppendC(UTF8STRC("\r\nCheckDigits=0x"));
		sb->AppendHex8(proto[protoSize - 1]);
		i = protoSize;
		while (i-- > 0)
		{
			chk ^= proto[i];
		}
		if (chk == 0)
		{
			sb->AppendC(UTF8STRC(", valid"));
			valid = true;
		}
		else
		{
			sb->AppendC(UTF8STRC(", invalid"));
		}
	}
	sb->AppendC(UTF8STRC("\r\nMessage Id=0x"));
	sb->AppendHex16(ReadMUInt16(&proto[0]));
	if (valid)
	{
		sb->AppendC(UTF8STRC(", "));
		switch (ReadMUInt16(&proto[0]))
		{
		case 0x0001:
			sb->AppendC(UTF8STRC("Terminal General Reply"));
			break;
		case 0x0002:
			sb->AppendC(UTF8STRC("Keep-Alive"));
			break;
		case 0x0003:
			sb->AppendC(UTF8STRC("Terminal Unregister"));
			break;
		case 0x0100:
			sb->AppendC(UTF8STRC("Terminal Register"));
			break;
		case 0x0102:
			sb->AppendC(UTF8STRC("Terminal Autherize"));
			sb->AppendC(UTF8STRC("\r\nAuther Code="));
			sb->AppendC(&proto[12], msgLeng);
			break;
		case 0x0104:
			sb->AppendC(UTF8STRC("Query Terminal Parameter Reply"));
			break;
		case 0x0107:
			sb->AppendC(UTF8STRC("Query Terminal Attribute Reply"));
			break;
		case 0x0108:
			sb->AppendC(UTF8STRC("OTA Update Reply"));
			break;
		case 0x0200:
			sb->AppendC(UTF8STRC("Location Message"));
			this->ParseLocation(&proto[12], msgLeng, sb);
			break;
		case 0x0201:
			sb->AppendC(UTF8STRC("Qusery Location Message Reply"));
			break;
		case 0x0301:
			sb->AppendC(UTF8STRC("Event Report"));
			break;
		case 0x0704:
			sb->AppendC(UTF8STRC("Batch Location Upload"));
			sb->AppendC(UTF8STRC("\r\nData Count="));
			sb->AppendU16(ReadMUInt16(&proto[12]));
			sb->AppendC(UTF8STRC("\r\nBlind Zone="));
			sb->AppendU16(proto[14]);
			{
				UIntOS i = ReadMUInt16(&proto[12]);
				UIntOS j = 15;
				UIntOS locSize;
				while (i > 0 && j + 2 <= msgLeng + 12)
				{
					i--;
					locSize = ReadMUInt16(&proto[j]);
					sb->AppendC(UTF8STRC("\r\nLocation Size="));
					sb->AppendUIntOS(locSize);

					if (j + 2 + locSize <= msgLeng + 12)
					{
						this->ParseLocation(&proto[j + 2], locSize, sb);
					}
					j += 2 + locSize;
				}
			}
			break;
		case 0x0900:
			sb->AppendC(UTF8STRC("Pass through data upload"));
			sb->AppendC(UTF8STRC("\r\nMessage Type="));
			sb->AppendU16(proto[12]);
			sb->AppendC(UTF8STRC("\r\nContent="));
			sb->AppendHexBuff(&proto[13], msgLeng - 1, ' ', Text::LineBreakType::CRLF);
			break;
		case 0x8001:
			sb->AppendC(UTF8STRC("Platform General Reply"));
			break;
		case 0x8003:
			sb->AppendC(UTF8STRC("Request Resend"));
			break;
		case 0x8100:
			sb->AppendC(UTF8STRC("Terminal Register Reply"));
			break;
		case 0x8103:
			sb->AppendC(UTF8STRC("Set Terminal Parameter"));
			break;
		case 0x8104:
			sb->AppendC(UTF8STRC("Query Terminal Parameter"));
			break;
		case 0x8105:
			sb->AppendC(UTF8STRC("Terminal Control"));
			break;
		case 0x8106:
			sb->AppendC(UTF8STRC("Query Specific Terminal Parameter"));
			break;
		case 0x8107:
			sb->AppendC(UTF8STRC("Query Terminal Attribute"));
			break;
		case 0x8108:
			sb->AppendC(UTF8STRC("OTA Update Packet"));
			break;
		case 0x8201:
			sb->AppendC(UTF8STRC("Query Location Message"));
			break;
		case 0x8202:
			sb->AppendC(UTF8STRC("Temporary Location Tracking"));
			break;
		case 0x8203:
			sb->AppendC(UTF8STRC("Temporary Location Tracking"));
			break;
		case 0x8300:
			sb->AppendC(UTF8STRC("Document Message Download"));
			break;
		case 0x8301:
			sb->AppendC(UTF8STRC("Event Setting"));
			break;
		default:
			sb->AppendC(UTF8STRC("Unknown Protocol"));
			break;
		}
	}
	return true;
}

Bool IO::ProtoDec::JTT808ProtocolDecoder::IsValid(UnsafeArray<UInt8> buff, UIntOS buffSize)
{
	return buff[0] == 0x7e;
}

UIntOS IO::ProtoDec::JTT808ProtocolDecoder::Unpack(UnsafeArray<UInt8> buff, UnsafeArray<const UInt8> proto, UIntOS protoSize)
{
	UIntOS retSize = protoSize - 2;
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

Bool IO::ProtoDec::JTT808ProtocolDecoder::ParseLocation(UnsafeArray<const UTF8Char> loc, UIntOS locSize, NN<Text::StringBuilderUTF8> sb)
{
	UInt32 uVal;
	UIntOS i;
	if (locSize >= 28)
	{
		uVal = ReadMUInt32(&loc[0]);
		sb->AppendC(UTF8STRC("\r\nAlert Flags=0x"));
		sb->AppendHex32(uVal);
		if (uVal & 0x00000001) sb->AppendC(UTF8STRC(", SOS Alert"));
		if (uVal & 0x00000002) sb->AppendC(UTF8STRC(", Overspeed Alert"));
		if (uVal & 0x00000004) sb->AppendC(UTF8STRC(", Tired Driving"));
		if (uVal & 0x00000008) sb->AppendC(UTF8STRC(", Danger Pre-Alert"));
		if (uVal & 0x00000010) sb->AppendC(UTF8STRC(", GNSS Module Error"));
		if (uVal & 0x00000020) sb->AppendC(UTF8STRC(", GNSS Antenna Error"));
		if (uVal & 0x00000040) sb->AppendC(UTF8STRC(", GNSS Antenna Short Circuit"));
		if (uVal & 0x00000080) sb->AppendC(UTF8STRC(", Main Unit Low Voltage"));
		if (uVal & 0x00000100) sb->AppendC(UTF8STRC(", Main Unit No Power"));
		if (uVal & 0x00000200) sb->AppendC(UTF8STRC(", LCD Monitor Failure"));
		if (uVal & 0x00000400) sb->AppendC(UTF8STRC(", TTS Module Failure"));
		if (uVal & 0x00000800) sb->AppendC(UTF8STRC(", Camera Failure"));
		if (uVal & 0x00001000) sb->AppendC(UTF8STRC(", IC Card Module Failure"));
		if (uVal & 0x00002000) sb->AppendC(UTF8STRC(", Overspeed Pre-Alert"));
		if (uVal & 0x00004000) sb->AppendC(UTF8STRC(", Tired Driving Pre-Alert"));
		if (uVal & 0x00008000) sb->AppendC(UTF8STRC(", Reserved"));
		if (uVal & 0x00010000) sb->AppendC(UTF8STRC(", Reserved"));
		if (uVal & 0x00020000) sb->AppendC(UTF8STRC(", Reserved"));
		if (uVal & 0x00040000) sb->AppendC(UTF8STRC(", Daily Driving Overtime"));
		if (uVal & 0x00080000) sb->AppendC(UTF8STRC(", Parking Overtime"));
		if (uVal & 0x00100000) sb->AppendC(UTF8STRC(", Zone Alert"));
		if (uVal & 0x00200000) sb->AppendC(UTF8STRC(", Route Alert"));
		if (uVal & 0x00400000) sb->AppendC(UTF8STRC(", Route Duration Alert"));
		if (uVal & 0x00800000) sb->AppendC(UTF8STRC(", Route Offset Alert"));
		if (uVal & 0x01000000) sb->AppendC(UTF8STRC(", VSS Failure"));
		if (uVal & 0x02000000) sb->AppendC(UTF8STRC(", Fuel Abnormal"));
		if (uVal & 0x04000000) sb->AppendC(UTF8STRC(", Armed Alert"));
		if (uVal & 0x08000000) sb->AppendC(UTF8STRC(", Illegal ACC"));
		if (uVal & 0x10000000) sb->AppendC(UTF8STRC(", Illegal Move"));
		if (uVal & 0x20000000) sb->AppendC(UTF8STRC(", Collision Pre-Alert"));
		if (uVal & 0x40000000) sb->AppendC(UTF8STRC(", Turn over Pre-Alert"));
		if (uVal & 0x80000000) sb->AppendC(UTF8STRC(", Door Alert"));

		uVal = ReadMUInt32(&loc[4]);
		sb->AppendC(UTF8STRC("\r\nStatus=0x"));
		sb->AppendHex32(uVal);
		if (uVal & 0x00000001) sb->AppendC(UTF8STRC(", ACC On"));
		if (uVal & 0x00000002) sb->AppendC(UTF8STRC(", Valid"));
		if (uVal & 0x00000004) sb->AppendC(UTF8STRC(", South"));
		if (uVal & 0x00000008) sb->AppendC(UTF8STRC(", West"));
		if (uVal & 0x00000010) sb->AppendC(UTF8STRC(", Non-operation"));
		if (uVal & 0x00000020) sb->AppendC(UTF8STRC(", Lat/Lon Encrypted"));
		if (uVal & 0x00000040) sb->AppendC(UTF8STRC(", Reserved"));
		if (uVal & 0x00000080) sb->AppendC(UTF8STRC(", Reserved"));
		if ((uVal & 0x300) == 0x000) sb->AppendC(UTF8STRC(", Empty"));
		if ((uVal & 0x300) == 0x100) sb->AppendC(UTF8STRC(", Half-Load"));
		if ((uVal & 0x300) == 0x200) sb->AppendC(UTF8STRC(", Reserved"));
		if ((uVal & 0x300) == 0x300) sb->AppendC(UTF8STRC(", Full-Load"));
		if (uVal & 0x00000400) sb->AppendC(UTF8STRC(", Fuel broken"));
		if (uVal & 0x00000800) sb->AppendC(UTF8STRC(", Power broken"));
		if (uVal & 0x00001000) sb->AppendC(UTF8STRC(", Door locked"));
		if (uVal & 0x00002000) sb->AppendC(UTF8STRC(", Door 1 opened"));
		if (uVal & 0x00004000) sb->AppendC(UTF8STRC(", Door 2 opened"));
		if (uVal & 0x00008000) sb->AppendC(UTF8STRC(", Door 3 opened"));
		if (uVal & 0x00010000) sb->AppendC(UTF8STRC(", Door 4 opened"));
		if (uVal & 0x00020000) sb->AppendC(UTF8STRC(", Door 5 opened"));
		if (uVal & 0x00040000) sb->AppendC(UTF8STRC(", GPS Positioning"));
		if (uVal & 0x00080000) sb->AppendC(UTF8STRC(", Beitou Positioning"));
		if (uVal & 0x00100000) sb->AppendC(UTF8STRC(", GLONASS Positioning "));
		if (uVal & 0x00200000) sb->AppendC(UTF8STRC(", Galileo Positioning"));

		sb->AppendC(UTF8STRC("\r\nLatitude="));
		Text::SBAppendF64(sb, ReadMUInt32(&loc[8]) / 1000000.0);
		sb->AppendC(UTF8STRC("\r\nLongitude="));
		Text::SBAppendF64(sb, ReadMUInt32(&loc[12]) / 1000000.0);
		sb->AppendC(UTF8STRC("\r\nAltitude="));
		sb->AppendU16(ReadMUInt16(&loc[16]));
		sb->AppendC(UTF8STRC("\r\nSpeed="));
		Text::SBAppendF64(sb, ReadMUInt16(&loc[18]) / 10.0);
		sb->AppendC(UTF8STRC("\r\nDirection="));
		sb->AppendU16(ReadMUInt16(&loc[20]));
		sb->AppendC(UTF8STRC("\r\nTime=20"));
		sb->AppendHex8(loc[22]);
		sb->AppendC(UTF8STRC("-"));
		sb->AppendHex8(loc[23]);
		sb->AppendC(UTF8STRC("-"));
		sb->AppendHex8(loc[24]);
		sb->AppendC(UTF8STRC(" "));
		sb->AppendHex8(loc[25]);
		sb->AppendC(UTF8STRC(":"));
		sb->AppendHex8(loc[26]);
		sb->AppendC(UTF8STRC(":"));
		sb->AppendHex8(loc[27]);
		sb->AppendC(UTF8STRC("+0800"));
	}
	else
	{
		return false;
	}
	i = 28;
	while (i + 2 <= locSize)
	{
		sb->AppendC(UTF8STRC("\r\nExtra data ID="));
		sb->AppendU16(loc[i]);
		sb->AppendC(UTF8STRC(", size="));
		sb->AppendU16(loc[i + 1]);
		if (i + 2 + loc[i + 1] <= locSize)
		{
			switch (loc[i])
			{
			case 1:
				if (loc[i + 1] >= 4)
				{
					sb->AppendC(UTF8STRC("\r\nMileage="));
					Text::SBAppendF64(sb, ReadMUInt32(&loc[i + 2]) * 0.1);
					sb->AppendC(UTF8STRC("km"));
				}
				break;
			case 2:
				if (loc[i + 1] >= 2)
				{
					sb->AppendC(UTF8STRC("\r\nFuel level="));
					Text::SBAppendF64(sb, ReadMUInt16(&loc[i + 2]) * 0.1);
					sb->AppendC(UTF8STRC("L"));
				}
				break;
			case 3:
				if (loc[i + 1] >= 2)
				{
					sb->AppendC(UTF8STRC("\r\nSpeed="));
					Text::SBAppendF64(sb, ReadMUInt16(&loc[i + 2]) * 0.1);
					sb->AppendC(UTF8STRC("km/h"));
				}
				break;
			case 4:
				if (loc[i + 1] >= 2)
				{
					sb->AppendC(UTF8STRC("\r\nWait for confirm event="));
					sb->AppendU16(ReadMUInt16(&loc[i + 2]));
				}
				break;
			case 0x11:
				if (loc[i + 1] >= 1)
				{
					sb->AppendC(UTF8STRC("\r\nOverspeed alert location type="));
					sb->AppendU16(loc[i + 2]);
					if ((loc[i + 2] == 1 || loc[i + 2] == 2 || loc[i + 2] == 3) && loc[i + 1] >= 5)
					{
						sb->AppendC(UTF8STRC("\r\nZone Id="));
						sb->AppendU32(ReadMUInt32(&loc[i + 3]));
					}
				}
				break;
			case 0x12:
				if (loc[i + 1] >= 6)
				{
					sb->AppendC(UTF8STRC("\r\nZone alert zone type="));
					sb->AppendU16(loc[i + 2]);
					sb->AppendC(UTF8STRC("\r\nZone Id="));
					sb->AppendU32(ReadMUInt32(&loc[i + 3]));
					sb->AppendC(UTF8STRC("\r\nOutZone="));
					sb->AppendU16(loc[i + 7]);
				}
				break;
			case 0x13:
				if (loc[i + 1] >= 7)
				{
					sb->AppendC(UTF8STRC("\r\nRoute Driving Time Alert Route Id="));
					sb->AppendU32(ReadMUInt32(&loc[i + 2]));
					sb->AppendC(UTF8STRC("\r\nDriving Time="));
					sb->AppendU16(ReadMUInt16(&loc[i + 6]));
					sb->AppendC(UTF8STRC("s\r\nType="));
					sb->AppendU16(loc[i + 8]);
				}
				break;
			case 0x25:
				if (loc[i + 1] >= 4)
				{
					sb->AppendC(UTF8STRC("\r\nSignal Status=0x"));
					sb->AppendHex32(ReadMUInt32(&loc[i + 2]));
				}
				break;
			case 0x2a:
				if (loc[i + 1] >= 2)
				{
					sb->AppendC(UTF8STRC("\r\nIO Status=0x"));
					sb->AppendHex16(ReadMUInt16(&loc[i + 2]));
				}
				break;
			case 0x2b:
				if (loc[i + 1] >= 4)
				{
					sb->AppendC(UTF8STRC("\r\nADC Status, AD0="));
					sb->AppendU16(ReadMUInt16(&loc[i + 2]));
					sb->AppendC(UTF8STRC("\r\nAD1="));
					sb->AppendU16(ReadMUInt16(&loc[i + 4]));
				}
				break;
			case 0x30:
				if (loc[i + 1] >= 1)
				{
					sb->AppendC(UTF8STRC("\r\nWireless Network Signal Strength="));
					sb->AppendU16(loc[i + 2]);
				}
				break;
			case 0x31:
				if (loc[i + 1] >= 1)
				{
					sb->AppendC(UTF8STRC("\r\nGNSS Sate Count="));
					sb->AppendU16(loc[i + 2]);
				}
				break;
			}
		}
		i += 2 + (UIntOS)loc[i + 1];
	}
	return true;
}
