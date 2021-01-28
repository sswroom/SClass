#include "Stdafx.h"
#include "Data/ByteTool.h"

extern "C" UInt32 SuperFastHash_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 currVal)
{
	UInt32 tmp;
    UOSInt rem;

    rem = buffSize & 3;
    buffSize >>= 2;

	while (buffSize-- > 0)
	{
		currVal += ReadMUInt16(buff);
		tmp = (ReadMUInt16(buff + 2) << 11) ^ currVal;
        currVal = (currVal << 16) ^ tmp;
        buff += 4;
        currVal += currVal >> 11;
    }

	switch (rem)
	{
	case 3:
		currVal += ReadMUInt16(buff);
		currVal ^= currVal << 16;
		currVal ^= buff[2] << 18;
		currVal += currVal >> 11;
		break;
	case 2:
		currVal += ReadMUInt16(buff);
		currVal ^= currVal << 11;
		currVal += currVal >> 17;
		break;
	case 1:
		currVal += buff[0];
		currVal ^= currVal << 10;
		currVal += currVal >> 1;
		break;
	}
	
	return currVal;
}
