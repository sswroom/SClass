#ifndef _SM_JAVA_JAVAFOCUSLISTENER
#define _SM_JAVA_JAVAFOCUSLISTENER
#include "Java/JavaEventListener.h"

namespace Java
{
	class JavaFocusListener : public JavaEventListener
	{
	public:
		static jclass GetClass();
	};
}
#endif
