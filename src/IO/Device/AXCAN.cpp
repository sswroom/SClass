#include "Stdafx.h"
#include "IO/Device/AXCAN.h"

IO::Device::AXCAN::AXCAN(CANHandler *hdlr)
{
	this->hdlr = hdlr;
}

IO::Device::AXCAN::~AXCAN()
{

}

void IO::Device::AXCAN::ParseReader(IO::Reader *reader)
{
	Text::StringBuilderUTF8 sb;
	UInt8 buff[16];
	while (reader->ReadLine(&sb, 1024))
	{
		if(sb.StartsWith(UTF8STRC("@F")) && sb.leng > 7)
		{
			UInt32 id;
			UInt8 len;
			Bool rtr;
			if (sb.v[6] == '1' && sb.leng >= 17)
			{
				id = Text::StrHex2UInt32C(&sb.v[7]);
				rtr = sb.v[15] != '0';
				len = (UInt8)(Text::StrHex2UInt8C(&sb.v[15]) & 15);
				if (sb.leng == (UOSInt)len * 2 + 17 && Text::StrHex2Bytes(&sb.v[17], buff) == len)
				{
					this->hdlr->CANMessage(id, rtr, buff, len);
				}
			}
			else if (sb.v[6] == '0' && sb.leng >= 12)
			{
				id = Text::StrHex2UInt16C(&sb.v[6]);
				rtr = sb.v[10] != '0';
				len = (UInt8)(Text::StrHex2UInt8C(&sb.v[10]) & 15);
				if (sb.leng == (UOSInt)len * 2 + 12 && Text::StrHex2Bytes(&sb.v[12], buff) == len)
				{
					this->hdlr->CANMessage(id, rtr, buff, len);
				}
			}
		}
		sb.ClearStr();
	}
}
