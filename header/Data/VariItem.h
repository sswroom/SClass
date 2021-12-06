#ifndef _SM_DATA_VARIITEM
#define _SM_DATA_VARIITEM
#include "Data/DateTime.h"
#include "Data/ReadonlyArray.h"
#include "Data/UUID.h"
#include "Math/Vector2D.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF.h"

namespace Data
{
	class VariItem
	{
	public:
		enum class ItemType
		{
			Unknown,
			Null,
			Str,
			Date,
			F32,
			F64,
			I8,
			U8,
			I16,
			U16,
			I32,
			U32,
			I64,
			U64,
			BOOL,
			ByteArr,
			Vector,
			UUID
		};
	
		union ItemValue
		{
			Text::String *str;
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
			Data::UUID *uuid;
		};
	private:
		ItemType itemType;
		ItemValue val;
		
		VariItem(ItemType itemType, ItemValue val);
		void FreeItem();
	public:
		VariItem();
		~VariItem();

		ItemType GetItemType();
		const ItemValue GetItemValue();
		Single GetAsF32();
		Double GetAsF64();
		Int8 GetAsI8();
		UInt8 GetAsU8();
		Int16 GetAsI16();
		UInt16 GetAsU16();
		Int32 GetAsI32();
		UInt32 GetAsU32();
		Int64 GetAsI64();
		UInt64 GetAsU64();
		Bool GetAsBool();
		void GetAsString(Text::StringBuilderUTF *sb);
		Data::DateTime *GetAsNewDate();
		Data::ReadonlyArray<UInt8> *GetAsNewByteArr();
		Math::Vector2D *GetAsNewVector();
		Data::UUID *GetAsNewUUID();
		void *GetAsUnk();

		void SetNull();
		void SetStr(const UTF8Char *str);
		void SetStr(Text::String *str);
		void SetDate(Data::DateTime *dt);
		void SetF32(Single val);
		void SetF64(Double val);
		void SetI8(Int8 val);
		void SetU8(UInt8 val);
		void SetI16(Int16 val);
		void SetU16(UInt16 val);
		void SetI32(Int32 val);
		void SetU32(UInt32 val);
		void SetI64(Int64 val);
		void SetU64(UInt64 val);
		void SetBool(Bool val);
		void SetByteArr(const UInt8 *arr, UOSInt cnt);
		void SetByteArr(Data::ReadonlyArray<UInt8> *arr);
		void SetVector(Math::Vector2D *vec);
		void SetUUID(Data::UUID *uuid);
		void SetDateDirect(Data::DateTime *dt);
		void SetVectorDirect(Math::Vector2D *vec);
		void SetUUIDDirect(Data::UUID *uuid);
		void Set(VariItem *item);

		VariItem *Clone();
		void ToString(Text::StringBuilderUTF *sb);

		static VariItem *NewNull();
		static VariItem *NewStr(const UTF8Char *str);
		static VariItem *NewStr(Text::String *str);
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
		static VariItem *NewByteArr(Data::ReadonlyArray<UInt8> *arr);
		static VariItem *NewVector(Math::Vector2D *vec);
		static VariItem *NewUUID(Data::UUID *uuid);
		static VariItem *NewDateDirect(Data::DateTime *dt);
		static VariItem *NewVectorDirect(Math::Vector2D *vec);
		static VariItem *NewUUIDDirect(Data::UUID *uuid);
		static VariItem *NewFromPtr(void *ptr, ItemType itemType);
		
		static void SetFromPtr(VariItem *item, void *ptr, ItemType itemType);
		static void SetPtr(void *ptr, ItemType itemType, VariItem *item);
		static Bool PtrEquals(void *ptr1, void *ptr2, ItemType itemType);

		static UOSInt GetItemSize(ItemType itemType);
	};
}
#endif
