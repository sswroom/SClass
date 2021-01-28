#ifndef _SM_TEXT_JAVATEXT
#define _SM_TEXT_JAVATEXT
#include "DB/DBUtil.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	class JavaText
	{
	public:
		static void ToJavaName(Text::StringBuilderUTF *sb, const UTF8Char *usName, Bool isClass);
		static const UTF8Char *GetJavaTypeName(DB::DBUtil::ColType colType, Bool notNull);
	};
}
#endif
