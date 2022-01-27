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
		static void ToJavaName(Text::StringBuilderUTF8 *sb, const UTF8Char *usName, Bool isClass);
		static Text::CString GetJavaTypeName(DB::DBUtil::ColType colType, Bool notNull);
	};
}
#endif
