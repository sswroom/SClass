#ifndef _SM_JAVA_JAVAJFRAME
#define _SM_JAVA_JAVAJFRAME
#include "Java/JavaContainer.h"
#include "Java/JavaFrame.h"

namespace Java
{
	class JavaJFrame : public JavaFrame
	{
	private:
		static jmethodID getContentPane;
	public:
		JavaJFrame();
		JavaJFrame(jobject me);
		virtual ~JavaJFrame();

		JavaContainer GetContentPane();

		static jclass GetClass();
		static jobject NewObject();
	};
}
#endif
