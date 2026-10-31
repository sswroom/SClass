#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "IO/BTUtil.h"
#include "IO/FileAnalyse/SBFrameDetail.h"
#include "Net/PacketAnalyzerBluetooth.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void Net::PacketAnalyzerBluetooth::AddDirection(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt32 dir)
{
	switch (dir)
	{
	case 0:
		frame->AddField(frameOfst, 4, CSTR("Direction"), CSTR("Sent"));
		break;
	case 1:
		frame->AddField(frameOfst, 4, CSTR("Direction"), CSTR("Rcvd"));
		break;
	default:
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Unknown (0x"));
			sb.AppendHex32(dir);
			sb.AppendUTF8Char(')');
			frame->AddField(frameOfst, 4, CSTR("Direction"), sb.ToCString());
		}
		break;
	}
}

void Net::PacketAnalyzerBluetooth::AddHCIPacketType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 packetType)
{
	frame->AddHex8Name(frameOfst, CSTR("HCI Packet Type"), packetType, HCIPacketTypeGetName(packetType));
}

void Net::PacketAnalyzerBluetooth::AddParamLen(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 len)
{
	frame->AddUInt(frameOfst, 1, CSTR("Parameter Total Length"), len);
}

void Net::PacketAnalyzerBluetooth::AddCmdOpcode(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 cmd)
{
	frame->AddHex16Name(frameOfst, CSTR("Command Opcode"), cmd, CmdGetName(cmd));
}

void Net::PacketAnalyzerBluetooth::AddBDAddr(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Text::CStringNN name, UnsafeArray<const UInt8> mac, Bool randomAddr)
{
	UInt8 macBuff[6];
	macBuff[0] = mac[5];
	macBuff[1] = mac[4];
	macBuff[2] = mac[3];
	macBuff[3] = mac[2];
	macBuff[4] = mac[1];
	macBuff[5] = mac[0];
	frame->AddMACAddr(frameOfst, name, macBuff, !randomAddr);
}

void Net::PacketAnalyzerBluetooth::AddScanType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 scanType)
{
	Text::CString vName = nullptr;
	switch (scanType)
	{
	case 1:
		vName = CSTR("Active");
		break;
	}
	frame->AddHex8Name(frameOfst, CSTR("Scan Type"), scanType, vName);
}

void Net::PacketAnalyzerBluetooth::AddScanFilterPolicy(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 scanType)
{
	Text::CString vName = nullptr;
	switch (scanType)
	{
	case 0:
		vName = CSTR("Accept all advertisements, except directed advertisements not addressed to this device");
		break;
	}
	frame->AddHex8Name(frameOfst, CSTR("Scan Filter Policy"), scanType, vName);
}

void Net::PacketAnalyzerBluetooth::AddScanInterval(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 scanInt)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendU16(scanInt);
	sb.AppendC(UTF8STRC(" ("));
	sb.AppendDouble(0.625 * scanInt);
	sb.AppendC(UTF8STRC("ms)"));
	frame->AddField(frameOfst, 2, CSTR("Scan Interval"), sb.ToCString());
}

void Net::PacketAnalyzerBluetooth::AddScanWindow(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 scanWind)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendU16(scanWind);
	sb.AppendC(UTF8STRC(" ("));
	sb.AppendDouble(0.625 * scanWind);
	sb.AppendC(UTF8STRC("ms)"));
	frame->AddField(frameOfst, 2, CSTR("Scan Window"), sb.ToCString());
}

void Net::PacketAnalyzerBluetooth::AddAddrType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Text::CStringNN name, UInt8 addrType)
{
	Text::CString vName = nullptr;
	switch (addrType)
	{
	case 0:
		vName = CSTR("Public Device Address");
		break;
	case 1:
		vName = CSTR("Random Address Type");
		break;
	}
	frame->AddHex8Name(frameOfst, name, addrType, vName);
}

void Net::PacketAnalyzerBluetooth::AddHCIEvent(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 evt)
{
	frame->AddHex8Name(frameOfst, CSTR("Event Code"), evt, HCIEventGetName(evt));
}

void Net::PacketAnalyzerBluetooth::AddLESubEvent(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 subEvt)
{
	frame->AddHex8Name(frameOfst, CSTR("Sub Event"), subEvt, LESubEventGetName(subEvt));
}

void Net::PacketAnalyzerBluetooth::AddAdvEvtType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 evtType)
{
	Text::CString vName = nullptr;
	switch (evtType)
	{
	case 0:
		vName = CSTR("Connectable Undirected Advertising");
		break;
	case 3:
		vName = CSTR("Non-Connectable Undirected Advertising");
		break;
	case 4:
		vName = CSTR("Scan Response");
		break;
	}
	frame->AddHex8Name(frameOfst, CSTR("Event Type"), evtType, vName);
}

void Net::PacketAnalyzerBluetooth::AddRSSI(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Int8 rssi)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(Text::StrInt16(sbuff, rssi), UTF8STRC("dBm"));
	frame->AddField(frameOfst, 1, CSTR("RSSI"), CSTRP(sbuff, sptr));
}

void Net::PacketAnalyzerBluetooth::AddAdvData(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UnsafeArray<const UInt8> packet, UIntOS packetSize)
{
	Text::CString vName;
	Text::StringBuilderUTF8 sb;
	UInt16 u16;
	UInt8 len;
	UInt32 i = 0;
	UInt32 j;
	while (i < packetSize)
	{
		len = packet[i];
		if (len + i + 1 > packetSize)
		{
			AddUnknown(frame, frameOfst + i, packet + i, packetSize - i);
			return;
		}
		if (len == 0)
		{
			frame->AddField(frameOfst + i, (UInt32)packetSize - i, CSTR("Unused"), nullptr);
			return;
		}
		frame->AddUInt(frameOfst + i, 1, CSTR("Adv Item Len"), len);
		frame->AddHex8(frameOfst + i + 1, CSTR("Adv Item Type"), packet[i + 1]);
		if (packet[i + 1] == 1 && len == 2)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("0x"));
			sb.AppendHex8(packet[i + 2]);
			Bool found = false;
			if (packet[i + 2] & 1)
			{
				sb.AppendC(UTF8STRC(" LE Limited Discoverable Mode"));
				found = true;
			}
			if (packet[i + 2] & 2)
			{
				if (found) sb.AppendUTF8Char(',');
				sb.AppendC(UTF8STRC(" LE General Discoverable Mode"));
				found = true;
			}
			if (packet[i + 2] & 4)
			{
				if (found) sb.AppendUTF8Char(',');
				sb.AppendC(UTF8STRC(" BR/EDR Not Supported"));
				found = true;
			}
			if (packet[i + 2] & 8)
			{
				if (found) sb.AppendUTF8Char(',');
				sb.AppendC(UTF8STRC(" Simultaneous LE and BR/EDR to Same Device Capable (Controller)"));
				found = true;
			}
			if (packet[i + 2] & 16)
			{
				if (found) sb.AppendUTF8Char(',');
				sb.AppendC(UTF8STRC(" Simultaneous LE and BR/EDR to Same Device Capable (Host)"));
				found = true;
			}
			frame->AddField(frameOfst + i + 2, 1, CSTR("Flags"), sb.ToCString());
		}
		else if (packet[i + 1] == 2 && len == 3)
		{
			u16 = ReadLUInt16(&packet[i + 2]);
			vName = nullptr;
			switch (u16)
			{
			case 0xFE24:
				vName = CSTR("August Home Inc");
				break;
			}
			frame->AddHex16Name(frameOfst + i + 2, CSTR("16-bit Service Class UUIDs (incomplete)"), u16, vName);
		}
		else if (packet[i + 1] == 3 && len >= 3 && len & 1)
		{
			j = 2;
			while (j < len)
			{
				switch (ReadLUInt16(&packet[i + j]))
				{
				case 0x0001:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("SDP"));
					break;
				case 0x0003:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("RFCOMM"));
					break;
				case 0x0100:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("L2CAP"));
					break;
				case 0x1101:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("Serial Port"));
					break;
				case 0x111F:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("Handsfree Audio Gateway"));
					break;
				case 0x1200:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("PnP Information"));
					break;
				case 0x1203:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("Generic Audio"));
					break;
				case 0xFE8F:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("CSR"));
					break;
				default:
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Unknown(0x"));
					sb.AppendHex16(ReadLUInt16(&packet[i + j]));
					sb.AppendUTF8Char(')');
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), sb.ToCString());
					break;
				}
				j += 2;
			}
		}
		else if (packet[i + 1] == 6 && len == 17)
		{
			sb.ClearStr();
			sb.AppendHex32(ReadLUInt32(&packet[i + 14]));
			sb.AppendUTF8Char('-');
			sb.AppendHex16(ReadLUInt16(&packet[i + 12]));
			sb.AppendUTF8Char('-');
			sb.AppendHex16(ReadLUInt16(&packet[i + 10]));
			sb.AppendUTF8Char('-');
			sb.AppendHex16(ReadLUInt16(&packet[i + 8]));
			sb.AppendUTF8Char('-');
			sb.AppendHex32(ReadLUInt32(&packet[i + 4]));
			sb.AppendHex16(ReadLUInt16(&packet[i + 2]));
			frame->AddField(frameOfst + i + 2, 16, CSTR("128-bit Service Class UUIDs (incomplete)"), sb.ToCString());
		}
		else if (packet[i + 1] == 8)
		{
			sb.ClearStr();
			sb.AppendC(&packet[i + 2], (UIntOS)len - 1);
			frame->AddField(frameOfst + i + 2, (UIntOS)len - 1, CSTR("Device Name (shortened)"), sb.ToCString());
		}
		else if (packet[i + 1] == 9)
		{
			sb.ClearStr();
			sb.AppendC(&packet[i + 2], (UIntOS)len - 1);
			frame->AddField(frameOfst + i + 2, (UIntOS)len - 1, CSTR("Device Name"), sb.ToCString());
		}
		else if (packet[i + 1] == 0x0A && len == 2)
		{
			sb.ClearStr();
			sb.AppendU16(packet[i + 2]);
			sb.AppendC(UTF8STRC("dBm"));
			frame->AddField(frameOfst + i + 2, 1, CSTR("Tx Power Level"), sb.ToCString());
		}
		else if (packet[i + 1] == 0xff && len >= 3)
		{
			UInt16 compId = ReadLUInt16(&packet[i + 2]);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("0x"));
			sb.AppendHex16(compId);
			Text::CStringNN cstr;
			if (IO::BTUtil::CompanyGetName(compId).SetTo(cstr))
			{
				sb.AppendC(UTF8STRC(" ("));
				sb.Append(cstr);
				sb.AppendUTF8Char(')');
			}
			else
			{
				sb.AppendC(UTF8STRC(" (Unknown)"));
			}
			if (compId == 0x4C)
			{
				/////////////////// https://github.com/furiousMAC/continuity/tree/master/messages
				frame->AddField(frameOfst + i + 2, 2, CSTR("Manufacturer Specific"), sb.ToCString());
				j = 4;
				while (j < (UInt32)len - 1)
				{
					vName = nullptr;
					switch (packet[i + j])
					{
					case 2:
						vName = CSTR("Proximity Beacons");
						break;
					case 3:
						vName = CSTR("AirPrint Message");
						break;
					case 5:
						vName = CSTR("AirDrop Message");
						break;
					case 6:
						vName = CSTR("HomeKit Message");
						break;
					case 7:
						vName = CSTR("Proximity Pairing Message");
						break;
					case 8:
						vName = CSTR("Hey Siri Message");
						break;
					case 9:
						vName = CSTR("Airplay Target Message");
						break;
					case 10:
						vName = CSTR("Airplay Source Message");
						break;
					case 11:
						vName = CSTR("Magic Switch Message");
						break;
					case 12:
						vName = CSTR("Handoff Message");
						break;
					case 13:
						vName = CSTR("Tethering Target Message");
						break;
					case 14:
						vName = CSTR("Tethering Source Message");
						break;
					case 15:
						vName = CSTR("Nearby Action");
						break;
					case 16:
						vName = CSTR("Nearby Info");
						break;
					case 18:
						vName = CSTR("Find My Message");
						break;
					}
					frame->AddHex8Name(frameOfst + i + j, CSTR("Apple Type"), packet[i + j], vName);
					frame->AddUInt(frameOfst + i + j + 1, 1, CSTR("Apple Length"), packet[i + j + 1]);
					UInt8 appLen = packet[i + j + 1] & 0x7F;
					if (appLen + j + 1 <= len)
					{
						if (packet[i + j] == 2 && appLen == 21)
						{
							sb.ClearStr();
							sb.AppendHex32(ReadLUInt32(&packet[i + j + 14]));
							sb.AppendUTF8Char('-');
							sb.AppendHex16(ReadLUInt16(&packet[i + j + 12]));
							sb.AppendUTF8Char('-');
							sb.AppendHex16(ReadLUInt16(&packet[i + j + 10]));
							sb.AppendUTF8Char('-');
							sb.AppendHex16(ReadLUInt16(&packet[i + j + 8]));
							sb.AppendUTF8Char('-');
							sb.AppendHex32(ReadLUInt32(&packet[i + j + 4]));
							sb.AppendHex16(ReadLUInt16(&packet[i + j + 2]));
							frame->AddField(frameOfst + i + j + 2, 16, CSTR("Proximity UUID"), sb.ToCString());
							frame->AddHex16(frameOfst + i + j + 18, CSTR("Major"), ReadLUInt16(&packet[i + j + 18]));
							frame->AddHex16(frameOfst + i + j + 20, CSTR("Minor"), ReadLUInt16(&packet[i + j + 20]));
							frame->AddInt(frameOfst + i + j + 22, 1, CSTR("Measured Power"), (Int8)packet[i + j + 22]);
						}
						else if (packet[i + j] == 5 && appLen == 18)
						{
							frame->AddHexBuff(frameOfst + i + j + 2, 8, CSTR("Zeros"), &packet[i + j + 2], false);
							frame->AddUInt(frameOfst + i + j + 10, 1, CSTR("AirDrop Version"), packet[i + j + 10]);
							frame->AddHex16(frameOfst + i + j + 11, CSTR("Short Hash of Apple ID"), ReadMUInt16(&packet[i + j + 11]));
							frame->AddHex16(frameOfst + i + j + 13, CSTR("Short Hash of Phone Number"), ReadMUInt16(&packet[i + j + 13]));
							frame->AddHex16(frameOfst + i + j + 15, CSTR("Short Hash of Email"), ReadMUInt16(&packet[i + j + 15]));
							frame->AddHex16(frameOfst + i + j + 17, CSTR("Short Hash of Email2"), ReadMUInt16(&packet[i + j + 17]));
							frame->AddHexBuff(frameOfst + i + j + 19, 1, CSTR("Zero"), &packet[i + j + 19], false);
						}
						else if (packet[i + j] == 6 && appLen == 13)
						{
							frame->AddHex8(frameOfst + i + j + 2, CSTR("Status"), packet[i + j + 2]);
							frame->AddMACAddr(frameOfst + i + j + 3, CSTR("Device ID"), &packet[i + j + 3], false);
							frame->AddUInt(frameOfst + i + j + 9, 2, CSTR("Category"), ReadLUInt16(&packet[i + j + 9]));
							frame->AddUInt(frameOfst + i + j + 11, 2, CSTR("Global State Number"), ReadLUInt16(&packet[i + j + 11]));
							frame->AddUInt(frameOfst + i + j + 13, 1, CSTR("Configuration Number"), packet[i + j + 13]);
							frame->AddUInt(frameOfst + i + j + 14, 1, CSTR("Compatible Version"), packet[i + j + 14]);
						}
						else if (packet[i + j] == 7 && appLen >= 9)
						{
							frame->AddHex8(frameOfst + i + j + 2, CSTR("Unknown"), packet[i + j + 2]);
							frame->AddHex16(frameOfst + i + j + 3, CSTR("Device Model"), ReadLUInt16(&packet[i + j + 3]));
							frame->AddHex8(frameOfst + i + j + 5, CSTR("Status"), packet[i + j + 5]);
							frame->AddHex16(frameOfst + i + j + 6, CSTR("Battery Levels"), ReadLUInt16(&packet[i + j + 6]));
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 6] & 15) * 10);
							frame->AddSubfield(frameOfst + i + j + 6, 2, CSTR("Left Battery Level"), sb.ToCString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 6] >> 4) * 10);
							frame->AddSubfield(frameOfst + i + j + 6, 2, CSTR("Right Battery Level"), sb.ToCString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 7] & 15) * 10);
							frame->AddSubfield(frameOfst + i + j + 6, 2, CSTR("Case Battery Level"), sb.ToCString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 7] & 0x10) >> 4);
							frame->AddSubfield(frameOfst + i + j + 6, 2, CSTR("Left Charging"), sb.ToCString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 7] & 0x20) >> 5);
							frame->AddSubfield(frameOfst + i + j + 6, 2, CSTR("Right Charging"), sb.ToCString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 7] & 0x40) >> 6);
							frame->AddSubfield(frameOfst + i + j + 6, 2, CSTR("Case Charging"), sb.ToCString());
							frame->AddUInt(frameOfst + i + j + 8, 1, CSTR("Open Count"), packet[i + j + 8]);
							frame->AddHex8(frameOfst + i + j + 9, CSTR("Device Color"), packet[i + j + 9]);
							frame->AddHex8(frameOfst + i + j + 10, CSTR("Unknown"), packet[i + j + 10]);
							if (appLen > 9)
							{
								frame->AddHexBuff(frameOfst + i + j + 11, (UIntOS)appLen - 9, CSTR("Encrypted Data"), &packet[i + j + 11], false);
							}
						}
						else if (packet[i + j] == 9 && appLen == 6)
						{
							frame->AddHex8(frameOfst + i + j + 2, CSTR("Flags"), packet[i + j + 2]);
							frame->AddHex8(frameOfst + i + j + 3, CSTR("Seed"), packet[i + j + 3]);
							frame->AddIPv4(frameOfst + i + j + 4, CSTR("AirPlay IPv4 Address"), &packet[i + j + 4]);
						}
						else if (packet[i + j] == 10 && appLen == 1)
						{
							frame->AddHex8(frameOfst + i + j + 2, CSTR("Data"), packet[i + j + 2]);
						}
						else if (packet[i + j] == 12 && appLen== 14)
						{
							frame->AddBool(frameOfst + i + j + 2, CSTR("Copy/Cut Performed"), packet[i + j + 2]);
							frame->AddUInt(frameOfst + i + j + 3, 2, CSTR("Sequence Number"), ReadLUInt16(&packet[i + j + 3]));
							frame->AddHex8(frameOfst + i + j + 5, CSTR("AES-GCM Auth Tag"), packet[i + j + 5]);
							frame->AddHexBuff(frameOfst + i + j + 6, 10, CSTR("Encrypted Handoff Data"), &packet[i + j + 6], false);
						}
						else if (packet[i + j] == 15 && appLen >= 2)
						{
							frame->AddHex8(frameOfst + i + j + 2, CSTR("Flags"), packet[i + j + 2]);
							vName = nullptr;
							switch (packet[i + j + 3])
							{
							case 1:
								vName = CSTR("Apple TV Setup");
								break;
							case 4:
								vName = CSTR("Mobile Backup");
								break;
							case 0x05:
								vName = CSTR("Watch Setup");
								break;
							case 0x06:
								vName = CSTR("Apple TV Pair");
								break;
							case 0x07:
								vName = CSTR("Internet Relay");
								break;
							case 0x08:
								vName = CSTR("WiFi Password");
								break;
							case 0x09:
								vName = CSTR("iOS Setup");
								break;
							case 0x0A:
								vName = CSTR("Repair");
								break;
							case 0x0B:
								vName = CSTR("Speaker Setupd");
								break;
							case 0x0C:
								vName = CSTR("Apple Pay");
								break;
							case 0x0D:
								vName = CSTR("Whole Home Audio Setup");
								break;
							case 0x0E:
								vName = CSTR("Developer Tools Pairing Request");
								break;
							case 0x0F:
								vName = CSTR("Answered Call");
								break;
							case 0x10:
								vName = CSTR("Ended Call");
								break;
							case 0x11:
								vName = CSTR("DD Ping");
								break;
							case 0x12:
								vName = CSTR("DD Pong");
								break;
							case 0x13:
								vName = CSTR("Remote Auto Fill");
								break;
							case 0x14:
								vName = CSTR("Companion Link Proximity");
								break;
							case 0x15:
								vName = CSTR("Remote Management");
								break;
							case 0x16:
								vName = CSTR("Remote Auto Fill Pong");
								break;
							case 0x17:
								vName = CSTR("Remote Display");
								break;
							}
							frame->AddUIntName(frameOfst + i + j + 3, 1, CSTR("Action Type"), packet[i + j + 3], vName);
							if (appLen >= 5)
							{
								frame->AddHexBuff(frameOfst + i + j + 4, 3, CSTR("Authentication Tag"), &packet[i + j + 4], false);
							}
						}
						else if (packet[i + j] == 16 && appLen >= 2)
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("0x"));
							sb.AppendHex8(packet[i + j + 2]);
							switch (packet[i + j + 2] & 15)
							{
							case 0:
								sb.AppendC(UTF8STRC(" (Activity Level Unknown)"));
								break;
							case 1:
								sb.AppendC(UTF8STRC(" (Activity Reporting Disabled )"));
								break;
							case 3:
								sb.AppendC(UTF8STRC(" (Idle User)"));
								break;
							case 5:
								sb.AppendC(UTF8STRC(" (Audio playing while screen locked)"));
								break;
							case 7:
								sb.AppendC(UTF8STRC(" (Active user (screen on) )"));
								break;
							case 9:
								sb.AppendC(UTF8STRC(" (Screen on with video playing)"));
								break;
							case 10:
								sb.AppendC(UTF8STRC(" (Watch on wrist and unlocked)"));
								break;
							case 11:
								sb.AppendC(UTF8STRC(" (Recent user interaction)"));
								break;
							case 13:
								sb.AppendC(UTF8STRC(" (User is driving a vehicle)"));
								break;
							case 14:
								sb.AppendC(UTF8STRC(" (Phone or Facetime Call)"));
								break;
							default:
								sb.AppendC(UTF8STRC(" (Unknown)"));
								break;
							}
							if (packet[i + j + 2] & 0x10)
							{
								sb.AppendC(UTF8STRC(", primary iCloud account device"));
							}
							if (packet[i + j + 2] & 0x20)
							{
								sb.AppendC(UTF8STRC(", Unknown"));
							}
							if (packet[i + j + 2] & 0x40)
							{
								sb.AppendC(UTF8STRC(", AirDrop Receiving is enabled"));
							}
							if (packet[i + j + 2] & 0x80)
							{
								sb.AppendC(UTF8STRC(", Unknown"));
							}
							frame->AddField(frameOfst + i + j + 2, 1, CSTR("Action"), sb.ToCString());
							frame->AddHex8(frameOfst + i + j + 3, CSTR("Status"), packet[i + j + 3]);
							if (packet[i + j + 3] & 2)
							{
								if (appLen >= 6)
								{
									frame->AddHexBuff(frameOfst + i + j + 4, 4, CSTR("Authentication Tag"), &packet[i + j + 4], false);
								}
								else if (appLen > 2)
								{
									frame->AddHexBuff(frameOfst + i + j + 4, (UIntOS)appLen - 2, CSTR("Authentication Tag"), &packet[i + j + 4], false);
								}
								if (appLen >= 7)
								{
									frame->AddHex8(frameOfst + i + j + 8, CSTR("Post Auth Tag Data"), packet[i + j + 8]);
								}
							}
							else
							{
								if (appLen >= 5)
								{
									frame->AddHexBuff(frameOfst + i + j + 4, 3, CSTR("Authentication Tag"), &packet[i + j + 4], false);
								}
								else if (appLen > 2)
								{
									frame->AddHexBuff(frameOfst + i + j + 4, (UIntOS)appLen - 2, CSTR("Authentication Tag"), &packet[i + j + 4], false);
								}
								if (appLen >= 6)
								{
									frame->AddHex8(frameOfst + i + j + 7, CSTR("Post Auth Tag Data"), packet[i + j + 7]);
								}
							}
						}
						else if (packet[i + j] == 18 && appLen >= 25)
						{
							frame->AddHex8(frameOfst + i + j + 2, CSTR("Status"), packet[i + j + 2]);
							frame->AddHexBuff(frameOfst + i + j + 3, 22, CSTR("Byte 6-27 of Public Key"), &packet[i + j + 3], false);
							frame->AddHex8(frameOfst + i + j + 25, CSTR("Byte 0, Bit 6-7 of public key"), packet[i + j + 25]);
							frame->AddHex8(frameOfst + i + j + 26, CSTR("Hint (Byte 5 of BD_ADDR)"), packet[i + j + 26]);
						}
						else if (packet[i + j] == 19)
						{
							frame->AddHexBuff(frameOfst + i + j + 2, appLen, CSTR("Unknown"), &packet[i + j + 2], false);
						}
					}
					j += (UInt32)appLen + 2;
				}
			}
			else
			{
				sb.AppendC(UTF8STRC(", Value="));
				sb.AppendHexBuff(&packet[i + 4], (UIntOS)len - 3, ' ', Text::LineBreakType::None);
				frame->AddField(frameOfst + i + 2, (UIntOS)len - 1, CSTR("Manufacturer Specific"), sb.ToCString());
			}
		}
		else
		{
			sb.ClearStr();
			sb.AppendHexBuff(packet + i + 2, (UIntOS)len - 1, ' ', Text::LineBreakType::None);
			frame->AddField(frameOfst + i + 2, (UIntOS)len - 1, CSTR("Adv Item Value"), sb.ToCString());
		}
		i += (UInt32)len + 1;
	}
}

void Net::PacketAnalyzerBluetooth::AddLAP(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt32 lap)
{
	frame->AddHex24(frameOfst, CSTR("LAP"), lap);
}

void Net::PacketAnalyzerBluetooth::AddInquiryLen(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 len)
{
	Text::CString vName = nullptr;
	switch (len)
	{
	case 4:
		vName = CSTR("5.12 sec");
		break;
	}
	frame->AddUIntName(frameOfst, 1, CSTR("Inquiry Length"), len, vName);
}

void Net::PacketAnalyzerBluetooth::AddPageScanMode(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 mode)
{
	Text::CString vName = nullptr;
	switch (mode)
	{
	case 1:
		vName = CSTR("R1");
		break;
	}
	frame->AddHex8Name(frameOfst, CSTR("Page Scan Repetition Mode"), mode, vName);
}

void Net::PacketAnalyzerBluetooth::AddClassOfDevice(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt32 cls)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("0x"));
	sb.AppendHex24(cls);
	sb.AppendC(UTF8STRC(", Class="));
	switch (cls & 31)
	{
	case 2:
		sb.AppendC(UTF8STRC("Phone"));
		break;
	default:
		sb.AppendU16(cls & 31);
		sb.AppendC(UTF8STRC(" (Unknown)"));
		break;
	}
	if (cls & 0x20)
	{
		sb.AppendC(UTF8STRC(", Limited Discoverable Mode"));
	}
	if (cls & 0x100)
	{
		sb.AppendC(UTF8STRC(", Positioning"));
	}
	if (cls & 0x200)
	{
		sb.AppendC(UTF8STRC(", Networking"));
	}
	if (cls & 0x400)
	{
		sb.AppendC(UTF8STRC(", Rendering"));
	}
	if (cls & 0x800)
	{
		sb.AppendC(UTF8STRC(", Capturing"));
	}
	if (cls & 0x1000)
	{
		sb.AppendC(UTF8STRC(", Object Transfer"));
	}
	if (cls & 0x2000)
	{
		sb.AppendC(UTF8STRC(", Audio"));
	}
	if (cls & 0x4000)
	{
		sb.AppendC(UTF8STRC(", Telephony"));
	}
	if (cls & 0x8000)
	{
		sb.AppendC(UTF8STRC(", Information"));
	}
	sb.AppendC(UTF8STRC(", Format="));
	sb.AppendU32((cls >> 16) & 3);
	sb.AppendC(UTF8STRC(", Minor Class="));
	sb.AppendU32((cls >> 18) & 0x3F);
	frame->AddField(frameOfst, 3, CSTR("Class Of Device"), sb.ToCString());
}

void Net::PacketAnalyzerBluetooth::AddClockOffset(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 ofst)
{
	frame->AddHex16(frameOfst, CSTR("Clock Offset"), ofst);
}

void Net::PacketAnalyzerBluetooth::AddExAdvEvtType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 evtType)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("0x"));
	sb.AppendHex16(evtType);
	if (evtType & 1)
	{
		sb.AppendC(UTF8STRC(", Connectable"));
	}
	if (evtType & 2)
	{
		sb.AppendC(UTF8STRC(", Scannable"));
	}
	if (evtType & 4)
	{
		sb.AppendC(UTF8STRC(", Directed"));
	}
	if (evtType & 8)
	{
		sb.AppendC(UTF8STRC(", Scan Response"));
	}
	if (evtType & 16)
	{
		sb.AppendC(UTF8STRC(", Legacy"));
	}
	sb.AppendC(UTF8STRC(", Data Status="));
	sb.AppendUIntOS((((UIntOS)evtType) >> 5) & 3);
	switch ((((UIntOS)evtType) >> 5) & 3)
	{
	case 0:
		sb.AppendC(UTF8STRC(" (Complete)"));
		break;
	default:
		sb.AppendC(UTF8STRC(" (Unknown)"));
		break;
	}
	frame->AddField(frameOfst, 2, CSTR("Event Type"), sb.ToCString());
}

void Net::PacketAnalyzerBluetooth::AddPHYType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Text::CStringNN name, UInt8 phyType)
{
	Text::CString vName = nullptr;
	switch (phyType)
	{
	case 0:
		vName = CSTR("No packets");
		break;
	case 1:
		vName = CSTR("LE 1M");
		break;
	}
	frame->AddHex8Name(frameOfst, name, phyType, vName);
}

void Net::PacketAnalyzerBluetooth::AddAdvSID(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 sid)
{
	Text::CString vName = nullptr;
	switch (sid)
	{
	case 0xFF:
		vName = CSTR("Not available");
		break;
	}
	frame->AddHex8Name(frameOfst, CSTR("Advertising SID"), sid, vName);
}

void Net::PacketAnalyzerBluetooth::AddTxPower(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Int8 txPower)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(Text::StrInt16(sbuff, txPower), UTF8STRC("dBm"));
	frame->AddField(frameOfst, 1, CSTR("TxPower"), CSTRP(sbuff, sptr));
}

void Net::PacketAnalyzerBluetooth::AddPeriodicAdv(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 interval)
{
	Text::CString vName = nullptr;
	switch (interval)
	{
	case 0:
		vName = CSTR("No periodic advertising");
		break;
	}
	frame->AddHex16Name(frameOfst, CSTR("Periodic Advertising Interval"), interval, vName);
}

void Net::PacketAnalyzerBluetooth::AddUnknown(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UnsafeArray<const UInt8> packet, UIntOS packetSize)
{
	frame->AddHexBuff(frameOfst, (UInt32)packetSize, CSTR("Unknown"), packet, true);
}

Bool Net::PacketAnalyzerBluetooth::PacketGetName(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb)
{
	UInt8 mac[6];
	Text::CStringNN name;
	switch (packet[4])
	{
	case 1:
		if (CmdGetName(ReadLUInt16(&packet[5])).SetTo(name))
		{
			sb->Append(name);
		}
		else
		{
			sb->AppendC(UTF8STRC("Bluetooth"));
		}
		return true;
	case 2:
		sb->AppendC(UTF8STRC("HCI ACL Packet"));
		return true;
	case 4:
		switch (packet[5])
		{
		case 1:
			if (packet[7] == 0)
			{
				sb->AppendC(UTF8STRC("HCI Inquiry Success"));
			}
			else
			{
				sb->AppendC(UTF8STRC("HCI Inquiry Complete"));
			}
			return true;
		case 0x0E:
			if (CmdGetName(ReadLUInt16(&packet[8])).SetTo(name))
			{
				sb->Append(name);
				sb->AppendC(UTF8STRC(" Accept"));
			}
			else
			{
				sb->AppendC(UTF8STRC("Bluetooth"));
			}
			return true;
		case 0x0F:
			if (CmdGetName(ReadLUInt16(&packet[9])).SetTo(name))
			{
				if (packet[7] == 0)
				{
					sb->Append(name);
					sb->AppendC(UTF8STRC(" Pending"));
				}
				else
				{
					sb->AppendC(UTF8STRC("Bluetooth"));
				}
			}
			else
			{
				sb->AppendC(UTF8STRC("Bluetooth"));
			}
			return true;
		case 0x2F:
			sb->AppendC(UTF8STRC("HCI Result: "));
			mac[0] = packet[13];
			mac[1] = packet[12];
			mac[2] = packet[11];
			mac[3] = packet[10];
			mac[4] = packet[9];
			mac[5] = packet[8];
			sb->AppendHexBuff(mac, 6, ':', Text::LineBreakType::None);
			return true;
		case 0x3E:
			switch (packet[7])
			{
			case 0x2:
				sb->AppendC(UTF8STRC("LE Adv: "));
				mac[0] = packet[16];
				mac[1] = packet[15];
				mac[2] = packet[14];
				mac[3] = packet[13];
				mac[4] = packet[12];
				mac[5] = packet[11];
				sb->AppendHexBuff(mac, 6, ':', Text::LineBreakType::None);
				return true;
			case 0xd:
				sb->AppendC(UTF8STRC("LE AdvEx: "));
				mac[0] = packet[17];
				mac[1] = packet[16];
				mac[2] = packet[15];
				mac[3] = packet[14];
				mac[4] = packet[13];
				mac[5] = packet[12];
				sb->AppendHexBuff(mac, 6, ':', Text::LineBreakType::None);
				return true;
			default:
				sb->AppendC(UTF8STRC("Bluetooth"));
				return true;
			}
		default:
			sb->AppendC(UTF8STRC("Bluetooth"));
			return true;
		}
	default:
		sb->AppendC(UTF8STRC("Bluetooth"));
		return true;
	}
}

void Net::PacketAnalyzerBluetooth::PacketGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketGetDetail(packet, packetSize, 0, frame);
}

void Net::PacketAnalyzerBluetooth::PacketGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame)
{
	Text::CString vName;
	UInt32 i;
	UInt32 dir = ReadMUInt32(&packet[0]);
	AddDirection(frame, frameOfst + 0, dir);
	AddHCIPacketType(frame, frameOfst + 4, packet[4]);
	switch (packet[4])
	{
	case 1:
		{
			UInt16 cmd = ReadLUInt16(&packet[5]);
			UInt8 cmdLen = packet[7];
			AddCmdOpcode(frame, frameOfst + 5, cmd);
			AddParamLen(frame, frameOfst + 7, cmdLen);
			if (8 + (UIntOS)cmdLen <= packetSize)
			{
				switch (cmd)
				{
				case 0x0401:
					if (cmdLen == 5)
					{
						AddLAP(frame, frameOfst + 8, ReadLUInt24(&packet[8]));
						AddInquiryLen(frame, frameOfst + 11, packet[11]);
						frame->AddUInt(frameOfst + 12, 1, CSTR("Num Response"), packet[12]);
					}
					else
					{
						AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					}
					break;
				case 0x2005:
					if (cmdLen == 6)
					{
						AddBDAddr(frame, frameOfst + 8, CSTR("BD_ADDR"), packet + 8, true);
					}
					else
					{
						AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					}
					break;
				case 0x200B:
					if (cmdLen == 7)
					{
						AddScanType(frame, frameOfst + 8, packet[8]);
						AddScanInterval(frame, frameOfst + 9, ReadLUInt16(&packet[9]));
						AddScanWindow(frame, frameOfst + 11, ReadLUInt16(&packet[11]));
						AddAddrType(frame, frameOfst + 13, CSTR("Own Address Type"), packet[13]);
						AddScanFilterPolicy(frame, frameOfst + 14, packet[14]);
					}
					else
					{
						AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					}
					break;
				case 0x200C:
					if (cmdLen == 2)
					{
						frame->AddBool(frameOfst + 8, CSTR("Scan Enable"), packet[8]);
						frame->AddBool(frameOfst + 9, CSTR("Filter Duplicates"), packet[9]);
					}
					else
					{
						AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					}
					break;
				case 0x2041:
					if (cmdLen == 8)
					{
						AddAddrType(frame, frameOfst + 8, CSTR("Own Address Type"), packet[8]);
						AddScanFilterPolicy(frame, frameOfst + 9, packet[9]);
						AddPHYType(frame, frameOfst + 10, CSTR("Scanning PHYs"), packet[10]);
						AddScanType(frame, frameOfst + 11, packet[11]);
						AddScanInterval(frame, frameOfst + 12, ReadLUInt16(&packet[12]));
						AddScanWindow(frame, frameOfst + 14, ReadLUInt16(&packet[14]));
					}
					else
					{
						AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					}
					break;
				default:
					AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					break;
				}
				if (8 + (UIntOS)cmdLen < packetSize)
				{
					AddUnknown(frame, frameOfst + 8 + cmdLen, packet + 8 + cmdLen, packetSize - 8 - cmdLen);
				}
			}
		}
		break;
	case 2:
		frame->AddHex16(frameOfst + 5, CSTR("Connection Handle"), ReadLUInt16(&packet[5]));
		frame->AddUInt(frameOfst + 7, 2, CSTR("Data Total Length"), ReadLUInt16(&packet[7]));
		AddUnknown(frame, frameOfst + 9, packet + 9, packetSize - 9);
		break;
	case 4:
		AddHCIEvent(frame, frameOfst + 5, packet[5]);
		AddParamLen(frame, frameOfst + 6, packet[6]);
		switch (packet[5])
		{
		case 0x01: //Inquiry Complete
			vName = nullptr;
			switch (packet[7])
			{
			case 0:
				vName = CSTR("Success");
				break;
			}
			frame->AddHex8Name(frameOfst + 7, CSTR("Status"), packet[7], vName);
			if (packetSize > 8)
			{
				AddUnknown(frame, frameOfst + 8, packet + 8, packetSize + 8);
			}
			break;
		case 0x0E: //Command Complete
			frame->AddUInt(frameOfst + 7, 1, CSTR("Number of Allowed Command Packets"), packet[7]);
			AddCmdOpcode(frame, frameOfst + 8, ReadLUInt16(&packet[8]));
			vName = nullptr;
			switch (packet[10])
			{
			case 0:
				vName = CSTR("Success");
				break;
			}
			frame->AddHex8Name(frameOfst + 10, CSTR("Status"), packet[10], vName);
			if (packetSize > 11)
			{
				AddUnknown(frame, frameOfst + 11, packet + 11, packetSize - 11);
			}
			break;
		case 0x0F: //Command Status
			vName = nullptr;
			switch (packet[7])
			{
			case 0:
				vName = CSTR("Pending");
				break;
			}
			frame->AddHex8Name(frameOfst + 7, CSTR("Status"), packet[7], vName);
			frame->AddUInt(frameOfst + 8, 1, CSTR("Number of Allowed Command Packets"), packet[8]);
			AddCmdOpcode(frame, frameOfst + 9, ReadLUInt16(&packet[9]));
			break;
		case 0x2F: //Extended Inquery Result
			frame->AddUInt(frameOfst + 7, 1, CSTR("Number of responses"), packet[7]);
			AddBDAddr(frame, frameOfst + 8, CSTR("BD_ADDR"), &packet[8], false);
			AddPageScanMode(frame, frameOfst + 14, packet[14]);
			frame->AddHex8(frameOfst + 15, CSTR("Reserved"), packet[15]);
			AddClassOfDevice(frame, frameOfst + 16, ReadLUInt24(&packet[16]));
			AddClockOffset(frame, frameOfst + 19, ReadLUInt16(&packet[19]));
			AddRSSI(frame, frameOfst + 21, (Int8)packet[21]);
			AddAdvData(frame, frameOfst + 22, &packet[22], packetSize - 22);
			break;
		case 0x3E: //LE Meta
			AddLESubEvent(frame, frameOfst + 7, packet[7]);
			switch (packet[7])
			{
			case 2:
				frame->AddUInt(frameOfst + 8, 1, CSTR("Num Reports"), packet[8]);
				if (packet[8] != 1 || packet[6] < 11)
				{
					AddUnknown(frame, frameOfst + 9, packet + 9, packetSize + 9);
					break;
				}
				AddAdvEvtType(frame, frameOfst + 9, packet[9]);
				AddAddrType(frame, frameOfst + 10, CSTR("Peer Address Type"), packet[10]);
				AddBDAddr(frame, frameOfst + 11, CSTR("BD_ADDR"), packet + 11, packet[10] == 1);
				frame->AddUInt(frameOfst + 17, 1, CSTR("Adv Data Length"), packet[17]);
				if (18 + (UIntOS)packet[17] < packetSize)
				{
					AddAdvData(frame, frameOfst + 18, packet + 18, packet[17]);
				}
				else if (packetSize > 18)
				{
					AddUnknown(frame, frameOfst + 18, packet + 18, packetSize - 18);
				}
				i = 18 + (UInt32)packet[17];
				if (i < packetSize)
				{
					AddRSSI(frame, frameOfst + i, (Int8)packet[i]);
					i++;
				}
				if (i < packetSize)
				{
					AddUnknown(frame, frameOfst + i, packet + i, packetSize - i);
				}
				break;
			case 0x0D: //LE Exteded Advertising Report
				frame->AddUInt(frameOfst + 8, 1, CSTR("Num Reports"), packet[8]);
				if (packet[8] != 1 || packet[6] < 26)
				{
					AddUnknown(frame, frameOfst + 9, packet + 9, packetSize + 9);
					break;
				}
				AddExAdvEvtType(frame, frameOfst + 9, ReadLUInt16(&packet[9]));
				AddAddrType(frame, frameOfst + 11, CSTR("Peer Address Type"), packet[11]);
				AddBDAddr(frame, frameOfst + 12, CSTR("BD_ADDR"), packet + 12, packet[11] == 1);
				AddPHYType(frame, frameOfst + 18, CSTR("Primary PHY"), packet[18]);
				AddPHYType(frame, frameOfst + 19, CSTR("Secondary PHY"), packet[19]);
				AddAdvSID(frame, frameOfst + 20, packet[20]);
				AddTxPower(frame, frameOfst + 21, (Int8)packet[21]);
				AddRSSI(frame, frameOfst + 22, (Int8)packet[22]);
				AddPeriodicAdv(frame, frameOfst + 23, ReadLUInt16(&packet[23]));
				AddAddrType(frame, frameOfst + 25, CSTR("Direct Address Type"), packet[25]);
				AddBDAddr(frame, frameOfst + 26, CSTR("Direct BD_ADDR"), packet + 26, packet[25] == 1);
				frame->AddUInt(frameOfst + 32, 1, CSTR("Adv Data Length"), packet[32]);
				if (33 + (UIntOS)packet[32] <= packetSize)
				{
					AddAdvData(frame, frameOfst + 33, packet + 33, packet[32]);
					if (33 + (UIntOS)packet[32] < packetSize)
					{
						AddUnknown(frame, frameOfst + 33 + (UInt32)packet[32], packet + 33 + (UIntOS)packet[32], packetSize - 33 - (UIntOS)packet[32]);
					}
				}
				else if (packetSize > 33)
				{
					AddUnknown(frame, frameOfst + 33, packet + 33, packetSize - 33);
				}
				break;
			default:
				AddUnknown(frame, frameOfst + 8, packet + 8, packetSize + 8);
				break;
			}
			break;
		default:
			AddUnknown(frame, frameOfst + 7, packet + 7, packetSize - 7);
			break;
		}
		break;
	default:
		AddUnknown(frame, frameOfst + 5, packet + 5, packetSize - 5);
		break;		
	}
}

Text::CString Net::PacketAnalyzerBluetooth::CmdGetName(UInt16 cmd)
{
	switch (cmd)
	{
	case 0x0401:
		return CSTR("HCI Inquiry");
	case 0x2005:
		return CSTR("LE Set Random Address");
	case 0x200B:
		return CSTR("LE Set Scan Parameter");
	case 0x200C:
		return CSTR("LE Set Scan Enable");
	default:
		return nullptr;
	}
}

Text::CString Net::PacketAnalyzerBluetooth::HCIPacketTypeGetName(UInt8 packetType)
{
	switch (packetType)
	{
	case 1:
		return CSTR("HCI Command");
	case 4:
		return CSTR("HCI Event");
	default:
		return nullptr;	
	}
}

Text::CString Net::PacketAnalyzerBluetooth::HCIEventGetName(UInt8 evt)
{
	switch (evt)
	{
	case 0x0E:
		return CSTR("Command Complete");
	case 0x0F:
		return CSTR("Command Status");
	case 0x2F:
		return CSTR("Extended Inquery Result");
	case 0x3E:
		return CSTR("LE Meta");
	default:
		return nullptr;
	}
}

Text::CString Net::PacketAnalyzerBluetooth::LESubEventGetName(UInt8 subEvt)
{
	switch (subEvt)
	{
	case 0x02:
		return CSTR("LE Advertising Report");
	case 0x0D:
		return CSTR("LE Extended Advertising Report");
	default:
		return nullptr;
	}
}
