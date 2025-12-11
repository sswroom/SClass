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
		jclass cls;
	public:
		JavaJLabel();
		JavaJLabel(Text::CStringNN text);
		virtual ~JavaJLabel();

		static jclass GetClass();
	};
}
#endif
