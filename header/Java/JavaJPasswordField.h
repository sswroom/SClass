#ifndef _SM_JAVA_JAVAJPASSWORDFIELD
#define _SM_JAVA_JAVAJPASSWORDFIELD
#include "Java/JavaJTextField.h"

namespace Java
{
	class JavaJPasswordField : public JavaJTextField
	{
	private:
		static jmethodID setEchoChar;
	public:
		JavaJPasswordField(Text::CStringNN text);
		virtual ~JavaJPasswordField();

		void SetEchoChar(UTF16Char c);

		static jclass GetClass();
		static jobject NewObject(Text::CStringNN text);
	};
}
#endif
