#ifndef _SM_JAVA_JAVAWINDOWLISTENER
#define _SM_JAVA_JAVAWINDOWLISTENER
#include "Java/JavaEventListener.h"

namespace Java
{
	class JavaWindowListener : public JavaEventListener
	{
	public:
		JavaWindowListener(jobject me);
		virtual ~JavaWindowListener();

		static jclass GetClass();
	};
}
#endif
