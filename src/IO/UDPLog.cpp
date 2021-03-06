#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/UDPLog.h"
#include "Net/SocketFactory.h"
#include "Text/Encoding.h"
#include "Text/MyStringFloat.h"

Bool IO::UDPLog::ParseLog(UInt8 *dataBuff, UOSInt buffSize, Text::StringBuilderUTF *sb, Bool detail)
{
	UTF8Char sbuff[32];

	if (buffSize == 1 && dataBuff[0] == 0x48)
	{
		sb->Append((const UTF8Char*)"SP1000: Heartbeat");
	}
	else if (buffSize >= 10 && dataBuff[0] == 0xff && dataBuff[buffSize - 1] == 0xff)
	{
		UInt32 cmdSize = (UInt32)(dataBuff[1] << 8) | dataBuff[2];
		Int64 termId = Data::ByteTool::GetBCD8(dataBuff[4]) * 100000000LL + Data::ByteTool::GetBCD32(&dataBuff[5]);
		if (cmdSize <= buffSize)
		{
			sb->Append((const UTF8Char*)"SP1000CS: ");
			sb->AppendI64(termId);
			switch (dataBuff[3])
			{
			case 1:
				sb->Append((const UTF8Char*)", Register");
				break;
			case 3:
			case 0x2a:
			case 0x87:
				if (dataBuff[3] == 3)
				{
					sb->Append((const UTF8Char*)", Basic GPS Data");
				}
				else if (dataBuff[3] == 0x2a)
				{
					sb->Append((const UTF8Char*)", Blind zone GPS Data");
				}
				else
				{
					sb->Append((const UTF8Char*)", LBS Data");
				}
				if (detail)
				{
					if (cmdSize == 31)
					{
						Data::DateTime t;
						sb->Append((const UTF8Char*)"\r\nGPS Time = ");
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
						sb->Append((const UTF8Char*)"\r\nLatitude = ");
						Text::SBAppendF64(sb, lat);
						sb->Append((const UTF8Char*)"\r\nLongitude = ");
						Text::SBAppendF64(sb, lon);
						Int32 spdDir = Data::ByteTool::GetBCD8(dataBuff[23]) * 10000 + Data::ByteTool::GetBCD8(dataBuff[24]) * 100 + Data::ByteTool::GetBCD8(dataBuff[25]);
						Double spd = spdDir / 1000;
						Double dir = spdDir % 1000;
						sb->Append((const UTF8Char*)"\r\nSpeed = ");
						Text::SBAppendF64(sb, spd);
						sb->Append((const UTF8Char*)" knot\r\nDirection = ");
						Text::SBAppendF64(sb, dir);
						sb->Append((const UTF8Char*)"\r\nStatus1 = 0x");
						sb->AppendHex8(dataBuff[26]);
						sb->Append((const UTF8Char*)"\r\nStatus2 = 0x");
						sb->AppendHex8(dataBuff[27]);
						sb->Append((const UTF8Char*)"\r\nPeriod = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[28]));
					}
				}
				break;
			case 7:
				sb->Append((const UTF8Char*)", Check Parameter Reply");
				if (detail)
				{
					if (cmdSize == 23)
					{
						sb->Append((const UTF8Char*)"\r\nStatus1 = 0x");
						sb->AppendHex8(dataBuff[9]);
						sb->Append((const UTF8Char*)"\r\nStatus2 = 0x");
						sb->AppendHex8(dataBuff[10]);
						sb->Append((const UTF8Char*)"\r\nStatus3 = 0x");
						sb->AppendHex8(dataBuff[11]);
						sb->Append((const UTF8Char*)"\r\nVersion = ");
						sb->AppendC((const UTF8Char*)&dataBuff[14], 8);
					}
				}
				break;
			case 0xa:
				sb->Append((const UTF8Char*)", MDT Message Echo");
				if (detail)
				{
					if (cmdSize == 11 + (UInt32)dataBuff[9])
					{
						sb->Append((const UTF8Char*)"\r\nMessage = ");
						sb->AppendC((const UTF8Char*)&dataBuff[10], cmdSize - 11);
					}
				}
				break;
			case 0xb:
				sb->Append((const UTF8Char*)", Text Message Reply");
				if (detail)
				{
					if (cmdSize == 11)
					{
						sb->Append((const UTF8Char*)"\r\nState = ");
						if (dataBuff[9] == 0x8f)
						{
							sb->Append((const UTF8Char*)"Accept");
						}
						else if (dataBuff[9] == 0x8e)
						{
							sb->Append((const UTF8Char*)"Reject");
						}
						else
						{
							sb->Append((const UTF8Char*)"Unknown (0x");
							sb->AppendHex8(dataBuff[9]);
							sb->Append((const UTF8Char*)")");
						}
					}
				}
				break;
			case 0x0c:
				sb->Append((const UTF8Char*)", SOS Alert");
				if (detail)
				{
					if (cmdSize == 11)
					{
						sb->Append((const UTF8Char*)"\r\nState = ");
						sb->AppendU16(dataBuff[9]);
						if (dataBuff[9] == 1)
						{
							sb->Append((const UTF8Char*)" (Driver Press SOS button)");
						}
					}
				}
				break;
			case 0x11:
				sb->Append((const UTF8Char*)", Fuel operation reply");
				break;
			case 0x1a:
				sb->Append((const UTF8Char*)", Set Basic Parameters reply");
				break;
			case 0x1b:
				sb->Append((const UTF8Char*)", Phone monitoring reply");
				if (detail)
				{
					if (cmdSize == 11)
					{
						sb->Append((const UTF8Char*)"\r\nStatus = ");
						sb->AppendU16(dataBuff[9]);
						switch (dataBuff[9])
						{
						case 0:
							sb->Append((const UTF8Char*)" (Success)");
							break;
						case 1:
							sb->Append((const UTF8Char*)" (Interrupted)");
							break;
						case 2:
							sb->Append((const UTF8Char*)" (Overtime)");
							break;
						case 3:
							sb->Append((const UTF8Char*)" (No answer)");
							break;
						case 4:
							sb->Append((const UTF8Char*)" (Busy)");
							break;
						case 5:
							sb->Append((const UTF8Char*)" (Finished)");
							break;
						}
					}
				}
				break;
			case 0x1f:
			case 0x94:
				if (dataBuff[3] == 0x1f)
				{
					sb->Append((const UTF8Char*)", GPS Data 1 - Temperature");
				}
				else
				{
					sb->Append((const UTF8Char*)", GPS Data - Temperature");
				}
				if (detail)
				{
					if (cmdSize == 48)
					{
						Data::DateTime t;
						sb->Append((const UTF8Char*)"\r\nGPS Time = ");
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
						sb->Append((const UTF8Char*)"\r\nLatitude = ");
						Text::SBAppendF64(sb, lat);
						sb->Append((const UTF8Char*)"\r\nLongitude = ");
						Text::SBAppendF64(sb, lon);
						Int32 spdDir = Data::ByteTool::GetBCD8(dataBuff[23]) * 10000 + Data::ByteTool::GetBCD8(dataBuff[24]) * 100 + Data::ByteTool::GetBCD8(dataBuff[25]);
						Double spd = spdDir / 1000;
						Double dir = spdDir % 1000;
						sb->Append((const UTF8Char*)"\r\nSpeed = ");
						Text::SBAppendF64(sb, spd);
						sb->Append((const UTF8Char*)" knot\r\nDirection = ");
						Text::SBAppendF64(sb, dir);
						sb->Append((const UTF8Char*)"\r\nStatus1 = 0x");
						sb->AppendHex8(dataBuff[26]);
						sb->Append((const UTF8Char*)"\r\nStatus2 = 0x");
						sb->AppendHex8(dataBuff[27]);
						sb->Append((const UTF8Char*)"\r\nTemperature1 = ");
						if (dataBuff[28] == 0x2d)
						{
							sb->AppendI64(-(Int64)ReadMUInt32(&dataBuff[29]));
						}
						else
						{
							sb->AppendI64((Int64)ReadMUInt32(&dataBuff[29]));
						}
						sb->Append((const UTF8Char*)"\r\nTemperature2 = ");
						if (dataBuff[33] == 0x2d)
						{
							sb->AppendI64(-(Int64)ReadMUInt32(&dataBuff[34]));
						}
						else
						{
							sb->AppendU32(ReadMUInt32(&dataBuff[34]));
						}
						sb->Append((const UTF8Char*)"\r\nTemperature3 = ");
						if (dataBuff[38] == 0x2d)
						{
							sb->AppendI64(-(Int64)ReadMUInt32(&dataBuff[39]));
						}
						else
						{
							sb->AppendU32(ReadMUInt32(&dataBuff[39]));
						}
						sb->Append((const UTF8Char*)"\r\nPeriod = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[43]));
						sb->Append((const UTF8Char*)"\r\nStatus3 = 0x");
						sb->AppendHex8(dataBuff[45]);
						sb->Append((const UTF8Char*)"\r\nStatus4 = 0x");
						sb->AppendHex8(dataBuff[46]);
					}
					else if (cmdSize == 58)
					{
						Data::DateTime t;
						sb->Append((const UTF8Char*)"\r\nGPS Time = ");
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
						sb->Append((const UTF8Char*)"\r\nLatitude = ");
						Text::SBAppendF64(sb, lat);
						sb->Append((const UTF8Char*)"\r\nLongitude = ");
						Text::SBAppendF64(sb, lon);
						Int32 spdDir = Data::ByteTool::GetBCD8(dataBuff[23]) * 10000 + Data::ByteTool::GetBCD8(dataBuff[24]) * 100 + Data::ByteTool::GetBCD8(dataBuff[25]);
						Double spd = spdDir / 1000;
						Double dir = spdDir % 1000;
						sb->Append((const UTF8Char*)"\r\nSpeed = ");
						Text::SBAppendF64(sb, spd);
						sb->Append((const UTF8Char*)" knot\r\nDirection = ");
						Text::SBAppendF64(sb, dir);
						sb->Append((const UTF8Char*)"\r\nStatus1 = 0x");
						sb->AppendHex8(dataBuff[26]);
						sb->Append((const UTF8Char*)"\r\nStatus2 = 0x");
						sb->AppendHex8(dataBuff[27]);
						sb->Append((const UTF8Char*)"\r\nTemperature1 = ");
						if (dataBuff[28] == 0x2d)
						{
							sb->AppendI64(-(Int64)ReadMUInt32(&dataBuff[29]));
						}
						else
						{
							sb->AppendU32(ReadMUInt32(&dataBuff[29]));
						}
						sb->Append((const UTF8Char*)"\r\nTemperature2 = ");
						if (dataBuff[33] == 0x2d)
						{
							sb->AppendI64(-(Int64)ReadMUInt32(&dataBuff[34]));
						}
						else
						{
							sb->AppendU32(ReadMUInt32(&dataBuff[34]));
						}
						sb->Append((const UTF8Char*)"\r\nTemperature3 = ");
						if (dataBuff[38] == 0x2d)
						{
							sb->AppendI64(-(Int64)ReadMUInt32(&dataBuff[39]));
						}
						else
						{
							sb->AppendU32(ReadMUInt32(&dataBuff[39]));
						}
						sb->Append((const UTF8Char*)"\r\nPeriod = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[43]));
						sb->Append((const UTF8Char*)"\r\nStatus3 = 0x");
						sb->AppendHex8(dataBuff[45]);
						sb->Append((const UTF8Char*)"\r\nStatus4 = 0x");
						sb->AppendHex8(dataBuff[46]);
						sb->Append((const UTF8Char*)"\r\nSatellite in use = ");
						sb->AppendU16(dataBuff[47]);
						sb->Append((const UTF8Char*)"\r\nSatellite in view = ");
						sb->AppendU16(dataBuff[48]);
						sb->Append((const UTF8Char*)"\r\nDelta mileage = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[49]));
						sb->Append((const UTF8Char*)"m\r\nMileage timezone = ");
						sb->AppendU16(dataBuff[51]);
						sb->Append((const UTF8Char*)"\r\nDaily mileage = ");
						sb->AppendU32(ReadMUInt16(&dataBuff[52]) * (UInt32)100);
						sb->Append((const UTF8Char*)"m\r\nFuel level = ");
						Text::SBAppendF64(sb, ReadMUInt16(&dataBuff[54]) * 0.1);
						sb->Append((const UTF8Char*)"%\r\nStatus5 = 0x");
						sb->AppendHex8(dataBuff[56]);
					}
				}
				break;
			case 0x22:
				sb->Append((const UTF8Char*)", Set Temperature Alert reply");
				break;
			case 0x3f:
			case 0x42:
				if (dataBuff[3] == 0x3f)
				{
					sb->Append((const UTF8Char*)", GPS Data 2 - Mileage, Fuel");
				}
				else
				{
					sb->Append((const UTF8Char*)", Fixed Mileage GPS Data");
				}
				if (detail)
				{
					if (cmdSize == 43)
					{
						Data::DateTime t;
						sb->Append((const UTF8Char*)"\r\nGPS Time = ");
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
						sb->Append((const UTF8Char*)"\r\nLatitude = ");
						Text::SBAppendF64(sb, lat);
						sb->Append((const UTF8Char*)"\r\nLongitude = ");
						Text::SBAppendF64(sb, lon);
						Int32 spdDir = Data::ByteTool::GetBCD8(dataBuff[23]) * 10000 + Data::ByteTool::GetBCD8(dataBuff[24]) * 100 + Data::ByteTool::GetBCD8(dataBuff[25]);
						Double spd = spdDir / 1000;
						Double dir = spdDir % 1000;
						sb->Append((const UTF8Char*)"\r\nSpeed = ");
						Text::SBAppendF64(sb, spd);
						sb->Append((const UTF8Char*)" knot\r\nDirection = ");
						Text::SBAppendF64(sb, dir);
						sb->Append((const UTF8Char*)"\r\nStatus1 = 0x");
						sb->AppendHex8(dataBuff[26]);
						sb->Append((const UTF8Char*)"\r\nStatus2 = 0x");
						sb->AppendHex8(dataBuff[27]);
						sb->Append((const UTF8Char*)"\r\nPeriod = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[28]));
						sb->Append((const UTF8Char*)"\r\nStatus3 = 0x");
						sb->AppendHex8(dataBuff[30]);
						sb->Append((const UTF8Char*)"\r\nStatus4 = 0x");
						sb->AppendHex8(dataBuff[31]);
						sb->Append((const UTF8Char*)"\r\nSatellite in use = ");
						sb->AppendU16(dataBuff[32]);
						sb->Append((const UTF8Char*)"\r\nSatellite in view = ");
						sb->AppendU16(dataBuff[33]);
						sb->Append((const UTF8Char*)"\r\nDelta mileage = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[34]));
						sb->Append((const UTF8Char*)"m\r\nMileage timezone = ");
						sb->AppendU16(dataBuff[36]);
						sb->Append((const UTF8Char*)"\r\nDaily mileage = ");
						sb->AppendU32(ReadMUInt16(&dataBuff[37]) * (UInt32)100);
						sb->Append((const UTF8Char*)"m\r\nFuel level = ");
						Text::SBAppendF64(sb, ReadMUInt16(&dataBuff[39]) * 0.1);
						sb->Append((const UTF8Char*)"%\r\nStatus5 = 0x");
						sb->AppendHex8(dataBuff[41]);
					}
				}
				break;
			case 0x46:
				sb->Append((const UTF8Char*)", Get Timezone reply");
				if (detail)
				{
					if (cmdSize == 11)
					{
						sb->Append((const UTF8Char*)"\r\nTimezone = ");
						sb->AppendU16(dataBuff[9]);
					}
				}
				break;
			case 0x53:
				sb->Append((const UTF8Char*)", Set DeadZone reporting period reply");
				break;
			case 0x65:
				sb->Append((const UTF8Char*)", Card Data");
				if (detail)
				{
					if (cmdSize == 15)
					{
						sb->Append((const UTF8Char*)"\r\nID = ");
						sb->AppendU32(ReadMUInt32(&dataBuff[9]));
					}
				}
				break;
			case 0x67:
				sb->Append((const UTF8Char*)", GPS Data 3 - Temperature, Humidity");
				if (detail)
				{
					if (cmdSize == 45)
					{
						Data::DateTime t;
						sb->Append((const UTF8Char*)"\r\nGPS Time = ");
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
						sb->Append((const UTF8Char*)"\r\nLatitude = ");
						Text::SBAppendF64(sb, lat);
						sb->Append((const UTF8Char*)"\r\nLongitude = ");
						Text::SBAppendF64(sb, lon);
						Int32 spdDir = Data::ByteTool::GetBCD8(dataBuff[23]) * 10000 + Data::ByteTool::GetBCD8(dataBuff[24]) * 100 + Data::ByteTool::GetBCD8(dataBuff[25]);
						Double spd = spdDir / 1000;
						Double dir = spdDir % 1000;
						sb->Append((const UTF8Char*)"\r\nSpeed = ");
						Text::SBAppendF64(sb, spd);
						sb->Append((const UTF8Char*)" knot\r\nDirection = ");
						Text::SBAppendF64(sb, dir);
						sb->Append((const UTF8Char*)"\r\nStatus1 = 0x");
						sb->AppendHex8(dataBuff[26]);
						sb->Append((const UTF8Char*)"\r\nStatus2 = 0x");
						sb->AppendHex8(dataBuff[27]);
						sb->Append((const UTF8Char*)"\r\nPeriod = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[28]));
						sb->Append((const UTF8Char*)"\r\nStatus3 = 0x");
						sb->AppendHex8(dataBuff[30]);
						sb->Append((const UTF8Char*)"\r\nStatus4 = 0x");
						sb->AppendHex8(dataBuff[31]);
						sb->Append((const UTF8Char*)"\r\nSatellite in use = ");
						sb->AppendU16(dataBuff[32]);
						sb->Append((const UTF8Char*)"\r\nSatellite in view = ");
						sb->AppendU16(dataBuff[33]);
						sb->Append((const UTF8Char*)"\r\nTemperature Valid = ");
						if (dataBuff[34] & 1)
						{
							Double t = -39.65 + 0.01 * ReadMUInt32(&dataBuff[35]);
							sb->Append((const UTF8Char*)"True\r\nTemperature = ");
							Text::SBAppendF64(sb, t);
							sb->Append((const UTF8Char*)"\r\nRH Valid = ");
							if (dataBuff[39] & 1)
							{
								UInt32 rh = ReadMUInt32(&dataBuff[40]);
								Double rhl = -2.0468 + 0.0367 * -(1.5955 * rh * rh) * 0.000001;
								sb->Append((const UTF8Char*)"True\r\nRH(Linear) = ");
								Text::SBAppendF64(sb, rhl);
								sb->Append((const UTF8Char*)"%\r\nRH (Refined) = ");
								Text::SBAppendF64(sb, (t - 25.0) * (0.01 + 0.00008 * rh) + rhl);
							}
							else
							{
								sb->Append((const UTF8Char*)"False");
							}
						}
						else
						{
							sb->Append((const UTF8Char*)"False");
						}
					}
				}
				break;
			case 0x6e:
				sb->Append((const UTF8Char*)", LED Tran Msg reply");
				break;
			case 0x72:
				sb->Append((const UTF8Char*)", LED Msg reply");
				break;
			case 0x74:
				sb->Append((const UTF8Char*)", LED Delete Msg reply");
				break;
			case 0x75:
				sb->Append((const UTF8Char*)", LED Notify Msg reply");
				break;
			case 0x77:
				sb->Append((const UTF8Char*)", Delete LED Notify Msg reply");
				break;
			case 0x7c:
				sb->Append((const UTF8Char*)", MDT message unicode echo");
				break;
			case 0x7d:
				sb->Append((const UTF8Char*)", Set MDT Phone reply");
				break;
			case 0x7f:
				sb->Append((const UTF8Char*)", Capture photo reply");
				if (detail)
				{
					if (cmdSize == 13)
					{
						sb->Append((const UTF8Char*)", Size = ");
						sb->AppendU32(ReadMUInt24(&dataBuff[9]));
					}
				}
				break;
			case 0x80:
				sb->Append((const UTF8Char*)", Photo packet");
				if (detail)
				{
					sb->Append((const UTF8Char*)", Packet ID = ");
					sb->AppendU16(ReadMUInt16(&dataBuff[9]));
					sb->Append((const UTF8Char*)", Data Size = ");
					sb->AppendU16(ReadMUInt16(&dataBuff[11]));
					sb->Append((const UTF8Char*)", CheckDigits = ");
					sb->AppendHex16(ReadMUInt16(&dataBuff[cmdSize - 3]));
				}
				break;
			case 0x81:
				sb->Append((const UTF8Char*)", Photo end reply");
				break;
			case 0x8c:
				sb->Append((const UTF8Char*)", Get Server time");
				break;
			case 0x8d:
				sb->Append((const UTF8Char*)", Get Device Battery");
				if (detail)
				{
					if (cmdSize == 23)
					{
						Data::DateTime dt;
						dt.SetValue((UInt16)(Data::ByteTool::GetBCD8(dataBuff[14]) + 2000), Data::ByteTool::GetBCD8(dataBuff[13]), Data::ByteTool::GetBCD8(dataBuff[12]), Data::ByteTool::GetBCD8(dataBuff[9]), Data::ByteTool::GetBCD8(dataBuff[10]), Data::ByteTool::GetBCD8(dataBuff[11]), 0, (Int8)((Int8)dataBuff[15] * 4));
						sb->Append((const UTF8Char*)"\r\nDevice Time = ");
						sb->AppendDate(&dt);
						sb->Append((const UTF8Char*)"\r\nBattery Status = 0x");
						sb->AppendHex8(dataBuff[16]);
						sb->Append((const UTF8Char*)"\r\nBattery Level = ");
						sb->AppendU16(dataBuff[17]);
						sb->Append((const UTF8Char*)"\r\nRSSI = ");
						sb->AppendU16(dataBuff[18]);
						sb->Append((const UTF8Char*)"\r\nDevice Status = 0x");
						sb->AppendHex8(dataBuff[19]);
						sb->Append((const UTF8Char*)"\r\nCard Space = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[20]));
						sb->Append((const UTF8Char*)"MB");
					}
				}
				break;
			case 0x91:
				sb->Append((const UTF8Char*)", Wireless Sensor Data");
				if (detail)
				{
					if (cmdSize == 43)
					{
						sb->Append((const UTF8Char*)"\r\nChannel = ");
						sb->AppendU16(dataBuff[9]);
						sb->Append((const UTF8Char*)"\r\nRSSI = ");
						sb->AppendU16(dataBuff[10]);
						sb->Append((const UTF8Char*)"\r\nSensor IP = ");
						sb->AppendU16(dataBuff[11]);
						sb->AppendChar('.', 1);
						sb->AppendU16(dataBuff[12]);
						sb->AppendChar('.', 1);
						sb->AppendU16(dataBuff[13]);
						sb->AppendChar('.', 1);
						sb->AppendU16(dataBuff[14]);
						sb->Append((const UTF8Char*)"\r\nSensor Port = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[15]));
						sb->Append((const UTF8Char*)"\r\nSensor ID = ");
						sb->AppendHexBuff(&dataBuff[17], 5, 0, Text::LBT_NONE);
						Data::DateTime t;
						sb->Append((const UTF8Char*)"\r\nGPS Time = ");
						t.SetValue((UInt16)(Data::ByteTool::GetBCD8(dataBuff[27]) + 2000), Data::ByteTool::GetBCD8(dataBuff[26]), Data::ByteTool::GetBCD8(dataBuff[25]), Data::ByteTool::GetBCD8(dataBuff[22]), Data::ByteTool::GetBCD8(dataBuff[23]), Data::ByteTool::GetBCD8(dataBuff[24]), 0, 0);
						sb->AppendDate(&t);
						sb->Append((const UTF8Char*)"\r\nStatus1 = 0x");
						sb->AppendHex8(dataBuff[28]);
						sb->Append((const UTF8Char*)"\r\nTemperature = ");
						if (dataBuff[28] & 4)
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[29]) * 0.0001);
						}
						else
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[29]) * 0.01 - 39.65);
						}
						sb->Append((const UTF8Char*)"\r\nStatus2 = 0x");
						sb->AppendHex8(dataBuff[33]);
						sb->Append((const UTF8Char*)"\r\nHumidity = ");
						if (dataBuff[33] & 2)
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[34]) * 0.01);
						}
						else
						{
							Double y = ReadMInt32(&dataBuff[34]);
							Text::SBAppendF64(sb, -2.0468 + 0.0367 * y -(1.5955 * y * y) / 1000000);
						}
						sb->Append((const UTF8Char*)"\r\nBattery = ");
						Text::SBAppendF64(sb, dataBuff[38] / 128.0 * 3.28);
						sb->Append((const UTF8Char*)"\r\nStatus3 = 0x");
						sb->AppendHex8(dataBuff[39]);
						sb->Append((const UTF8Char*)"\r\nPeriod = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[40]));
					}
					else if (cmdSize == 58)
					{
						sb->Append((const UTF8Char*)"\r\nChannel = ");
						sb->AppendU16(dataBuff[9]);
						sb->Append((const UTF8Char*)"\r\nRSSI = ");
						sb->AppendU16(dataBuff[10]);
						sb->Append((const UTF8Char*)"\r\nSensor IP = ");
						sb->AppendU16(dataBuff[11]);
						sb->AppendChar('.', 1);
						sb->AppendU16(dataBuff[12]);
						sb->AppendChar('.', 1);
						sb->AppendU16(dataBuff[13]);
						sb->AppendChar('.', 1);
						sb->AppendU16(dataBuff[14]);
						sb->Append((const UTF8Char*)"\r\nSensor Port = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[15]));
						sb->Append((const UTF8Char*)"\r\nSensor ID = ");
						sb->AppendHexBuff(&dataBuff[17], 5, 0, Text::LBT_NONE);
						Data::DateTime t;
						sb->Append((const UTF8Char*)"\r\nGPS Time = ");
						t.SetValue((UInt16)(Data::ByteTool::GetBCD8(dataBuff[27]) + 2000), Data::ByteTool::GetBCD8(dataBuff[26]), Data::ByteTool::GetBCD8(dataBuff[25]), Data::ByteTool::GetBCD8(dataBuff[22]), Data::ByteTool::GetBCD8(dataBuff[23]), Data::ByteTool::GetBCD8(dataBuff[24]), 0, 0);
						sb->AppendDate(&t);
						sb->Append((const UTF8Char*)"\r\nStatus1 = 0x");
						sb->AppendHex8(dataBuff[28]);
						sb->Append((const UTF8Char*)"\r\nTemperature = ");
						if (dataBuff[28] & 4)
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[29]) * 0.0001);
						}
						else
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[29]) * 0.01 - 39.65);
						}
						sb->Append((const UTF8Char*)"\r\nStatus2 = 0x");
						sb->AppendHex8(dataBuff[33]);
						sb->Append((const UTF8Char*)"\r\nHumidity = ");
						if (dataBuff[33] & 2)
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[34]) * 0.01);
						}
						else
						{
							Double y = ReadMInt32(&dataBuff[34]);
							Text::SBAppendF64(sb, -2.0468 + 0.0367 * y -(1.5955 * y * y) / 1000000);
						}
						sb->Append((const UTF8Char*)"\r\nBattery = ");
						Text::SBAppendF64(sb, dataBuff[38] / 128.0 * 3.28);
						sb->Append((const UTF8Char*)"\r\nStatus3 = 0x");
						sb->AppendHex8(dataBuff[39]);
						sb->Append((const UTF8Char*)"\r\nPeriod = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[40]));
						sb->Append((const UTF8Char*)"\r\nStatus4 = 0x");
						sb->AppendHex8(dataBuff[42]);
						sb->Append((const UTF8Char*)"\r\nPosition Status = 0x");
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
						sb->Append((const UTF8Char*)"\r\nLatitude = ");
						Text::SBAppendF64(sb, lat);
						sb->Append((const UTF8Char*)"\r\nLongitude = ");
						Text::SBAppendF64(sb, lon);
						sb->Append((const UTF8Char*)"\r\nAltitude = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[52]));
						Int32 spdDir = Data::ByteTool::GetBCD8(dataBuff[54]) * 10000 + Data::ByteTool::GetBCD8(dataBuff[55]) * 100 + Data::ByteTool::GetBCD8(dataBuff[56]);
						Double spd = spdDir / 1000;
						Double dir = spdDir % 1000;
						sb->Append((const UTF8Char*)"\r\nSpeed = ");
						Text::SBAppendF64(sb, spd);
						sb->Append((const UTF8Char*)" knot\r\nDirection = ");
						Text::SBAppendF64(sb, dir);
					}
				}
				break;
			case 0x93:
				sb->Append((const UTF8Char*)", Wireless Sensor with Repeater Data");
				if (detail)
				{
					if (cmdSize >= 52)
					{
						sb->Append((const UTF8Char*)"\r\nChannel = ");
						sb->AppendU16(dataBuff[9]);
						sb->Append((const UTF8Char*)"\r\nRSSI = ");
						sb->AppendU16(dataBuff[10]);
						sb->Append((const UTF8Char*)"\r\nSensor IP = ");
						sb->AppendU16(dataBuff[11]);
						sb->AppendChar('.', 1);
						sb->AppendU16(dataBuff[12]);
						sb->AppendChar('.', 1);
						sb->AppendU16(dataBuff[13]);
						sb->AppendChar('.', 1);
						sb->AppendU16(dataBuff[14]);
						sb->Append((const UTF8Char*)"\r\nSensor Port = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[15]));
						sb->Append((const UTF8Char*)"\r\nSensor ID = ");
						sb->AppendHexBuff(&dataBuff[17], 5, 0, Text::LBT_NONE);
						Data::DateTime t;
						sb->Append((const UTF8Char*)"\r\nGPS Time = ");
						t.SetValue((UInt16)(Data::ByteTool::GetBCD8(dataBuff[27]) + 2000), Data::ByteTool::GetBCD8(dataBuff[26]), Data::ByteTool::GetBCD8(dataBuff[25]), Data::ByteTool::GetBCD8(dataBuff[22]), Data::ByteTool::GetBCD8(dataBuff[23]), Data::ByteTool::GetBCD8(dataBuff[24]), 0, 0);
						sb->AppendDate(&t);
						sb->Append((const UTF8Char*)"\r\nStatus1 = 0x");
						sb->AppendHex8(dataBuff[28]);
						sb->Append((const UTF8Char*)"\r\nTemperature = ");
						if (dataBuff[28] & 4)
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[29]) * 0.0001);
						}
						else
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[29]) * 0.01 - 39.65);
						}
						sb->Append((const UTF8Char*)"\r\nStatus2 = 0x");
						sb->AppendHex8(dataBuff[33]);
						sb->Append((const UTF8Char*)"\r\nHumidity = ");
						if (dataBuff[33] & 2)
						{
							Text::SBAppendF64(sb, ReadMInt32(&dataBuff[34]) * 0.01);
						}
						else
						{
							Double y = ReadMInt32(&dataBuff[34]);
							Text::SBAppendF64(sb, -2.0468 + 0.0367 * y -(1.5955 * y * y) / 1000000);
						}
						sb->Append((const UTF8Char*)"\r\nBattery = ");
						Text::SBAppendF64(sb, dataBuff[38] / 128.0 * 3.28);
						sb->Append((const UTF8Char*)"\r\nStatus3 = 0x");
						sb->AppendHex8(dataBuff[39]);
						sb->Append((const UTF8Char*)"\r\nPeriod = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[40]));
						//////////////////////////////
					}
				}
				break;
			default:
				sb->Append((const UTF8Char*)", Unknown");
				break;
			}
		}
		else
		{
			sb->Append((const UTF8Char*)"Unknown protocol");
		}
	}
	else if (buffSize >= 10 && dataBuff[0] == 0xfe && dataBuff[buffSize - 1] == 0xfe)
	{
		UInt32 cmdSize = (UInt32)(dataBuff[1] << 8) | dataBuff[2];
		Int64 termId = Data::ByteTool::GetBCD8(dataBuff[4]) * 100000000LL + Data::ByteTool::GetBCD32(&dataBuff[5]);
		if (cmdSize <= buffSize)
		{
			sb->Append((const UTF8Char*)"SP1000SC: ");
			sb->AppendI64(termId);
			switch (dataBuff[3])
			{
			case 1:
				sb->Append((const UTF8Char*)", Register reply");
				if (detail)
				{
					if (cmdSize == 13)
					{
						sb->Append((const UTF8Char*)"\r\nACK = ");
						sb->AppendU16(dataBuff[9]);
						if (dataBuff[9] == 1)
						{
							sb->Append((const UTF8Char*)" (Success)");
						}
						else if (dataBuff[9] == 0)
						{
							sb->Append((const UTF8Char*)" (Failed)");
						}
						sb->Append((const UTF8Char*)"\r\nPeriod = ");
						sb->AppendU16((UInt16)((dataBuff[10] << 8) | dataBuff[11]));
						sb->Append((const UTF8Char*)"s");
					}
				}
				break;
			case 7:
				sb->Append((const UTF8Char*)", Check Parameter");
				break;
			case 0xc:
				sb->Append((const UTF8Char*)", Cancel SOS Alert");
				break;
			case 0x1a:
				sb->Append((const UTF8Char*)", Set Basic Parameters");
				if (detail)
				{
					if (cmdSize == 37)
					{
						sb->Append((const UTF8Char*)"\r\nServer IP = ");
						Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&dataBuff[9]));
						sb->Append(sbuff);
						sb->Append((const UTF8Char*)"\r\nServer Port = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[13]));
						sb->Append((const UTF8Char*)"\r\nNew Device ID = ");
						sb->AppendC((const UTF8Char*)&dataBuff[15], 10);
						sb->Append((const UTF8Char*)"\r\nAPN = ");
						sb->AppendC((const UTF8Char*)&dataBuff[25], 11);
					}
					else if (cmdSize == 56)
					{
						sb->Append((const UTF8Char*)"\r\nServer IP = ");
						Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&dataBuff[9]));
						sb->Append(sbuff);
						sb->Append((const UTF8Char*)"\r\nServer Port = ");
						sb->AppendU16(ReadMUInt16(&dataBuff[13]));
						sb->Append((const UTF8Char*)"\r\nNew Device ID = ");
						sb->AppendC((const UTF8Char*)&dataBuff[15], 10);
						sb->Append((const UTF8Char*)"\r\nAPN = ");
						sb->AppendC((const UTF8Char*)&dataBuff[25], 30);
					}
				}
				break;
			default:
				sb->Append((const UTF8Char*)", Unknown");
				break;
			}
		}
		else
		{
			sb->Append((const UTF8Char*)"Unknown protocol");
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
		sb->Append((const UTF8Char*)"Unknown protocol");
	}
	return true;
}

IO::UDPLog::UDPLog(const UTF8Char *sourceName) : IO::LogFile(sourceName)
{
}

IO::UDPLog::~UDPLog()
{
}
