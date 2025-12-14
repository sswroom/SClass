#ifndef _SM_JAVA_JAVAJTEXTFIELD
#define _SM_JAVA_JAVAJTEXTFIELD
#include "Java/JavaJTextComponent.h"
#include "Text/CString.h"

namespace Java
{
	class JavaJTextField : public JavaJTextComponent
	{
	public:
		JavaJTextField(Text::CStringNN text);
		JavaJTextField(jobject me);
		virtual ~JavaJTextField();

		static jclass GetClass();
		static jobject NewObject(Text::CStringNN text);
	};
}
#endif
