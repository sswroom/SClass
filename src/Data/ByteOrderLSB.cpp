#include "Stdafx.h"
#include "Data/ByteOrderLSB.h"
#include "Data/ByteTool.h"

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

Int16 Data::ByteOrderLSB::GetInt16(const UInt8 *pVal) const
{
	return ReadInt16(pVal);
}

UInt16 Data::ByteOrderLSB::GetUInt16(const UInt8 *pVal) const
{
	return ReadUInt16(pVal);
}

Int32 Data::ByteOrderLSB::GetInt32(const UInt8 *pVal) const
{
	return ReadInt32(pVal);
}

UInt32 Data::ByteOrderLSB::GetUInt32(const UInt8 *pVal) const
{
	return ReadUInt32(pVal);
}

Int64 Data::ByteOrderLSB::GetInt64(const UInt8 *pVal) const
{
	return ReadInt64(pVal);
}

UInt64 Data::ByteOrderLSB::GetUInt64(const UInt8 *pVal) const
{
	return ReadUInt64(pVal);
}

Single Data::ByteOrderLSB::GetFloat16(const UInt8 *pVal) const
{
	UInt32 v = ReadUInt16(pVal);
	v = ((v & 0xc000) << 16) | 0x38000000 | ((v & 0x3fff) << 13);
	return *(Single*)&v;
}

Single Data::ByteOrderLSB::GetFloat24(const UInt8 *pVal) const
{
	UInt32 v = ReadUInt24(pVal);
	v = ((v & 0xc00000) << 8) | 0x20000000 | ((v & 0x3fffff) << 7);
	return *(Single*)&v;
}

Single Data::ByteOrderLSB::GetFloat32(const UInt8 *pVal) const
{
	return ReadFloat(pVal);
}

Double Data::ByteOrderLSB::GetFloat64(const UInt8 *pVal) const
{
	return ReadDouble(pVal);
}

void Data::ByteOrderLSB::SetInt32(UInt8 *pVal, Int32 v) const
{
	WriteInt32(pVal, v);
}

void Data::ByteOrderLSB::SetInt16(UInt8 *pVal, Int16 v) const
{
	WriteInt16(pVal, v);
}
