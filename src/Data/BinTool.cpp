#include "Stdafx.h"
#include "Data/BinTool.h"
#include "Data/ByteTool.h"

Bool Data::BinTool::Equals(UnsafeArray<const UInt8> buff1, UnsafeArray<const UInt8> buff2, UOSInt len)
{
#if _OSINT_SIZE == 64
	while (len >= 8)
	{
		if (ReadNInt64(&buff1[0]) != ReadNInt64(&buff2[0]))
			return false;
		len -= 8;
		buff1 += 8;
		buff2 += 8;
	}
	switch (len)
	{
	case 7:
		return (ReadNInt32(&buff1[0]) == ReadNInt32(&buff2[0])) && (ReadNInt16(&buff1[4]) == ReadNInt16(&buff2[4])) && (buff1[6] == buff2[6]);
	case 6:
		return (ReadNInt32(&buff1[0]) == ReadNInt32(&buff2[0])) && (ReadNInt16(&buff1[4]) == ReadNInt16(&buff2[4]));
	case 5:
		return (ReadNInt32(&buff1[0]) == ReadNInt32(&buff2[0])) && (buff1[4] == buff2[4]);
	case 4:
		return (ReadNInt32(&buff1[0]) == ReadNInt32(&buff2[0]));
	case 3:
		return (ReadNInt16(&buff1[0]) == ReadNInt16(&buff2[0])) && (buff1[2] == buff2[2]);
	case 2:
		return ReadNInt16(&buff1[0]) == ReadNInt16(&buff2[0]);
	case 1:
		return buff1[0] == buff2[0];
	default:
		return true;
	}
#else
	while (len >= 4)
	{
		if (ReadNInt32(&buff1[0]) != ReadNInt32(&buff2[0]))
			return false;
		len -= 4;
		buff1 += 4;
		buff2 += 4;
	}
	switch (len)
	{
	case 3:
		return (ReadNInt16(&buff1[0]) == ReadNInt16(&buff2[0])) && (buff1[2] == buff2[2]);
	case 2:
		return ReadNInt16(&buff1[0]) == ReadNInt16(&buff2[0]);
	case 1:
		return buff1[0] == buff2[0];
	default:
		return true;
	}
#endif
}
