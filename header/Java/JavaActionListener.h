#ifndef _SM_JAVA_JAVAACTIONLISTENER
#define _SM_JAVA_JAVAACTIONLISTENER
#include "Java/JavaEventListener.h"

namespace Java
{
	class JavaActionListener : public JavaEventListener
	{
	public:
		static jclass GetClass();
	};
}
#endif
