#ifndef _SM_DATA_BYTEORDER
#define _SM_DATA_BYTEORDER
namespace Data
{
	class ByteOrder
	{
	public:
		virtual ~ByteOrder() {};

		virtual Bool IsBigEndian() = 0;
		virtual Int16 GetInt16(UnsafeArray<const UInt8> pVal) const = 0;
		virtual UInt16 GetUInt16(UnsafeArray<const UInt8> pVal) const = 0;
		virtual Int32 GetInt32(UnsafeArray<const UInt8> pVal) const = 0;
		virtual UInt32 GetUInt32(UnsafeArray<const UInt8> pVal) const = 0;
		virtual Int64 GetInt64(UnsafeArray<const UInt8> pVal) const = 0;
		virtual UInt64 GetUInt64(UnsafeArray<const UInt8> pVal) const = 0;
		virtual Single GetFloat16(UnsafeArray<const UInt8> pVal) const = 0;
		virtual Single GetFloat24(UnsafeArray<const UInt8> pVal) const = 0;
		virtual Single GetFloat32(UnsafeArray<const UInt8> pVal) const = 0;
		virtual Double GetFloat64(UnsafeArray<const UInt8> pVal) const = 0;
		virtual void SetInt16(UInt8 *pVal, Int16 v) const = 0;
		virtual void SetInt32(UInt8 *pVal, Int32 v) const = 0;
	};
}
#endif