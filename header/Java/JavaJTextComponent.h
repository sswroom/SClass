#ifndef _SM_JAVA_JAVAJTEXTCOMPONENT
#define _SM_JAVA_JAVAJTEXTCOMPONENT
#include "Java/JavaJComponent.h"
#include "Java/JavaString.h"

namespace Java
{
	class JavaJTextComponent : public JavaJComponent
	{
	private:
		static jmethodID getText;
		static jmethodID selectAll;
		static jmethodID setEditable;
		static jmethodID setText;
	public:
		JavaJTextComponent(jobject me);
		virtual ~JavaJTextComponent();

		JavaString GetText();
		void SelectAll();
		void SetEditable(Bool b);
		void SetText(Text::CStringNN t);

		static jclass GetClass();
	};
}
#endif
