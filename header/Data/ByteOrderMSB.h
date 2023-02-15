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
		virtual Int32 GetInt32(const UInt8 *pVal) const;
		virtual Int16 GetInt16(const UInt8 *pVal) const;
		virtual UInt32 GetUInt32(const UInt8 *pVal) const;
		virtual UInt16 GetUInt16(const UInt8 *pVal) const;
		virtual Single GetFloat16(const UInt8 *pVal) const;
		virtual Single GetFloat24(const UInt8 *pVal) const;
		virtual Single GetFloat32(const UInt8 *pVal) const;
		virtual Double GetFloat64(const UInt8 *pVal) const;
		virtual void SetInt32(UInt8 *pVal, Int32 v) const;
		virtual void SetInt16(UInt8 *pVal, Int16 v) const;
	};
}
#endif