#ifndef _SM_DATA_VARIITEM
#define _SM_DATA_VARIITEM
#include "Data/DateTime.h"
#include "Data/ReadonlyArray.h"
#include "Math/Vector2D.h"

namespace Data
{
	class VariItem
	{
	public:
		enum ItemType
		{
			IT_UNKNOWN,
			IT_NULL,
			IT_STR,
			IT_DATE,
			IT_F32,
			IT_F64,
			IT_I8,
			IT_U8,
			IT_I16,
			IT_U16,
			IT_I32,
			IT_U32,
			IT_I64,
			IT_U64,
			IT_BOOL,
			IT_BYTEARR,
			IT_VECTOR
		};
	
		union ItemValue
		{
			const UTF8Char *str;
			Data::DateTime *date;
			Single f32;
			Double f64;
			Int8 i8;
			UInt8 u8;
			Int16 i16;
			UInt16 u16;
			Int32 i32;
			UInt32 u32;
			Int64 i64;
			UInt64 u64;
			Bool boolean;
			Data::ReadonlyArray<UInt8> *byteArr;
			Math::Vector2D *vector;
		};
	private:
		ItemType itemType;
		ItemValue val;
		
		VariItem(ItemType itemType, ItemValue val);
	public:
		~VariItem();

		ItemType GetItemType();
		const ItemValue GetItemValue();

		static VariItem *NewNull();
		static VariItem *NewStr(const UTF8Char *str);
		static VariItem *NewDate(Data::DateTime *dt);
		static VariItem *NewF32(Single val);
		static VariItem *NewF64(Double val);
		static VariItem *NewI8(Int8 val);
		static VariItem *NewU8(UInt8 val);
		static VariItem *NewI16(Int16 val);
		static VariItem *NewU16(UInt16 val);
		static VariItem *NewI32(Int32 val);
		static VariItem *NewU32(UInt32 val);
		static VariItem *NewI64(Int64 val);
		static VariItem *NewU64(UInt64 val);
		static VariItem *NewBool(Bool val);
		static VariItem *NewByteArr(const UInt8 *arr, UOSInt cnt);
		static VariItem *NewVector(Math::Vector2D *vec);
	};
}
#endif
