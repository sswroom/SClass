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

		void SetItem(UnsafeArray<const UTF8Char> name, NN<Data::VariItem> item);
	public:
		VariObject(NameType nameType);
		~VariObject();

		NameType GetNameType() const;
		Bool HasItem(UnsafeArray<const UTF8Char> name) const;
		Data::VariItem *GetItem(UnsafeArray<const UTF8Char> name) const;
		void SetItemNull(UnsafeArray<const UTF8Char> name);
		void SetItemStr(UnsafeArray<const UTF8Char> name, UnsafeArrayOpt<const UTF8Char> str);
		void SetItemStr(UnsafeArray<const UTF8Char> name, Optional<Text::String> str);
		void SetItemDateTime(UnsafeArray<const UTF8Char> name, Data::DateTime *dt);
		void SetItemTS(UnsafeArray<const UTF8Char> name, const Data::Timestamp &ts);
		void SetItemF32(UnsafeArray<const UTF8Char> name, Single val);
		void SetItemF64(UnsafeArray<const UTF8Char> name, Double val);
		void SetItemI8(UnsafeArray<const UTF8Char> name, Int8 val);
		void SetItemU8(UnsafeArray<const UTF8Char> name, UInt8 val);
		void SetItemI16(UnsafeArray<const UTF8Char> name, Int16 val);
		void SetItemU16(UnsafeArray<const UTF8Char> name, UInt16 val);
		void SetItemI32(UnsafeArray<const UTF8Char> name, Int32 val);
		void SetItemU32(UnsafeArray<const UTF8Char> name, UInt32 val);
		void SetItemI64(UnsafeArray<const UTF8Char> name, Int64 val);
		void SetItemU64(UnsafeArray<const UTF8Char> name, UInt64 val);
		void SetItemBool(UnsafeArray<const UTF8Char> name, Bool val);
		void SetItemByteArray(UnsafeArray<const UTF8Char> name, const UInt8 *arr, UOSInt cnt);
		void SetItemVector(UnsafeArray<const UTF8Char> name, Optional<Math::Geometry::Vector2D> vec);
		void SetItemUUID(UnsafeArray<const UTF8Char> name, Data::UUID *uuid);
		void SetItemVectorDirect(UnsafeArray<const UTF8Char> name, Optional<Math::Geometry::Vector2D> vec);
		void SetItemUUIDDirect(UnsafeArray<const UTF8Char> name, Data::UUID *uuid);

		void ToString(NN<Text::StringBuilderUTF8> sb) const;
		NN<Class> CreateClass() const;
	};
}
#endif
