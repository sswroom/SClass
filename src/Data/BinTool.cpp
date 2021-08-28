#include "Stdafx.h"
#include "Data/BinTool.h"

Bool Data::BinTool::Equals(UInt8 *buff1, UInt8 *buff2, UOSInt len)
{
	while (len-- > 0)
		if (*buff1++ != *buff2++) return false;
	return true;
}
