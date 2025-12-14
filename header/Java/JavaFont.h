#ifndef _SM_JAVA_JAVAFONT
#define _SM_JAVA_JAVAFONT
#include "Java/JavaObject.h"
#include "Text/CString.h"

namespace Java
{
	class JavaFont : public JavaObject
	{
	public:
		enum class Style
		{
			PLAIN = 0,
			BOLD = 1,
			ITALIC = 2
		};
	public:
		JavaFont(Text::CString name, Style style, Int32 size);
		virtual ~JavaFont();

		static jclass GetClass();
		static jobject NewObject(Text::CString name, Style style, Int32 size);
	};
}
#endif
