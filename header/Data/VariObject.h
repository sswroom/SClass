#ifndef _SM_DATA_VARIOBJECT
#define _SM_DATA_VARIOBJECT
//#include "Data/BTreeUTF8Map.h"
#include "Data/Class.h"
#include "Data/StringUTF8Map.h"
#include "Data/VariItem.h"

namespace Data
{
	class VariObject
	{
	public:
		enum class NameType
		{
			Database,
			Field
		};
	private:
		Data::StringUTF8Map<Data::VariItem*> items;
		NameType nameType;

		void SetItem(UnsafeArray<const UTF8Char> name, NN<Data::VariItem> item) { if (item.Set(this->items.Put(name, item.Ptr()))) item.Delete(); }
	public:
		VariObject(NameType nameType) { this->nameType = nameType; }
		~VariObject();

		NameType GetNameType() const { return this->nameType; }
		Bool HasItem(UnsafeArray<const UTF8Char> name) const { return this->items.Get(name) != 0; }
		Optional<Data::VariItem> GetItem(UnsafeArray<const UTF8Char> name) const { return this->items.Get(name); }
		void SetItemNull(UnsafeArray<const UTF8Char> name) { this->SetItem(name, Data::VariItem::NewNull()); }
		void SetItemStr(UnsafeArray<const UTF8Char> name, UnsafeArrayOpt<const UTF8Char> str) { this->SetItem(name, Data::VariItem::NewStrSlow(str)); }
		void SetItemStr(UnsafeArray<const UTF8Char> name, Optional<Text::String> str) { this->SetItem(name, Data::VariItem::NewStr(str)); }
		void SetItemDateTime(UnsafeArray<const UTF8Char> name, Data::DateTime *dt) { this->SetItem(name, Data::VariItem::NewDateTime(dt)); }
		void SetItemTS(UnsafeArray<const UTF8Char> name, const Data::Timestamp &ts) { this->SetItem(name, Data::VariItem::NewTS(ts)); }
		void SetItemF32(UnsafeArray<const UTF8Char> name, Single val) { this->SetItem(name, Data::VariItem::NewF32(val)); }
		void SetItemF64(UnsafeArray<const UTF8Char> name, Double val) { this->SetItem(name, Data::VariItem::NewF64(val)); }
		void SetItemI8(UnsafeArray<const UTF8Char> name, Int8 val) { this->SetItem(name, Data::VariItem::NewI8(val)); }
		void SetItemU8(UnsafeArray<const UTF8Char> name, UInt8 val) { this->SetItem(name, Data::VariItem::NewU8(val)); }
		void SetItemI16(UnsafeArray<const UTF8Char> name, Int16 val) { this->SetItem(name, Data::VariItem::NewI16(val)); }
		void SetItemU16(UnsafeArray<const UTF8Char> name, UInt16 val) { this->SetItem(name, Data::VariItem::NewU16(val)); }
		void SetItemI32(UnsafeArray<const UTF8Char> name, Int32 val) { this->SetItem(name, Data::VariItem::NewI32(val)); }
		void SetItemU32(UnsafeArray<const UTF8Char> name, UInt32 val) { this->SetItem(name, Data::VariItem::NewU32(val)); }
		void SetItemI64(UnsafeArray<const UTF8Char> name, Int64 val) { this->SetItem(name, Data::VariItem::NewI64(val)); }
		void SetItemU64(UnsafeArray<const UTF8Char> name, UInt64 val) { this->SetItem(name, Data::VariItem::NewU64(val)); }
		void SetItemBool(UnsafeArray<const UTF8Char> name, Bool val) { this->SetItem(name, Data::VariItem::NewBool(val)); }
		void SetItemByteArray(UnsafeArray<const UTF8Char> name, const UInt8 *arr, UOSInt cnt) { this->SetItem(name, Data::VariItem::NewByteArr(arr, cnt)); }
		void SetItemVector(UnsafeArray<const UTF8Char> name, Optional<Math::Geometry::Vector2D> vec) { this->SetItem(name, Data::VariItem::NewVector(vec)); }
		void SetItemUUID(UnsafeArray<const UTF8Char> name, Data::UUID *uuid) { this->SetItem(name, Data::VariItem::NewUUID(uuid)); }
		void SetItemVectorDirect(UnsafeArray<const UTF8Char> name, Optional<Math::Geometry::Vector2D> vec) { this->SetItem(name, Data::VariItem::NewVectorDirect(vec)); }
		void SetItemUUIDDirect(UnsafeArray<const UTF8Char> name, Data::UUID *uuid) { this->SetItem(name, Data::VariItem::NewUUIDDirect(uuid)); }

		void ToString(NN<Text::StringBuilderUTF8> sb) const;
		NN<Class> CreateClass() const;
	};
}
#endif
