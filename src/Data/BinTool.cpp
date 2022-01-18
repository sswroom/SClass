#include "Stdafx.h"
#include "Data/BinTool.h"
#include "Data/ByteTool.h"

Bool Data::BinTool::Equals(UInt8 *buff1, UInt8 *buff2, UOSInt len)
{
	while (len >= 4)
	{
		if (ReadNInt32(buff1) != ReadNInt32(buff2))
			return false;
		len -= 4;
		buff1 += 4;
		buff2 += 4;
	}
	switch (len)
	{
	case 3:
		return (ReadNInt16(buff1) == ReadNInt16(buff2)) && (buff1[2] == buff2[2]);
	case 2:
		return ReadNInt16(buff1) == ReadNInt16(buff2);
	case 1:
		return buff1[0] == buff2[0];
	default:
		return true;
	}
}
