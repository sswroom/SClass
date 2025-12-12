#ifndef _SM_JAVA_JAVAEVENTLISTENER
#define _SM_JAVA_JAVAEVENTLISTENER
#include "Java/JavaObject.h"

namespace Java
{
	class JavaEventListener : public JavaObject
	{
	public:
		JavaEventListener(jobject me);
		virtual ~JavaEventListener();

		static jclass GetClass();
	};
}
#endif
