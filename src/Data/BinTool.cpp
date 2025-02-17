#include "Stdafx.h"
#include "Data/BinTool.h"
#include "Data/ByteTool.h"

Bool Data::BinTool::Equals(UnsafeArray<const UInt8> buff1, UnsafeArray<const UInt8> buff2, UOSInt len)
{
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
}
