#ifndef _SM_JAVA_JAVAJLABEL
#define _SM_JAVA_JAVAJLABEL
#include "Java/JavaJComponent.h"
#include "Text/CString.h"
#include <jni.h>

namespace Java
{
	class JavaJLabel : public JavaJComponent
	{
	private:
		static jmethodID setText;

	public:
		JavaJLabel();
		JavaJLabel(Text::CStringNN text);
		virtual ~JavaJLabel();

		void SetText(Text::CStringNN text);

		static jclass GetClass();
		static jobject NewObject();
		static jobject NewObject(Text::CStringNN text);
	};
}
#endif
