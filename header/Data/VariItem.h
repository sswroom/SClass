#ifndef _SM_DATA_VARIITEM
#define _SM_DATA_VARIITEM
#include "Data/DateTime.h"
#include "Data/Timestamp.h"
#include "Data/ReadonlyArray.h"
#include "Data/UUID.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Data
{
	class VariItem
	{
	public:
		enum class ItemType
		{
			Unknown,
			Null,
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
			CStr,

			Str,
			ByteArr,
			Vector,
			UUID
		};
	
		union ItemValue
		{
			Text::String *str;
			Data::Timestamp date;
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
			Math::Geometry::Vector2D *vector;
			Data::UUID *uuid;
			struct { const UTF8Char *v; UOSInt leng; } cstr;
			UInt8 buff[16];

			ItemValue() = default;
			ItemValue(const ItemValue &v)
			{
				MemCopyNO(this->buff, v.buff, 16);
			}
			~ItemValue(){};
		};
	private:
		ItemValue val;
		ItemType itemType;
		
		VariItem(ItemType itemType, ItemValue val)
		{
			this->itemType = itemType;
			this->val = val;
		}

		void FreeItem()
		{
			if (this->itemType >= ItemType::Str && this->itemType <= ItemType::UUID)
			{
				if (this->itemType == ItemType::Str)
				{
					this->val.str->Release();
				}
				else if (this->itemType == ItemType::ByteArr)
				{
					DEL_CLASS(this->val.byteArr);
				}
				else if (this->itemType == ItemType::Vector)
				{
					DEL_CLASS(this->val.vector);
				}
				else
				{
					DEL_CLASS(this->val.uuid);
				}
			}
			this->itemType = ItemType::Unknown;
		}

	public:
		VariItem()
		{
			this->itemType = ItemType::Unknown;
			this->val.str = 0;
		}

		~VariItem()
		{
			this->FreeItem();
		}

		ItemType GetItemType() const
		{
			return this->itemType;
		}

		const ItemValue GetItemValue() const
		{
			return this->val;
		}

		Single GetAsF32() const;
		Double GetAsF64() const;
		Int8 GetAsI8() const;
		UInt8 GetAsU8() const;
		Int16 GetAsI16() const;
		UInt16 GetAsU16() const;
		Int32 GetAsI32() const;
		UInt32 GetAsU32() const;
		Int64 GetAsI64() const;
		UInt64 GetAsU64() const;
		Bool GetAsBool() const;
		void GetAsString(Text::StringBuilderUTF8 *sb) const;
		UTF8Char *GetAsStringS(UTF8Char *buff, UOSInt buffSize) const;
		Text::String *GetAsNewString() const;
		Data::DateTime *GetAsNewDate() const;
		Data::Timestamp GetAsTimestamp() const;
		Data::ReadonlyArray<UInt8> *GetAsNewByteArr() const;
		Math::Geometry::Vector2D *GetAsNewVector() const;
		Data::UUID *GetAsNewUUID() const;
		Data::ReadonlyArray<UInt8> *GetAndRemoveByteArr();
		Math::Geometry::Vector2D *GetAndRemoveVector();
		Data::UUID *GetAndRemoveUUID();
		void *GetAsUnk() const;

		void SetNull();
		void SetStrSlow(const UTF8Char *str);
		void SetStr(const UTF8Char *str, UOSInt strLen);
		void SetStr(Text::String *str);
		void SetDate(Data::DateTime *dt);
		void SetDate(const Data::Timestamp &ts);
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
		void SetVector(Math::Geometry::Vector2D *vec);
		void SetUUID(Data::UUID *uuid);
		void SetVectorDirect(Math::Geometry::Vector2D *vec);
		void SetUUIDDirect(Data::UUID *uuid);
		void Set(VariItem *item);

		VariItem *Clone() const;
		void ToString(Text::StringBuilderUTF8 *sb) const;

		static VariItem *NewNull();
		static VariItem *NewStrSlow(const UTF8Char *str);
		static VariItem *NewStr(Text::CString str);
		static VariItem *NewStr(Text::String *str);
		static VariItem *NewDate(Data::DateTime *dt);
		static VariItem *NewTS(const Data::Timestamp &ts);
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
		static VariItem *NewVector(Math::Geometry::Vector2D *vec);
		static VariItem *NewUUID(Data::UUID *uuid);
		static VariItem *NewDateDirect(Data::DateTime *dt);
		static VariItem *NewVectorDirect(Math::Geometry::Vector2D *vec);
		static VariItem *NewUUIDDirect(Data::UUID *uuid);
		static VariItem *NewFromPtr(void *ptr, ItemType itemType);
		
		static void SetFromPtr(VariItem *item, void *ptr, ItemType itemType);
		static void SetPtr(void *ptr, ItemType itemType, VariItem *item);
		static void SetPtrAndNotKeep(void *ptr, ItemType itemType, VariItem *item);
		static Bool PtrEquals(void *ptr1, void *ptr2, ItemType itemType);

		static UOSInt GetItemSize(ItemType itemType);
	};
}
#endif
