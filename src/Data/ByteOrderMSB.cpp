#include "Stdafx.h"
#include "Data/ByteOrderMSB.h"
#include "Core/ByteTool_C.h"

Data::ByteOrderMSB::ByteOrderMSB()
{
}

Data::ByteOrderMSB::~ByteOrderMSB()
{
}

Bool Data::ByteOrderMSB::IsBigEndian()
{
	return true;
}

Int16 Data::ByteOrderMSB::GetInt16(UnsafeArray<const UInt8> pVal) const
{
	return ReadMInt16(pVal.Ptr());
}

UInt16 Data::ByteOrderMSB::GetUInt16(UnsafeArray<const UInt8> pVal) const
{
	return ReadMUInt16(pVal.Ptr());
}

Int32 Data::ByteOrderMSB::GetInt32(UnsafeArray<const UInt8> pVal) const
{
	return ReadMInt32(pVal.Ptr());
}

UInt32 Data::ByteOrderMSB::GetUInt32(UnsafeArray<const UInt8> pVal) const
{
	return ReadMUInt32(pVal.Ptr());
}

Int64 Data::ByteOrderMSB::GetInt64(UnsafeArray<const UInt8> pVal) const
{
	return ReadMInt64(pVal.Ptr());
}

UInt64 Data::ByteOrderMSB::GetUInt64(UnsafeArray<const UInt8> pVal) const
{
	return ReadMUInt64(pVal.Ptr());
}

Single Data::ByteOrderMSB::GetFloat16(UnsafeArray<const UInt8> pVal) const
{
	UInt32 v = ReadMUInt16(pVal.Ptr());
	v = ((v & 0xc000) << 16) | 0x38000000 | ((v & 0x3fff) << 13);
	return *(Single*)&v;
}

Single Data::ByteOrderMSB::GetFloat24(UnsafeArray<const UInt8> pVal) const
{
	UInt32 v = ReadMUInt24(pVal.Ptr());
	v = ((v & 0xc00000) << 8) | 0x20000000 | ((v & 0x3fffff) << 7);
	return *(Single*)&v;
}

Single Data::ByteOrderMSB::GetFloat32(UnsafeArray<const UInt8> pVal) const
{
	return ReadMFloat(pVal.Ptr());
}

Double Data::ByteOrderMSB::GetFloat64(UnsafeArray<const UInt8> pVal) const
{
	return ReadMDouble(pVal.Ptr());
}

void Data::ByteOrderMSB::SetInt32(UnsafeArray<UInt8> pVal, Int32 v) const
{
	WriteMInt32(&pVal[0], v);
}

void Data::ByteOrderMSB::SetInt16(UnsafeArray<UInt8> pVal, Int16 v) const
{
	WriteMInt16(&pVal[0], v);
}
