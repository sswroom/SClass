#ifndef _SM_JAVA_JAVAMOUSELISTENER
#define _SM_JAVA_JAVAMOUSELISTENER
#include "Java/JavaEventListener.h"

namespace Java
{
	class JavaMouseListener : public JavaEventListener
	{
	public:
		static jclass GetClass();
	};
}
#endif
