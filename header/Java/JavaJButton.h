#ifndef _SM_JAVA_JAVAJBUTTON
#define _SM_JAVA_JAVAJBUTTON
#include "Java/JavaAbstractButton.h"
#include "Text/CString.h"

namespace Java
{
	class JavaJButton : public JavaAbstractButton
	{
	public:
		JavaJButton(Text::CStringNN text);
		virtual ~JavaJButton();

		static jclass GetClass();
		static jobject NewObject(Text::CStringNN text);
	};
}
#endif
