#ifndef _SM_JAVA_JAVAEVENTLISTENER
#define _SM_JAVA_JAVAEVENTLISTENER
#include "Java/JavaInterface.h"

namespace Java
{
	class JavaEventListener : public JavaInterface
	{
	public:
		static jclass GetClass();
	};
}
#endif
