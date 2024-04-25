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
			Timestamp,
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
			Date,

			Str,
			ByteArr,
			Vector,
			UUID
		};
	
		union ItemValue
		{
			Text::String *str;
			Data::Timestamp ts;
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
			Data::Date date;
			Data::ReadonlyArray<UInt8> *byteArr;
			NN<Math::Geometry::Vector2D> vector;
			NN<Data::UUID> uuid;
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
					this->val.vector.Delete();
				}
				else
				{
					this->val.uuid.Delete();
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
		void GetAsString(NN<Text::StringBuilderUTF8> sb) const;
		UTF8Char *GetAsStringS(UTF8Char *buff, UOSInt buffSize) const;
		Text::String *GetAsNewString() const;
		Data::DateTime *GetAsNewDateTime() const;
		Data::Timestamp GetAsTimestamp() const;
		Data::Date GetAsDate() const;
		Data::ReadonlyArray<UInt8> *GetAsNewByteArr() const;
		Optional<Math::Geometry::Vector2D> GetAsNewVector() const;
		Data::UUID *GetAsNewUUID() const;
		Data::ReadonlyArray<UInt8> *GetAndRemoveByteArr();
		Optional<Math::Geometry::Vector2D> GetAndRemoveVector();
		Data::UUID *GetAndRemoveUUID();
		void *GetAsUnk() const;

		void SetNull();
		void SetStrSlow(const UTF8Char *str);
		void SetStr(const UTF8Char *str, UOSInt strLen);
		void SetStrCopy(const UTF8Char *str, UOSInt strLen);
		void SetStr(Optional<Text::String> str);
		void SetStr(NN<Text::String> str);
		void SetDate(Data::DateTime *dt);
		void SetDate(const Data::Timestamp &ts);
		void SetDate(const Data::Date &date);
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
		void SetVector(NN<Math::Geometry::Vector2D> vec);
		void SetUUID(NN<Data::UUID> uuid);
		void SetVectorDirect(NN<Math::Geometry::Vector2D> vec);
		void SetUUIDDirect(NN<Data::UUID> uuid);
		void Set(NN<VariItem> item);

		NN<VariItem> Clone() const;
		void ToString(NN<Text::StringBuilderUTF8> sb) const;

		static NN<VariItem> NewNull();
		static NN<VariItem> NewStrSlow(const UTF8Char *str);
		static NN<VariItem> NewStr(Text::CString str);
		static NN<VariItem> NewStr(Optional<Text::String> str);
		static NN<VariItem> NewDateTime(Data::DateTime *dt);
		static NN<VariItem> NewTS(const Data::Timestamp &ts);
		static NN<VariItem> NewDate(const Data::Date &date);
		static NN<VariItem> NewF32(Single val);
		static NN<VariItem> NewF64(Double val);
		static NN<VariItem> NewI8(Int8 val);
		static NN<VariItem> NewU8(UInt8 val);
		static NN<VariItem> NewI16(Int16 val);
		static NN<VariItem> NewU16(UInt16 val);
		static NN<VariItem> NewI32(Int32 val);
		static NN<VariItem> NewU32(UInt32 val);
		static NN<VariItem> NewI64(Int64 val);
		static NN<VariItem> NewU64(UInt64 val);
		static NN<VariItem> NewBool(Bool val);
		static NN<VariItem> NewByteArr(const UInt8 *arr, UOSInt cnt);
		static NN<VariItem> NewByteArr(Data::ReadonlyArray<UInt8> *arr);
		static NN<VariItem> NewVector(Optional<Math::Geometry::Vector2D> vec);
		static NN<VariItem> NewUUID(Data::UUID *uuid);
		static NN<VariItem> NewVectorDirect(Optional<Math::Geometry::Vector2D> vec);
		static NN<VariItem> NewUUIDDirect(Data::UUID *uuid);
		static NN<VariItem> NewFromPtr(void *ptr, ItemType itemType);
		
		static void SetFromPtr(NN<VariItem> item, void *ptr, ItemType itemType);
		static void SetPtr(void *ptr, ItemType itemType, NN<VariItem> item);
		static void SetPtrAndNotKeep(void *ptr, ItemType itemType, NN<VariItem> item);
		static Bool PtrEquals(void *ptr1, void *ptr2, ItemType itemType);

		static UOSInt GetItemSize(ItemType itemType);
		static Text::CStringNN ItemTypeGetName(ItemType itemType);
	};
}
#endif
