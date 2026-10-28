#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "IO/BTAdvData.h"
#include "IO/BTUtil.h"

Bool IO::BTAdvData::ToString(UnsafeArray<const UInt8> advData, NN<Text::StringBuilderUTF8> sb)
{
	UInt8 len = advData[0];
	UInt8 type = advData[1];
	if (type == 0xff && len >= 3)
	{
		UInt16 company = ReadLUInt16(&advData[2]);
		if (company == 0x4C) //Apple
		{
			UIntOS i = 4;
			while (i < (UIntOS)len + 1)
			{
				if (i != 4)
				{
					sb->Append(CSTR(", "));
				}
				UInt8 subType = advData[i];
				UInt8 subLen = advData[i + 1];
				if (i + 1 + subLen > (UIntOS)len + 1)
				{
					sb->Append(CSTR("Invalid sub-length{"));
					sb->AppendHexBuff(&advData[i], len + 1 - i, ' ', Text::LineBreakType::None);
					sb->AppendUTF8Char('}');
					break;
				}
				if (subType == 2 && subLen == 21) //iBeacon
				{
					sb->Append(CSTR("iBeacon{UUID="));
					Data::UUID uuid(&advData[i + 2]);
					uuid.ToString(sb);
					sb->Append(CSTR(", Major="));
					sb->AppendU16(ReadLUInt16(&advData[i + 18]));
					sb->Append(CSTR(", Minor="));
					sb->AppendU16(ReadLUInt16(&advData[i + 20]));
					sb->Append(CSTR("}"));
				}
				else if (subType == 3) //AirPrint
				{
					sb->Append(CSTR("AirPrint{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}
				else if (subType == 5) //AirDrop
				{
					sb->Append(CSTR("AirDrop{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}
				else if (subType == 6) //HomeKit
				{
					sb->Append(CSTR("HomeKit{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}
				else if (subType == 7) //ProximityPairing
				{
					sb->Append(CSTR("ProximityPairing{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}
				else if (subType == 9) //AirPlayTarget
				{
					sb->Append(CSTR("AirPlayTarget{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}
				else if (subType == 10) //AirPlaySrc
				{
					sb->Append(CSTR("AirPlaySrc{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}
				else if (subType == 11) //MagicSwitch
				{
					sb->Append(CSTR("MagicSwitch{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}
				else if (subType == 12) //Handoff
				{
					sb->Append(CSTR("Handoff{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}
				else if (subType == 13) //TetheringTarget
				{
					sb->Append(CSTR("TetheringTarget{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}
				else if (subType == 14) //TetheringSrc
				{
					sb->Append(CSTR("TetheringSrc{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}
				else if (subType == 15) //NearbyAction
				{
					sb->Append(CSTR("NearbyAction{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}
				else if (subType == 16) //NearbyInfo
				{
					sb->Append(CSTR("NearbyInfo{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}
				else if (subType == 18) //FindMyBroadcast
				{
					sb->Append(CSTR("FindMyBroadcast{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}
				else
				{
					sb->Append(CSTR("Unknown{"));
					sb->AppendHexBuff(&advData[i + 2], subLen - 1, ' ', Text::LineBreakType::None);
					sb->Append(CSTR("}"));
				}

				i += subLen + 2;
			}
			return true;
		}
		else if (company == 0x56) //Sony
		{
			return false;
		}
	}
	return false;
}
