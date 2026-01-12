#include "Stdafx.h"
#include "IO/ProtocolBuffersUtil.h"

Int64 IO::ProtocolBuffersUtil::ToSInt64(UInt64 val)
{
	return (Int64)((val >> 1) ^ (~(val & 1) + 1));
}

UOSInt IO::ProtocolBuffersUtil::ReadVarUInt(UnsafeArray<const UInt8> buff, UOSInt buffOfst, OutParam<UInt64> val)
{
	UInt64 outVal;
	UInt64 v;
	UOSInt shift = 0;
	outVal = 0;
	while (true)
	{
		v = buff[buffOfst];
		outVal |= (v & 0x7f) << shift;
		buffOfst++;
		if ((v & 0x80) == 0)
		{
			break;
		}
		shift += 7;
	}
	val.Set(outVal);
	return buffOfst;
}

UOSInt IO::ProtocolBuffersUtil::ReadVarInt(UnsafeArray<const UInt8> buff, UOSInt buffOfst, OutParam<Int64> val)
{
	UInt64 outVal;
	UInt64 v;
	UOSInt shift = 0;
	outVal = 0;
	while (true)
	{
		v = buff[buffOfst];
		outVal |= (v & 0x7f) << shift;
		buffOfst++;
		shift += 7;
		if ((v & 0x80) == 0)
		{
			break;
		}
	}
	if (outVal & (1 << (shift - 1)))
	{
		outVal |= (~0ULL) << shift;
	}
	val.Set((Int64)outVal);
	return buffOfst;
}

UOSInt IO::ProtocolBuffersUtil::ReadVarSInt(UnsafeArray<const UInt8> buff, UOSInt buffOfst, OutParam<Int64> val)
{
	UInt64 outVal;
	UInt64 v;
	UOSInt shift = 0;
	outVal = 0;
	while (true)
	{
		v = buff[buffOfst];
		outVal |= (v & 0x7f) << shift;
		buffOfst++;
		if ((v & 0x80) == 0)
		{
			break;
		}
		shift += 7;
	}
	val.Set(ToSInt64(outVal));
	return buffOfst;
}

Text::CStringNN IO::ProtocolBuffersUtil::WireTypeGetName(UInt8 wireType)
{
	switch (wireType)
	{
	case 0:
		return CSTR("Varint");
	case 1:
		return CSTR("64-bit");
	case 2:
		return CSTR("Length-delimited");
	case 3:
		return CSTR("Start group");
	case 4:
		return CSTR("End group");
	case 5:
		return CSTR("32-bit");
	default:
		return CSTR("Unknown");
	}
}
