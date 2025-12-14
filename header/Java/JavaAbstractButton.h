#ifndef _SM_JAVA_JAVAABSTRACTBUTTON
#define _SM_JAVA_JAVAABSTRACTBUTTON
#include "Java/JavaJComponent.h"
#include "Text/CString.h"

namespace Java
{
	class JavaAbstractButton : public JavaJComponent
	{
	private:
		static jmethodID setText;
	public:
		JavaAbstractButton(jobject me);
		virtual ~JavaAbstractButton();

		void SetText(Text::CStringNN text);

		static jclass GetClass();
	};
}
#endif
