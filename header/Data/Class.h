#ifndef _SM_DATA_CLASS
#define _SM_DATA_CLASS
#include "Data/ArrayList.h"
#include "Data/VariItem.h"
#include "Text/StringBuilderUTF8.h"

namespace Data
{
	class Class
	{
	public:
		struct FieldInfo
		{
			Text::String *name;
			OSInt ofst;
			Data::VariItem::ItemType itemType;
		};
	private:
		void *refObj;
		Data::ArrayList<FieldInfo *> fields;

		void FreeFieldInfo(FieldInfo *field);
	public:
		Class(void *refObj);
		virtual ~Class();

		UOSInt AddField(Text::CString name, OSInt ofst, Data::VariItem::ItemType itemType); //return field size
		Bool AddField(Text::CString name, UInt8 *val);
		Bool AddField(Text::CString name, Int8 *val);
		Bool AddField(Text::CString name, UInt16 *val);
		Bool AddField(Text::CString name, Int16 *val);
		Bool AddField(Text::CString name, UInt32 *val);
		Bool AddField(Text::CString name, Int32 *val);
		Bool AddField(Text::CString name, UInt64 *val);
		Bool AddField(Text::CString name, Int64 *val);
		Bool AddField(Text::CString name, Single *val);
		Bool AddField(Text::CString name, Double *val);
		Bool AddField(Text::CString name, Text::String **val);
		Bool AddField(Text::CString name, Data::DateTime **val);
		Bool AddField(Text::CString name, Bool *val);
		Bool AddField(Text::CString name, Data::ReadonlyArray<UInt8> **val);
		Bool AddField(Text::CString name, Math::Vector2D **val);
		Bool AddField(Text::CString name, Data::UUID **val);

		UOSInt GetFieldCount();
		Text::String *GetFieldName(UOSInt index);
		Data::VariItem::ItemType GetFieldType(UOSInt index);
		Data::VariItem *GetNewValue(UOSInt index, void *obj);
		Bool GetValue(Data::VariItem *itm, UOSInt index, void *obj);
		Bool SetField(void *obj, UOSInt index, Data::VariItem *item);
		Bool SetFieldClearItem(void *obj, UOSInt index, Data::VariItem *item);
		Bool Equals(void *obj1, void *obj2);

		void ToCppClassHeader(Text::StringBase<UTF8Char> *clsName, UOSInt tabLev, Text::StringBuilderUTF8 *sb);
		void ToCppClassSource(Text::StringBase<UTF8Char> *clsPrefix, Text::StringBase<UTF8Char> *clsName, UOSInt tabLev, Text::StringBuilderUTF8 *sb);
	};
}

#define CLASS_ADD(cls, val) (cls->AddField(CSTR(#val), &this->val))
#endif
