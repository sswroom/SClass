#ifndef _SM_JAVA_JAVAJLABEL
#define _SM_JAVA_JAVAJLABEL
#include "Text/CString.h"
#include <jni.h>

namespace Java
{
	class JavaJLabel
	{
	private:
		jobject me;
	public:
		JavaJLabel();
		JavaJLabel(Text::CStringNN text);
		JavaJLabel(Text::CStringNN text);
		JavaJLabel(Text::CStringNN text);

		static jclass GetClass();
	};
}
#endif
