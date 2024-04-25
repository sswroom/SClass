#ifndef _SM_TEXT_JAVATEXT
#define _SM_TEXT_JAVATEXT
#include "DB/DBUtil.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class JavaText
	{
	public:
		static void ToJavaName(NN<Text::StringBuilderUTF8> sb, const UTF8Char *usName, Bool isClass);
		static void ToDBName(NN<Text::StringBuilderUTF8> sb, const UTF8Char *name);
		static Text::CStringNN GetJavaTypeName(DB::DBUtil::ColType colType, Bool notNull);
		static Text::CStringNN GetJavaTypeName(Data::VariItem::ItemType itemType, Bool notNull);
	};
}
#endif
