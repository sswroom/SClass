#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"

#define DIFFRATIO 0.0000001
Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt16 iVal;
	Double dVal;
	iVal = 0x0001;
	dVal = ReadNFloat16((UInt8*)&iVal);
	if (!Math::NearlyEquals(dVal, 0.00000005960464453215, DIFFRATIO))
	{
		return 1;
	}

	iVal = 0x03ff;
	dVal = ReadNFloat16((UInt8*)&iVal);
	if (!Math::NearlyEquals(dVal, 0.000060975551355468752, DIFFRATIO))
	{
		return 1;
	}

	iVal = 0x0400;
	dVal = ReadNFloat16((UInt8*)&iVal);
	if (!Math::NearlyEquals(dVal, 0.00006103515625, DIFFRATIO))
	{
		return 1;
	}

	iVal = 0x7bff;
	dVal = ReadNFloat16((UInt8*)&iVal);
	if (!Math::NearlyEquals(dVal, 65504, DIFFRATIO))
	{
		return 1;
	}

	iVal = 0x3bff;
	dVal = ReadNFloat16((UInt8*)&iVal);
	if (!Math::NearlyEquals(dVal, 0.99951171875, DIFFRATIO))
	{
		return 1;
	}

	iVal = 0x3c00;
	dVal = ReadNFloat16((UInt8*)&iVal);
	if (!Math::NearlyEquals(dVal, 1, DIFFRATIO))
	{
		return 1;
	}

	iVal = 0x3c01;
	dVal = ReadNFloat16((UInt8*)&iVal);
	if (!Math::NearlyEquals(dVal, 1.00097656, DIFFRATIO))
	{
		return 1;
	}

	iVal = 0x3555;
	dVal = ReadNFloat16((UInt8*)&iVal);
	if (!Math::NearlyEquals(dVal, 0.33325195, DIFFRATIO))
	{
		return 1;
	}

	iVal = 0xc000;
	dVal = ReadNFloat16((UInt8*)&iVal);
	if (!Math::NearlyEquals(dVal, -2, DIFFRATIO))
	{
		return 1;
	}

	iVal = 0x0000;
	dVal = ReadNFloat16((UInt8*)&iVal);
	if (dVal != 0)
	{
		return 1;
	}

	iVal = 0x8000;
	dVal = ReadNFloat16((UInt8*)&iVal);
	if (dVal != 0 || !Math::IsNeg(dVal))
	{
		return 1;
	}

	iVal = 0x7c00;
	dVal = ReadNFloat16((UInt8*)&iVal);
	if (!Math::IsInfinity(dVal) || Math::IsNeg(dVal))
	{
		return 1;
	}

	iVal = 0xfc00;
	dVal = ReadNFloat16((UInt8*)&iVal);
	if (!Math::IsInfinity(dVal) || !Math::IsNeg(dVal))
	{
		return 1;
	}
	return 0;
}

