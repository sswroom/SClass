#ifndef _SM_DATA_CLASS
#define _SM_DATA_CLASS
#include "Data/StringUTF8Map.h"
#include "Data/VariItem.h"
#include "Text/StringBuilderUTF.h"

namespace Data
{
	class Class
	{
	public:
		struct FieldInfo
		{
			const UTF8Char *name;
			OSInt ofst;
			Data::VariItem::ItemType itemType;
		};
	private:
		void *refObj;
		Data::StringUTF8Map<FieldInfo *> *fields;

		void FreeFieldInfo(FieldInfo *field);
	public:
		Class(void *refObj);
		virtual ~Class();

		UOSInt AddField(const UTF8Char *name, OSInt ofst, Data::VariItem::ItemType itemType); //return field size
		Bool AddField(const UTF8Char *name, UInt8 *val);
		Bool AddField(const UTF8Char *name, Int8 *val);
		Bool AddField(const UTF8Char *name, UInt16 *val);
		Bool AddField(const UTF8Char *name, Int16 *val);
		Bool AddField(const UTF8Char *name, UInt32 *val);
		Bool AddField(const UTF8Char *name, Int32 *val);
		Bool AddField(const UTF8Char *name, UInt64 *val);
		Bool AddField(const UTF8Char *name, Int64 *val);
		Bool AddField(const UTF8Char *name, Single *val);
		Bool AddField(const UTF8Char *name, Double *val);
		Bool AddField(const UTF8Char *name, const UTF8Char **val);
		Bool AddField(const UTF8Char *name, Data::DateTime **val);
		Bool AddField(const UTF8Char *name, Bool *val);
		Bool AddField(const UTF8Char *name, Data::ReadonlyArray<UInt8> **val);
		Bool AddField(const UTF8Char *name, Math::Vector2D **val);
		Bool AddField(const UTF8Char *name, Data::UUID **val);

		UOSInt GetFieldCount();
		const UTF8Char *GetFieldName(UOSInt index);
		Data::VariItem *GetNewValue(UOSInt index, void *obj);
		Bool SetField(void *obj, UOSInt index, Data::VariItem *item);
		Bool Equals(void *obj1, void *obj2);

		void ToCppClassHeader(const UTF8Char *clsName, UOSInt tabLev, Text::StringBuilderUTF *sb);
		void ToCppClassSource(const UTF8Char *clsPrefix, const UTF8Char *clsName, UOSInt tabLev, Text::StringBuilderUTF *sb);
	};
}

#define CLASS_ADD(cls, val) (cls->AddField((const UTF8Char*)#val, &this->val))
#endif