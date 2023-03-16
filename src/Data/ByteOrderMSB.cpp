#include "Stdafx.h"
#include "Data/ByteOrderMSB.h"
#include "Data/ByteTool.h"

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

Int16 Data::ByteOrderMSB::GetInt16(const UInt8 *pVal) const
{
	return ReadMInt16(pVal);
}

UInt16 Data::ByteOrderMSB::GetUInt16(const UInt8 *pVal) const
{
	return ReadMUInt16(pVal);
}

Int32 Data::ByteOrderMSB::GetInt32(const UInt8 *pVal) const
{
	return ReadMInt32(pVal);
}

UInt32 Data::ByteOrderMSB::GetUInt32(const UInt8 *pVal) const
{
	return ReadMUInt32(pVal);
}

Int64 Data::ByteOrderMSB::GetInt64(const UInt8 *pVal) const
{
	return ReadMInt64(pVal);
}

UInt64 Data::ByteOrderMSB::GetUInt64(const UInt8 *pVal) const
{
	return ReadMUInt64(pVal);
}

Single Data::ByteOrderMSB::GetFloat16(const UInt8 *pVal) const
{
	UInt32 v = ReadMUInt16(pVal);
	v = ((v & 0xc000) << 16) | 0x38000000 | ((v & 0x3fff) << 13);
	return *(Single*)&v;
}

Single Data::ByteOrderMSB::GetFloat24(const UInt8 *pVal) const
{
	UInt32 v = ReadMUInt24(pVal);
	v = ((v & 0xc00000) << 8) | 0x20000000 | ((v & 0x3fffff) << 7);
	return *(Single*)&v;
}

Single Data::ByteOrderMSB::GetFloat32(const UInt8 *pVal) const
{
	return ReadMFloat(pVal);
}

Double Data::ByteOrderMSB::GetFloat64(const UInt8 *pVal) const
{
	return ReadMDouble(pVal);
}

void Data::ByteOrderMSB::SetInt32(UInt8 *pVal, Int32 v) const
{
	WriteMInt32(pVal, v);
}

void Data::ByteOrderMSB::SetInt16(UInt8 *pVal, Int16 v) const
{
	WriteMInt16(pVal, v);
}
