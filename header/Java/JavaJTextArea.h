#ifndef _SM_JAVA_JAVAJTEXTAREA
#define _SM_JAVA_JAVAJTEXTAREA
#include "Java/JavaJTextComponent.h"
#include "Text/CString.h"

namespace Java
{
	class JavaJTextArea : public JavaJTextComponent
	{
	private:
		static jmethodID setLineWrap;
	public:
		JavaJTextArea(Text::CStringNN text);
		virtual ~JavaJTextArea();

		void SetLineWrap(Bool wrap);

		static jclass GetClass();
		static jobject NewObject(Text::CStringNN text);
	};
}
#endif
