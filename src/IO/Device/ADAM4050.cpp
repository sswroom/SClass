#include "Stdafx.h"
#include "IO/Device/ADAM4050.h"

Bool IO::Device::ADAM4050::GetIOStatus(UInt8 addr, UInt16 *outputs, UInt16 *inputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '6';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 7)
	{
		*outputs = Text::StrHex2UInt8C(&sbuff[1]);
		*inputs = Text::StrHex2UInt8C(&sbuff[3]);
		return true;
	}
	return false;
}
