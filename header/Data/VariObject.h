#ifndef _SM_DATA_VARIOBJECT
#define _SM_DATA_VARIOBJECT
#include "Data/StringUTF8Map.h"
#include "Data/VariItem.h"

namespace Data
{
	class VariObject
	{
	private:
		Data::StringUTF8Map<Data::VariItem*> *items;

		void SetItem(const UTF8Char *name, Data::VariItem *item);
	public:
		VariObject();
		~VariObject();

		Bool HasItem(const UTF8Char *name);
		Data::VariItem *GetItem(const UTF8Char *name);
		void SetItemNull(const UTF8Char *name);
		void SetItemStr(const UTF8Char *name, const UTF8Char *str);
		void SetItemDate(const UTF8Char *name, Data::DateTime *dt);
		void SetItemF32(const UTF8Char *name, Single val);
		void SetItemF64(const UTF8Char *name, Double val);
		void SetItemI8(const UTF8Char *name, Int8 val);
		void SetItemU8(const UTF8Char *name, UInt8 val);
		void SetItemI16(const UTF8Char *name, Int16 val);
		void SetItemU16(const UTF8Char *name, UInt16 val);
		void SetItemI32(const UTF8Char *name, Int32 val);
		void SetItemU32(const UTF8Char *name, UInt32 val);
		void SetItemI64(const UTF8Char *name, Int64 val);
		void SetItemU64(const UTF8Char *name, UInt64 val);
		void SetItemBool(const UTF8Char *name, Bool val);
		void SetItemByteArray(const UTF8Char *name, const UInt8 *arr, UOSInt cnt);
		void SetItemVector(const UTF8Char *name, Math::Vector2D *vec);
		void SetItemUUID(const UTF8Char *name, Data::UUID *uuid);

		void ToString(Text::StringBuilderUTF *sb);
	};
}
#endif
