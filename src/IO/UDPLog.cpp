#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/UDPLog.h"
#include "Net/SocketFactory.h"
#include "Text/Encoding.h"
#include "Text/MyStringFloat.h"

Bool IO::UDPLog::ParseLog(UInt8 *dataBuff, UOSInt buffSize, Text::StringBuilderUTF8 *sb, Bool detail)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;

	if (buffSize == 1 && dataBuff[0] == 0x48)
	{
		sb->AppendC(UTF8STRC("SP1000: Heartbeat"));
	}
	else if (buffSize >= 10 && dataBuff[0] == 0xff && dataBuff[buffSize - 1] == 0xff)
	{
		UInt32 cmdSize = (UInt32)(dataBuff[1] << 8) | dataBuff[2];
		Int64 termId = Data::ByteTool::GetBCD8(dataBuff[4]) * 100000000LL + Data::ByteTool::GetBCD32(&dataBuff[5]);
		if (cmdSize <= buffSize)
		{
			sb->AppendC(UTF8STRC("SP1000CS: "));
			sb->AppendI64(termId);
			switch (dataBuff[3])
			{
			case 1:
				sb->AppendC(UTF8STRC(", Register"));
				break;
			case 3:
			case 0x2a:
			case 0x87:
				if (dataBuff[3] == 3)
				{
					sb->AppendC(UTF8STRC(", Basic GPS Data"));
				}
				else if (dataBuff[3] == 0x2a)
				{
					sb->AppendC(UTF8STRC(", Blind zone GPS Data"));
				}
				else
				{
					sb->AppendC(UTF8STRC(", LBS Data"));
				}
				if (detail)
				{
					if (cmdSize == 31)
					{
						Data::DateTime t;
						sb->AppendC(UTF8STRC("\r\nGPS Time = "));
						t.SetValue((UInt16)(Data::ByteTool::GetBCD8(dataBuff[14]) + 2000), Data::ByteTool::GetBCD8(dataBuff[13]), Data::ByteTool::GetBCD8(dataBuff[12]), Data::ByteTool::GetBCD8(dataBuff[9]), Data::ByteTool::GetBCD8(dataBuff[10]), Data::ByteTool::GetBCD8(dataBuff[11]), 0, 0);
						sb->AppendDate(&t);

						Double lat;
						Double lon;
						lat = ReadMUInt32(&dataBuff[19]) / 100000.0;
						lon = ReadMUInt32(&dataBuff[15]) / 100000.0;
						Int32 latI = (Int32)(lat / 100);
						Int32 lonI = (Int32)(lon / 100);
						lat = latI + (lat - latI * 100) / 60;
						lon = lonI + (lon - lonI * 100) / 60;
						if (dataBuff[26] & 1)
						{
							lon = -lon;
						}
						if (dataBuff[26] & 2)
						{
							lat = -lat;
						}
						sb->AppendC(UTF8STRC("\r\nLatitude = "));
						Text::SBAppendF64(sb, lat);
						sb->AppendC(UTF8STRC("\r\nLongitude = "));
						Text::SBAppendF64(sb, lon);
						Int32 spdDir = Data::ByteTool::GetBCD8(dataBuff[23]) * 10000 + Data::ByteTool::GetBCD8(dataBuff[24]) * 100 + Data::ByteTool::GetBCD8(dataBuff[25]);
						Double spd = spdDir / 1000;
						Double dir = spdDir % 1000;
						sb->AppendC(UTF8STRC("\r\nSpeed = "));
						Text::SBAppendF64(sb, spd);
						sb->AppendC(UTF8STRC(" knot\r\nDirection = "));
						Text::SBAppendF64(sb, dir);
						sb->AppendC(UTF8STRC("\r\nStatus1 = 0x"));
						sb->AppendHex8(dataBuff[26]);
						sb->AppendC(UTF8STRC("\r\nStatus2 = 0x"));
						sb->AppendHex8(dataBuff[27]);
						sb->AppendC(UTF8STRC("\r\nPeriod = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[28]));
					}
				}
				break;
			case 7:
				sb->AppendC(UTF8STRC(", Check Parameter Reply"));
				if (detail)
				{
					if (cmdSize == 23)
					{
						sb->AppendC(UTF8STRC("\r\nStatus1 = 0x"));
						sb->AppendHex8(dataBuff[9]);
						sb->AppendC(UTF8STRC("\r\nStatus2 = 0x"));
						sb->AppendHex8(dataBuff[10]);
						sb->AppendC(UTF8STRC("\r\nStatus3 = 0x"));
						sb->AppendHex8(dataBuff[11]);
						sb->AppendC(UTF8STRC("\r\nVersion = "));
						sb->AppendC((const UTF8Char*)&dataBuff[14], 8);
					}
				}
				break;
			case 0xa:
				sb->AppendC(UTF8STRC(", MDT Message Echo"));
				if (detail)
				{
					if (cmdSize == 11 + (UInt32)dataBuff[9])
					{
						sb->AppendC(UTF8STRC("\r\nMessage = "));
						sb->AppendC((const UTF8Char*)&dataBuff[10], cmdSize - 11);
					}
				}
				break;
			case 0xb:
				sb->AppendC(UTF8STRC(", Text Message Reply"));
				if (detail)
				{
					if (cmdSize == 11)
					{
						sb->AppendC(UTF8STRC("\r\nState = "));
						if (dataBuff[9] == 0x8f)
						{
							sb->AppendC(UTF8STRC("Accept"));
						}
						else if (dataBuff[9] == 0x8e)
						{
							sb->AppendC(UTF8STRC("Reject"));
						}
						else
						{
							sb->AppendC(UTF8STRC("Unknown (0x"));
							sb->AppendHex8(dataBuff[9]);
							sb->AppendC(UTF8STRC(")"));
						}
					}
				}
				break;
			case 0x0c:
				sb->AppendC(UTF8STRC(", SOS Alert"));
				if (detail)
				{
					if (cmdSize == 11)
					{
						sb->AppendC(UTF8STRC("\r\nState = "));
						sb->AppendU16(dataBuff[9]);
						if (dataBuff[9] == 1)
						{
							sb->AppendC(UTF8STRC(" (Driver Press SOS button)"));
						}
					}
				}
				break;
			case 0x11:
				sb->AppendC(UTF8STRC(", Fuel operation reply"));
				break;
			case 0x1a:
				sb->AppendC(UTF8STRC(", Set Basic Parameters reply"));
				break;
			case 0x1b:
				sb->AppendC(UTF8STRC(", Phone monitoring reply"));
				if (detail)
				{
					if (cmdSize == 11)
					{
						sb->AppendC(UTF8STRC("\r\nStatus = "));
						sb->AppendU16(dataBuff[9]);
						switch (dataBuff[9])
						{
						case 0:
							sb->AppendC(UTF8STRC(" (Success)"));
							break;
						case 1:
							sb->AppendC(UTF8STRC(" (Interrupted)"));
							break;
						case 2:
							sb->AppendC(UTF8STRC(" (Overtime)"));
							break;
						case 3:
							sb->AppendC(UTF8STRC(" (No answer)"));
							break;
						case 4:
							sb->AppendC(UTF8STRC(" (Busy)"));
							break;
						case 5:
							sb->AppendC(UTF8STRC(" (Finished)"));
							break;
						}
					}
				}
				break;
			case 0x1f:
			case 0x94:
				if (dataBuff[3] == 0x1f)
				{
					sb->AppendC(UTF8STRC(", GPS Data 1 - Temperature"));
				}
				else
				{
					sb->AppendC(UTF8STRC(", GPS Data - Temperature"));
				}
				if (detail)
				{
					if (cmdSize == 48)
					{
						Data::DateTime t;
						sb->AppendC(UTF8STRC("\r\nGPS Time = "));
						t.SetValue((UInt16)(Data::ByteTool::GetBCD8(dataBuff[14]) + 2000), Data::ByteTool::GetBCD8(dataBuff[13]), Data::ByteTool::GetBCD8(dataBuff[12]), Data::ByteTool::GetBCD8(dataBuff[9]), Data::ByteTool::GetBCD8(dataBuff[10]), Data::ByteTool::GetBCD8(dataBuff[11]), 0, 0);
						sb->AppendDate(&t);

						Double lat;
						Double lon;
						lat = ReadMUInt32(&dataBuff[19]) / 100000.0;
						lon = ReadMUInt32(&dataBuff[15]) / 100000.0;
						Int32 latI = (Int32)(lat / 100);
						Int32 lonI = (Int32)(lon / 100);
						lat = latI + (lat - latI * 100) / 60;
						lon = lonI + (lon - lonI * 100) / 60;
						if (dataBuff[26] & 1)
						{
							lon = -lon;
						}
						if (dataBuff[26] & 2)
						{
							lat = -lat;
						}
						sb->AppendC(UTF8STRC("\r\nLatitude = "));
						Text::SBAppendF64(sb, lat);
						sb->AppendC(UTF8STRC("\r\nLongitude = "));
						Text::SBAppendF64(sb, lon);
						Int32 spdDir = Data::ByteTool::GetBCD8(dataBuff[23]) * 10000 + Data::ByteTool::GetBCD8(dataBuff[24]) * 100 + Data::ByteTool::GetBCD8(dataBuff[25]);
						Double spd = spdDir / 1000;
						Double dir = spdDir % 1000;
						sb->AppendC(UTF8STRC("\r\nSpeed = "));
						Text::SBAppendF64(sb, spd);
						sb->AppendC(UTF8STRC(" knot\r\nDirection = "));
						Text::SBAppendF64(sb, dir);
						sb->AppendC(UTF8STRC("\r\nStatus1 = 0x"));
						sb->AppendHex8(dataBuff[26]);
						sb->AppendC(UTF8STRC("\r\nStatus2 = 0x"));
						sb->AppendHex8(dataBuff[27]);
						sb->AppendC(UTF8STRC("\r\nTemperature1 = "));
						if (dataBuff[28] == 0x2d)
						{
							sb->AppendI64(-(Int64)ReadMUInt32(&dataBuff[29]));
						}
						else
						{
							sb->AppendI64((Int64)ReadMUInt32(&dataBuff[29]));
						}
						sb->AppendC(UTF8STRC("\r\nTemperature2 = "));
						if (dataBuff[33] == 0x2d)
						{
							sb->AppendI64(-(Int64)ReadMUInt32(&dataBuff[34]));
						}
						else
						{
							sb->AppendU32(ReadMUInt32(&dataBuff[34]));
						}
						sb->AppendC(UTF8STRC("\r\nTemperature3 = "));
						if (dataBuff[38] == 0x2d)
						{
							sb->AppendI64(-(Int64)ReadMUInt32(&dataBuff[39]));
						}
						else
						{
							sb->AppendU32(ReadMUInt32(&dataBuff[39]));
						}
						sb->AppendC(UTF8STRC("\r\nPeriod = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[43]));
						sb->AppendC(UTF8STRC("\r\nStatus3 = 0x"));
						sb->AppendHex8(dataBuff[45]);
						sb->AppendC(UTF8STRC("\r\nStatus4 = 0x"));
						sb->AppendHex8(dataBuff[46]);
					}
					else if (cmdSize == 58)
					{
						Data::DateTime t;
						sb->AppendC(UTF8STRC("\r\nGPS Time = "));
						t.SetValue((UInt16)(Data::ByteTool::GetBCD8(dataBuff[14]) + 2000), Data::ByteTool::GetBCD8(dataBuff[13]), Data::ByteTool::GetBCD8(dataBuff[12]), Data::ByteTool::GetBCD8(dataBuff[9]), Data::ByteTool::GetBCD8(dataBuff[10]), Data::ByteTool::GetBCD8(dataBuff[11]), 0, 0);
						sb->AppendDate(&t);

						Double lat;
						Double lon;
						lat = ReadMUInt32(&dataBuff[19]) / 100000.0;
						lon = ReadMUInt32(&dataBuff[15]) / 100000.0;
						Int32 latI = (Int32)(lat / 100);
						Int32 lonI = (Int32)(lon / 100);
						lat = latI + (lat - latI * 100) / 60;
						lon = lonI + (lon - lonI * 100) / 60;
						if (dataBuff[26] & 1)
						{
							lon = -lon;
						}
						if (dataBuff[26] & 2)
						{
							lat = -lat;
						}
						sb->AppendC(UTF8STRC("\r\nLatitude = "));
						Text::SBAppendF64(sb, lat);
						sb->AppendC(UTF8STRC("\r\nLongitude = "));
						Text::SBAppendF64(sb, lon);
						Int32 spdDir = Data::ByteTool::GetBCD8(dataBuff[23]) * 10000 + Data::ByteTool::GetBCD8(dataBuff[24]) * 100 + Data::ByteTool::GetBCD8(dataBuff[25]);
						Double spd = spdDir / 1000;
						Double dir = spdDir % 1000;
						sb->AppendC(UTF8STRC("\r\nSpeed = "));
						Text::SBAppendF64(sb, spd);
						sb->AppendC(UTF8STRC(" knot\r\nDirection = "));
						Text::SBAppendF64(sb, dir);
						sb->AppendC(UTF8STRC("\r\nStatus1 = 0x"));
						sb->AppendHex8(dataBuff[26]);
						sb->AppendC(UTF8STRC("\r\nStatus2 = 0x"));
						sb->AppendHex8(dataBuff[27]);
						sb->AppendC(UTF8STRC("\r\nTemperature1 = "));
						if (dataBuff[28] == 0x2d)
						{
							sb->AppendI64(-(Int64)ReadMUInt32(&dataBuff[29]));
						}
						else
						{
							sb->AppendU32(ReadMUInt32(&dataBuff[29]));
						}
						sb->AppendC(UTF8STRC("\r\nTemperature2 = "));
						if (dataBuff[33] == 0x2d)
						{
							sb->AppendI64(-(Int64)ReadMUInt32(&dataBuff[34]));
						}
						else
						{
							sb->AppendU32(ReadMUInt32(&dataBuff[34]));
						}
						sb->AppendC(UTF8STRC("\r\nTemperature3 = "));
						if (dataBuff[38] == 0x2d)
						{
							sb->AppendI64(-(Int64)ReadMUInt32(&dataBuff[39]));
						}
						else
						{
							sb->AppendU32(ReadMUInt32(&dataBuff[39]));
						}
						sb->AppendC(UTF8STRC("\r\nPeriod = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[43]));
						sb->AppendC(UTF8STRC("\r\nStatus3 = 0x"));
						sb->AppendHex8(dataBuff[45]);
						sb->AppendC(UTF8STRC("\r\nStatus4 = 0x"));
						sb->AppendHex8(dataBuff[46]);
						sb->AppendC(UTF8STRC("\r\nSatellite in use = "));
						sb->AppendU16(dataBuff[47]);
						sb->AppendC(UTF8STRC("\r\nSatellite in view = "));
						sb->AppendU16(dataBuff[48]);
						sb->AppendC(UTF8STRC("\r\nDelta mileage = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[49]));
						sb->AppendC(UTF8STRC("m\r\nMileage timezone = "));
						sb->AppendU16(dataBuff[51]);
						sb->AppendC(UTF8STRC("\r\nDaily mileage = "));
						sb->AppendU32(ReadMUInt16(&dataBuff[52]) * (UInt32)100);
						sb->AppendC(UTF8STRC("m\r\nFuel level = "));
						Text::SBAppendF64(sb, ReadMUInt16(&dataBuff[54]) * 0.1);
						sb->AppendC(UTF8STRC("%\r\nStatus5 = 0x"));
						sb->AppendHex8(dataBuff[56]);
					}
				}
				break;
			case 0x22:
				sb->AppendC(UTF8STRC(", Set Temperature Alert reply"));
				break;
			case 0x3f:
			case 0x42:
				if (dataBuff[3] == 0x3f)
				{
					sb->AppendC(UTF8STRC(", GPS Data 2 - Mileage, Fuel"));
				}
				else
				{
					sb->AppendC(UTF8STRC(", Fixed Mileage GPS Data"));
				}
				if (detail)
				{
					if (cmdSize == 43)
					{
						Data::DateTime t;
						sb->AppendC(UTF8STRC("\r\nGPS Time = "));
						t.SetValue((UInt16)(Data::ByteTool::GetBCD8(dataBuff[14]) + 2000), Data::ByteTool::GetBCD8(dataBuff[13]), Data::ByteTool::GetBCD8(dataBuff[12]), Data::ByteTool::GetBCD8(dataBuff[9]), Data::ByteTool::GetBCD8(dataBuff[10]), Data::ByteTool::GetBCD8(dataBuff[11]), 0, 0);
						sb->AppendDate(&t);

						Double lat;
						Double lon;
						lat = ReadMUInt32(&dataBuff[19]) / 100000.0;
						lon = ReadMUInt32(&dataBuff[15]) / 100000.0;
						Int32 latI = (Int32)(lat / 100);
						Int32 lonI = (Int32)(lon / 100);
						lat = latI + (lat - latI * 100) / 60;
						lon = lonI + (lon - lonI * 100) / 60;
						if (dataBuff[26] & 1)
						{
							lon = -lon;
						}
						if (dataBuff[26] & 2)
						{
							lat = -lat;
						}
						sb->AppendC(UTF8STRC("\r\nLatitude = "));
						Text::SBAppendF64(sb, lat);
						sb->AppendC(UTF8STRC("\r\nLongitude = "));
						Text::SBAppendF64(sb, lon);
						Int32 spdDir = Data::ByteTool::GetBCD8(dataBuff[23]) * 10000 + Data::ByteTool::GetBCD8(dataBuff[24]) * 100 + Data::ByteTool::GetBCD8(dataBuff[25]);
						Double spd = spdDir / 1000;
						Double dir = spdDir % 1000;
						sb->AppendC(UTF8STRC("\r\nSpeed = "));
						Text::SBAppendF64(sb, spd);
						sb->AppendC(UTF8STRC(" knot\r\nDirection = "));
						Text::SBAppendF64(sb, dir);
						sb->AppendC(UTF8STRC("\r\nStatus1 = 0x"));
						sb->AppendHex8(dataBuff[26]);
						sb->AppendC(UTF8STRC("\r\nStatus2 = 0x"));
						sb->AppendHex8(dataBuff[27]);
						sb->AppendC(UTF8STRC("\r\nPeriod = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[28]));
						sb->AppendC(UTF8STRC("\r\nStatus3 = 0x"));
						sb->AppendHex8(dataBuff[30]);
						sb->AppendC(UTF8STRC("\r\nStatus4 = 0x"));
						sb->AppendHex8(dataBuff[31]);
						sb->AppendC(UTF8STRC("\r\nSatellite in use = "));
						sb->AppendU16(dataBuff[32]);
						sb->AppendC(UTF8STRC("\r\nSatellite in view = "));
						sb->AppendU16(dataBuff[33]);
						sb->AppendC(UTF8STRC("\r\nDelta mileage = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[34]));
						sb->AppendC(UTF8STRC("m\r\nMileage timezone = "));
						sb->AppendU16(dataBuff[36]);
						sb->AppendC(UTF8STRC("\r\nDaily mileage = "));
						sb->AppendU32(ReadMUInt16(&dataBuff[37]) * (UInt32)100);
						sb->AppendC(UTF8STRC("m\r\nFuel level = "));
						Text::SBAppendF64(sb, ReadMUInt16(&dataBuff[39]) * 0.1);
						sb->AppendC(UTF8STRC("%\r\nStatus5 = 0x"));
						sb->AppendHex8(dataBuff[41]);
					}
				}
				break;
			case 0x46:
				sb->AppendC(UTF8STRC(", Get Timezone reply"));
				if (detail)
				{
					if (cmdSize == 11)
					{
						sb->AppendC(UTF8STRC("\r\nTimezone = "));
						sb->AppendU16(dataBuff[9]);
					}
				}
				break;
			case 0x53:
				sb->AppendC(UTF8STRC(", Set DeadZone reporting period reply"));
				break;
			case 0x65:
				sb->AppendC(UTF8STRC(", Card Data"));
				if (detail)
				{
					if (cmdSize == 15)
					{
						sb->AppendC(UTF8STRC("\r\nID = "));
						sb->AppendU32(ReadMUInt32(&dataBuff[9]));
					}
				}
				break;
			case 0x67:
				sb->AppendC(UTF8STRC(", GPS Data 3 - Temperature, Humidity"));
				if (detail)
				{
					if (cmdSize == 45)
					{
						Data::DateTime t;
						sb->AppendC(UTF8STRC("\r\nGPS Time = "));
						t.SetValue((UInt16)(Data::ByteTool::GetBCD8(dataBuff[14]) + 2000), Data::ByteTool::GetBCD8(dataBuff[13]), Data::ByteTool::GetBCD8(dataBuff[12]), Data::ByteTool::GetBCD8(dataBuff[9]), Data::ByteTool::GetBCD8(dataBuff[10]), Data::ByteTool::GetBCD8(dataBuff[11]), 0, 0);
						sb->AppendDate(&t);

						Double lat;
						Double lon;
						lat = ReadMUInt32(&dataBuff[19]) / 100000.0;
						lon = ReadMUInt32(&dataBuff[15]) / 100000.0;
						Int32 latI = (Int32)(lat / 100);
						Int32 lonI = (Int32)(lon / 100);
						lat = latI + (lat - latI * 100) / 60;
						lon = lonI + (lon - lonI * 100) / 60;
						if (dataBuff[26] & 1)
						{
							lon = -lon;
						}
						if (dataBuff[26] & 2)
						{
							lat = -lat;
						}
						sb->AppendC(UTF8STRC("\r\nLatitude = "));
						Text::SBAppendF64(sb, lat);
						sb->AppendC(UTF8STRC("\r\nLongitude = "));
						Text::SBAppendF64(sb, lon);
						Int32 spdDir = Data::ByteTool::GetBCD8(dataBuff[23]) * 10000 + Data::ByteTool::GetBCD8(dataBuff[24]) * 100 + Data::ByteTool::GetBCD8(dataBuff[25]);
						Double spd = spdDir / 1000;
						Double dir = spdDir % 1000;
						sb->AppendC(UTF8STRC("\r\nSpeed = "));
						Text::SBAppendF64(sb, spd);
						sb->AppendC(UTF8STRC(" knot\r\nDirection = "));
						Text::SBAppendF64(sb, dir);
						sb->AppendC(UTF8STRC("\r\nStatus1 = 0x"));
						sb->AppendHex8(dataBuff[26]);
						sb->AppendC(UTF8STRC("\r\nStatus2 = 0x"));
						sb->AppendHex8(dataBuff[27]);
						sb->AppendC(UTF8STRC("\r\nPeriod = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[28]));
						sb->AppendC(UTF8STRC("\r\nStatus3 = 0x"));
						sb->AppendHex8(dataBuff[30]);
						sb->AppendC(UTF8STRC("\r\nStatus4 = 0x"));
						sb->AppendHex8(dataBuff[31]);
						sb->AppendC(UTF8STRC("\r\nSatellite in use = "));
						sb->AppendU16(dataBuff[32]);
						sb->AppendC(UTF8STRC("\r\nSatellite in view = "));
						sb->AppendU16(dataBuff[33]);
						sb->AppendC(UTF8STRC("\r\nTemperature Valid = "));
						if (dataBuff[34] & 1)
						{
							Double t = -39.65 + 0.01 * ReadMUInt32(&dataBuff[35]);
							sb->AppendC(UTF8STRC("True\r\nTemperature = "));
							Text::SBAppendF64(sb, t);
							sb->AppendC(UTF8STRC("\r\nRH Valid = "));
							if (dataBuff[39] & 1)
							{
								UInt32 rh = ReadMUInt32(&dataBuff[40]);
								Double rhl = -2.0468 + 0.0367 * -(1.5955 * rh * rh) * 0.000001;
								sb->AppendC(UTF8STRC("True\r\nRH(Linear) = "));
								Text::SBAppendF64(sb, rhl);
								sb->AppendC(UTF8STRC("%\r\nRH (Refined) = "));
								Text::SBAppendF64(sb, (t - 25.0) * (0.01 + 0.00008 * rh) + rhl);
							}
							else
							{
								sb->AppendC(UTF8STRC("False"));
							}
						}
						else
						{
							sb->AppendC(UTF8STRC("False"));
						}
					}
				}
				break;
			case 0x6e:
				sb->AppendC(UTF8STRC(", LED Tran Msg reply"));
				break;
			case 0x72:
				sb->AppendC(UTF8STRC(", LED Msg reply"));
				break;
			case 0x74:
				sb->AppendC(UTF8STRC(", LED Delete Msg reply"));
				break;
			case 0x75:
				sb->AppendC(UTF8STRC(", LED Notify Msg reply"));
				break;
			case 0x77:
				sb->AppendC(UTF8STRC(", Delete LED Notify Msg reply"));
				break;
			case 0x7c:
				sb->AppendC(UTF8STRC(", MDT message unicode echo"));
				break;
			case 0x7d:
				sb->AppendC(UTF8STRC(", Set MDT Phone reply"));
				break;
			case 0x7f:
				sb->AppendC(UTF8STRC(", Capture photo reply"));
				if (detail)
				{
					if (cmdSize == 13)
					{
						sb->AppendC(UTF8STRC(", Size = "));
						sb->AppendU32(ReadMUInt24(&dataBuff[9]));
					}
				}
				break;
			case 0x80:
				sb->AppendC(UTF8STRC(", Photo packet"));
				if (detail)
				{
					sb->AppendC(UTF8STRC(", Packet ID = "));
					sb->AppendU16(ReadMUInt16(&dataBuff[9]));
					sb->AppendC(UTF8STRC(", Data Size = "));
					sb->AppendU16(ReadMUInt16(&dataBuff[11]));
					sb->AppendC(UTF8STRC(", CheckDigits = "));
					sb->AppendHex16(ReadMUInt16(&dataBuff[cmdSize - 3]));
				}
				break;
			case 0x81:
				sb->AppendC(UTF8STRC(", Photo end reply"));
				break;
			case 0x8c:
				sb->AppendC(UTF8STRC(", Get Server time"));
				break;
			case 0x8d:
				sb->AppendC(UTF8STRC(", Get Device Battery"));
				if (detail)
				{
					if (cmdSize == 23)
					{
						Data::DateTime dt;
						dt.SetValue((UInt16)(Data::ByteTool::GetBCD8(dataBuff[14]) + 2000), Data::ByteTool::GetBCD8(dataBuff[13]), Data::ByteTool::GetBCD8(dataBuff[12]), Data::ByteTool::GetBCD8(dataBuff[9]), Data::ByteTool::GetBCD8(dataBuff[10]), Data::ByteTool::GetBCD8(dataBuff[11]), 0, (Int8)((Int8)dataBuff[15] * 4));
						sb->AppendC(UTF8STRC("\r\nDevice Time = "));
						sb->AppendDate(&dt);
						sb->AppendC(UTF8STRC("\r\nBattery Status = 0x"));
						sb->AppendHex8(dataBuff[16]);
						sb->AppendC(UTF8STRC("\r\nBattery Level = "));
						sb->AppendU16(dataBuff[17]);
						sb->AppendC(UTF8STRC("\r\nRSSI = "));
						sb->AppendU16(dataBuff[18]);
						sb->AppendC(UTF8STRC("\r\nDevice Status = 0x"));
						sb->AppendHex8(dataBuff[19]);
						sb->AppendC(UTF8STRC("\r\nCard Space = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[20]));
						sb->AppendC(UTF8STRC("MB"));
					}
				}
				break;
			case 0x91:
				sb->AppendC(UTF8STRC(", Wireless Sensor Data"));
				if (detail)
				{
					if (cmdSize == 43)
					{
						sb->AppendC(UTF8STRC("\r\nChannel = "));
						sb->AppendU16(dataBuff[9]);
						sb->AppendC(UTF8STRC("\r\nRSSI = "));
						sb->AppendU16(dataBuff[10]);
						sb->AppendC(UTF8STRC("\r\nSensor IP = "));
						sb->AppendU16(dataBuff[11]);
						sb->AppendUTF8Char('.');
						sb->AppendU16(dataBuff[12]);
						sb->AppendUTF8Char('.');
						sb->AppendU16(dataBuff[13]);
						sb->AppendUTF8Char('.');
						sb->AppendU16(dataBuff[14]);
						sb->AppendC(UTF8STRC("\r\nSensor Port = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[15]));
						sb->AppendC(UTF8STRC("\r\nSensor ID = "));
						sb->AppendHexBuff(&dataBuff[17], 5, 0, Text::LineBreakType::None);
						Data::DateTime t;
						sb->AppendC(UTF8STRC("\r\nGPS Time = "));
						t.SetValue((UInt16)(Data::ByteTool::GetBCD8(dataBuff[27]) + 2000), Data::ByteTool::GetBCD8(dataBuff[26]), Data::ByteTool::GetBCD8(dataBuff[25]), Data::ByteTool::GetBCD8(dataBuff[22]), Data::ByteTool::GetBCD8(dataBuff[23]), Data::ByteTool::GetBCD8(dataBuff[24]), 0, 0);
						sb->AppendDate(&t);
						sb->AppendC(UTF8STRC("\r\nStatus1 = 0x"));
						sb->AppendHex8(dataBuff[28]);
						sb->AppendC(UTF8STRC("\r\nTemperature = "));
						if (dataBuff[28] & 4)
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[29]) * 0.0001);
						}
						else
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[29]) * 0.01 - 39.65);
						}
						sb->AppendC(UTF8STRC("\r\nStatus2 = 0x"));
						sb->AppendHex8(dataBuff[33]);
						sb->AppendC(UTF8STRC("\r\nHumidity = "));
						if (dataBuff[33] & 2)
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[34]) * 0.01);
						}
						else
						{
							Double y = ReadMInt32(&dataBuff[34]);
							Text::SBAppendF64(sb, -2.0468 + 0.0367 * y -(1.5955 * y * y) / 1000000);
						}
						sb->AppendC(UTF8STRC("\r\nBattery = "));
						Text::SBAppendF64(sb, dataBuff[38] / 128.0 * 3.28);
						sb->AppendC(UTF8STRC("\r\nStatus3 = 0x"));
						sb->AppendHex8(dataBuff[39]);
						sb->AppendC(UTF8STRC("\r\nPeriod = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[40]));
					}
					else if (cmdSize == 58)
					{
						sb->AppendC(UTF8STRC("\r\nChannel = "));
						sb->AppendU16(dataBuff[9]);
						sb->AppendC(UTF8STRC("\r\nRSSI = "));
						sb->AppendU16(dataBuff[10]);
						sb->AppendC(UTF8STRC("\r\nSensor IP = "));
						sb->AppendU16(dataBuff[11]);
						sb->AppendUTF8Char('.');
						sb->AppendU16(dataBuff[12]);
						sb->AppendUTF8Char('.');
						sb->AppendU16(dataBuff[13]);
						sb->AppendUTF8Char('.');
						sb->AppendU16(dataBuff[14]);
						sb->AppendC(UTF8STRC("\r\nSensor Port = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[15]));
						sb->AppendC(UTF8STRC("\r\nSensor ID = "));
						sb->AppendHexBuff(&dataBuff[17], 5, 0, Text::LineBreakType::None);
						Data::DateTime t;
						sb->AppendC(UTF8STRC("\r\nGPS Time = "));
						t.SetValue((UInt16)(Data::ByteTool::GetBCD8(dataBuff[27]) + 2000), Data::ByteTool::GetBCD8(dataBuff[26]), Data::ByteTool::GetBCD8(dataBuff[25]), Data::ByteTool::GetBCD8(dataBuff[22]), Data::ByteTool::GetBCD8(dataBuff[23]), Data::ByteTool::GetBCD8(dataBuff[24]), 0, 0);
						sb->AppendDate(&t);
						sb->AppendC(UTF8STRC("\r\nStatus1 = 0x"));
						sb->AppendHex8(dataBuff[28]);
						sb->AppendC(UTF8STRC("\r\nTemperature = "));
						if (dataBuff[28] & 4)
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[29]) * 0.0001);
						}
						else
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[29]) * 0.01 - 39.65);
						}
						sb->AppendC(UTF8STRC("\r\nStatus2 = 0x"));
						sb->AppendHex8(dataBuff[33]);
						sb->AppendC(UTF8STRC("\r\nHumidity = "));
						if (dataBuff[33] & 2)
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[34]) * 0.01);
						}
						else
						{
							Double y = ReadMInt32(&dataBuff[34]);
							Text::SBAppendF64(sb, -2.0468 + 0.0367 * y -(1.5955 * y * y) / 1000000);
						}
						sb->AppendC(UTF8STRC("\r\nBattery = "));
						Text::SBAppendF64(sb, dataBuff[38] / 128.0 * 3.28);
						sb->AppendC(UTF8STRC("\r\nStatus3 = 0x"));
						sb->AppendHex8(dataBuff[39]);
						sb->AppendC(UTF8STRC("\r\nPeriod = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[40]));
						sb->AppendC(UTF8STRC("\r\nStatus4 = 0x"));
						sb->AppendHex8(dataBuff[42]);
						sb->AppendC(UTF8STRC("\r\nPosition Status = 0x"));
						sb->AppendHex8(dataBuff[43]);

						Double lat;
						Double lon;
						lat = ReadMUInt32(&dataBuff[48]) / 100000.0;
						lon = ReadMUInt32(&dataBuff[44]) / 100000.0;
						Int32 latI = (Int32)(lat / 100);
						Int32 lonI = (Int32)(lon / 100);
						lat = latI + (lat - latI * 100) / 60;
						lon = lonI + (lon - lonI * 100) / 60;
						if (dataBuff[39] & 1)
						{
							lon = -lon;
						}
						if (dataBuff[39] & 2)
						{
							lat = -lat;
						}
						sb->AppendC(UTF8STRC("\r\nLatitude = "));
						Text::SBAppendF64(sb, lat);
						sb->AppendC(UTF8STRC("\r\nLongitude = "));
						Text::SBAppendF64(sb, lon);
						sb->AppendC(UTF8STRC("\r\nAltitude = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[52]));
						Int32 spdDir = Data::ByteTool::GetBCD8(dataBuff[54]) * 10000 + Data::ByteTool::GetBCD8(dataBuff[55]) * 100 + Data::ByteTool::GetBCD8(dataBuff[56]);
						Double spd = spdDir / 1000;
						Double dir = spdDir % 1000;
						sb->AppendC(UTF8STRC("\r\nSpeed = "));
						Text::SBAppendF64(sb, spd);
						sb->AppendC(UTF8STRC(" knot\r\nDirection = "));
						Text::SBAppendF64(sb, dir);
					}
				}
				break;
			case 0x93:
				sb->AppendC(UTF8STRC(", Wireless Sensor with Repeater Data"));
				if (detail)
				{
					if (cmdSize >= 52)
					{
						sb->AppendC(UTF8STRC("\r\nChannel = "));
						sb->AppendU16(dataBuff[9]);
						sb->AppendC(UTF8STRC("\r\nRSSI = "));
						sb->AppendU16(dataBuff[10]);
						sb->AppendC(UTF8STRC("\r\nSensor IP = "));
						sb->AppendU16(dataBuff[11]);
						sb->AppendUTF8Char('.');
						sb->AppendU16(dataBuff[12]);
						sb->AppendUTF8Char('.');
						sb->AppendU16(dataBuff[13]);
						sb->AppendUTF8Char('.');
						sb->AppendU16(dataBuff[14]);
						sb->AppendC(UTF8STRC("\r\nSensor Port = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[15]));
						sb->AppendC(UTF8STRC("\r\nSensor ID = "));
						sb->AppendHexBuff(&dataBuff[17], 5, 0, Text::LineBreakType::None);
						Data::DateTime t;
						sb->AppendC(UTF8STRC("\r\nGPS Time = "));
						t.SetValue((UInt16)(Data::ByteTool::GetBCD8(dataBuff[27]) + 2000), Data::ByteTool::GetBCD8(dataBuff[26]), Data::ByteTool::GetBCD8(dataBuff[25]), Data::ByteTool::GetBCD8(dataBuff[22]), Data::ByteTool::GetBCD8(dataBuff[23]), Data::ByteTool::GetBCD8(dataBuff[24]), 0, 0);
						sb->AppendDate(&t);
						sb->AppendC(UTF8STRC("\r\nStatus1 = 0x"));
						sb->AppendHex8(dataBuff[28]);
						sb->AppendC(UTF8STRC("\r\nTemperature = "));
						if (dataBuff[28] & 4)
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[29]) * 0.0001);
						}
						else
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[29]) * 0.01 - 39.65);
						}
						sb->AppendC(UTF8STRC("\r\nStatus2 = 0x"));
						sb->AppendHex8(dataBuff[33]);
						sb->AppendC(UTF8STRC("\r\nHumidity = "));
						if (dataBuff[33] & 2)
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[34]) * 0.01);
						}
						else
						{
							Double y = ReadMInt32(&dataBuff[34]);
							Text::SBAppendF64(sb, -2.0468 + 0.0367 * y -(1.5955 * y * y) / 1000000);
						}
						sb->AppendC(UTF8STRC("\r\nBattery = "));
						Text::SBAppendF64(sb, dataBuff[38] / 128.0 * 3.28);
						sb->AppendC(UTF8STRC("\r\nStatus3 = 0x"));
						sb->AppendHex8(dataBuff[39]);
						sb->AppendC(UTF8STRC("\r\nPeriod = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[40]));
						//////////////////////////////
					}
				}
				break;
			default:
				sb->AppendC(UTF8STRC(", Unknown"));
				break;
			}
		}
		else
		{
			sb->AppendC(UTF8STRC("Unknown protocol"));
		}
	}
	else if (buffSize >= 10 && dataBuff[0] == 0xfe && dataBuff[buffSize - 1] == 0xfe)
	{
		UInt32 cmdSize = (UInt32)(dataBuff[1] << 8) | dataBuff[2];
		Int64 termId = Data::ByteTool::GetBCD8(dataBuff[4]) * 100000000LL + Data::ByteTool::GetBCD32(&dataBuff[5]);
		if (cmdSize <= buffSize)
		{
			sb->AppendC(UTF8STRC("SP1000SC: "));
			sb->AppendI64(termId);
			switch (dataBuff[3])
			{
			case 1:
				sb->AppendC(UTF8STRC(", Register reply"));
				if (detail)
				{
					if (cmdSize == 13)
					{
						sb->AppendC(UTF8STRC("\r\nACK = "));
						sb->AppendU16(dataBuff[9]);
						if (dataBuff[9] == 1)
						{
							sb->AppendC(UTF8STRC(" (Success)"));
						}
						else if (dataBuff[9] == 0)
						{
							sb->AppendC(UTF8STRC(" (Failed)"));
						}
						sb->AppendC(UTF8STRC("\r\nPeriod = "));
						sb->AppendU16((UInt16)((dataBuff[10] << 8) | dataBuff[11]));
						sb->AppendC(UTF8STRC("s"));
					}
				}
				break;
			case 7:
				sb->AppendC(UTF8STRC(", Check Parameter"));
				break;
			case 0xc:
				sb->AppendC(UTF8STRC(", Cancel SOS Alert"));
				break;
			case 0x1a:
				sb->AppendC(UTF8STRC(", Set Basic Parameters"));
				if (detail)
				{
					if (cmdSize == 37)
					{
						sb->AppendC(UTF8STRC("\r\nServer IP = "));
						sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&dataBuff[9]));
						sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
						sb->AppendC(UTF8STRC("\r\nServer Port = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[13]));
						sb->AppendC(UTF8STRC("\r\nNew Device ID = "));
						sb->AppendC((const UTF8Char*)&dataBuff[15], 10);
						sb->AppendC(UTF8STRC("\r\nAPN = "));
						sb->AppendC((const UTF8Char*)&dataBuff[25], 11);
					}
					else if (cmdSize == 56)
					{
						sb->AppendC(UTF8STRC("\r\nServer IP = "));
						sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&dataBuff[9]));
						sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
						sb->AppendC(UTF8STRC("\r\nServer Port = "));
						sb->AppendU16(ReadMUInt16(&dataBuff[13]));
						sb->AppendC(UTF8STRC("\r\nNew Device ID = "));
						sb->AppendC((const UTF8Char*)&dataBuff[15], 10);
						sb->AppendC(UTF8STRC("\r\nAPN = "));
						sb->AppendC((const UTF8Char*)&dataBuff[25], 30);
					}
				}
				break;
			default:
				sb->AppendC(UTF8STRC(", Unknown"));
				break;
			}
		}
		else
		{
			sb->AppendC(UTF8STRC("Unknown protocol"));
		}
	}
	else if (dataBuff[buffSize - 2] == 0x0D && dataBuff[buffSize - 1] == 0x0A)
	{
		sb->AppendC((const UTF8Char*)dataBuff, buffSize - 2);
	}
	else if (dataBuff[0] == 0x24 && dataBuff[buffSize - 1] == 0x21)
	{
		sb->AppendC((const UTF8Char*)dataBuff, buffSize);
	}
	else
	{
		sb->AppendC(UTF8STRC("Unknown protocol"));
	}
	return true;
}

IO::UDPLog::UDPLog(Text::String *sourceName) : IO::LogFile(sourceName)
{
}

IO::UDPLog::UDPLog(const UTF8Char *sourceName) : IO::LogFile(sourceName)
{
}

IO::UDPLog::~UDPLog()
{
}
