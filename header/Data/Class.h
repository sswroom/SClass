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
		const void *refObj;
		Data::ArrayList<FieldInfo *> fields;

		void FreeFieldInfo(FieldInfo *field);
	public:
		Class(const void *refObj);
		virtual ~Class();

		UOSInt AddField(Text::CString name, OSInt ofst, Data::VariItem::ItemType itemType); //return field size
		Bool AddField(Text::CString name, const UInt8 *val);
		Bool AddField(Text::CString name, const Int8 *val);
		Bool AddField(Text::CString name, const UInt16 *val);
		Bool AddField(Text::CString name, const Int16 *val);
		Bool AddField(Text::CString name, const UInt32 *val);
		Bool AddField(Text::CString name, const Int32 *val);
		Bool AddField(Text::CString name, const UInt64 *val);
		Bool AddField(Text::CString name, const Int64 *val);
		Bool AddField(Text::CString name, const Single *val);
		Bool AddField(Text::CString name, const Double *val);
		Bool AddField(Text::CString name, Text::String *const *val);
		Bool AddField(Text::CString name, const Data::Timestamp *val);
		Bool AddField(Text::CString name, const Bool *val);
		Bool AddField(Text::CString name, Data::ReadonlyArray<UInt8> *const *val);
		Bool AddField(Text::CString name, Math::Geometry::Vector2D *const *val);
		Bool AddField(Text::CString name, Data::UUID *const *val);

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
