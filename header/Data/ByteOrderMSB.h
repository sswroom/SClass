#ifndef _SM_DATA_BYTEORDERMSB
#define _SM_DATA_BYTEORDERMSB
#include "Data/ByteOrder.h"

namespace Data
{
	class ByteOrderMSB : public ByteOrder
	{
	public:
		ByteOrderMSB();
		virtual ~ByteOrderMSB();

		virtual Bool IsBigEndian();
		virtual Int16 GetInt16(UnsafeArray<const UInt8> pVal) const;
		virtual UInt16 GetUInt16(UnsafeArray<const UInt8> pVal) const;
		virtual Int32 GetInt32(UnsafeArray<const UInt8> pVal) const;
		virtual UInt32 GetUInt32(UnsafeArray<const UInt8> pVal) const;
		virtual Int64 GetInt64(UnsafeArray<const UInt8> pVal) const;
		virtual UInt64 GetUInt64(UnsafeArray<const UInt8> pVal) const;
		virtual Single GetFloat16(UnsafeArray<const UInt8> pVal) const;
		virtual Single GetFloat24(UnsafeArray<const UInt8> pVal) const;
		virtual Single GetFloat32(UnsafeArray<const UInt8> pVal) const;
		virtual Double GetFloat64(UnsafeArray<const UInt8> pVal) const;
		virtual void SetInt32(UInt8 *pVal, Int32 v) const;
		virtual void SetInt16(UInt8 *pVal, Int16 v) const;
	};
}
#endif