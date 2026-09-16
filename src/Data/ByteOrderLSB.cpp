#include "Stdafx.h"
#include "Data/ByteOrderLSB.h"
#include "Core/ByteTool_C.h"

Data::ByteOrderLSB::ByteOrderLSB()
{
}

Data::ByteOrderLSB::~ByteOrderLSB()
{
}

Bool Data::ByteOrderLSB::IsBigEndian()
{
	return false;
}

Int16 Data::ByteOrderLSB::GetInt16(UnsafeArray<const UInt8> pVal) const
{
	return ReadLInt16(pVal.Ptr());
}

UInt16 Data::ByteOrderLSB::GetUInt16(UnsafeArray<const UInt8> pVal) const
{
	return ReadLUInt16(pVal.Ptr());
}

Int32 Data::ByteOrderLSB::GetInt32(UnsafeArray<const UInt8> pVal) const
{
	return ReadLInt32(pVal.Ptr());
}

UInt32 Data::ByteOrderLSB::GetUInt32(UnsafeArray<const UInt8> pVal) const
{
	return ReadLUInt32(pVal.Ptr());
}

Int64 Data::ByteOrderLSB::GetInt64(UnsafeArray<const UInt8> pVal) const
{
	return ReadLInt64(pVal.Ptr());
}

UInt64 Data::ByteOrderLSB::GetUInt64(UnsafeArray<const UInt8> pVal) const
{
	return ReadLUInt64(pVal.Ptr());
}

Single Data::ByteOrderLSB::GetFloat16(UnsafeArray<const UInt8> pVal) const
{
	UInt32 v = ReadLUInt16(pVal.Ptr());
	v = ((v & 0xc000) << 16) | 0x38000000 | ((v & 0x3fff) << 13);
	return *(Single*)&v;
}

Single Data::ByteOrderLSB::GetFloat24(UnsafeArray<const UInt8> pVal) const
{
	UInt32 v = ReadLUInt24(pVal.Ptr());
	v = ((v & 0xc00000) << 8) | 0x20000000 | ((v & 0x3fffff) << 7);
	return *(Single*)&v;
}

Single Data::ByteOrderLSB::GetFloat32(UnsafeArray<const UInt8> pVal) const
{
	return ReadLFloat(pVal.Ptr());
}

Double Data::ByteOrderLSB::GetFloat64(UnsafeArray<const UInt8> pVal) const
{
	return ReadLDouble(pVal.Ptr());
}

void Data::ByteOrderLSB::SetInt32(UnsafeArray<UInt8> pVal, Int32 v) const
{
	WriteLInt32(&pVal[0], v);
}

void Data::ByteOrderLSB::SetInt16(UnsafeArray<UInt8> pVal, Int16 v) const
{
	WriteLInt16(&pVal[0], v);
}
