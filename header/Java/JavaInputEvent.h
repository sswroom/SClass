#ifndef _SM_JAVA_JAVAINPUTEVENT
#define _SM_JAVA_JAVAINPUTEVENT
#include "Java/JavaComponentEvent.h"

namespace Java
{
	class JavaInputEvent : public JavaComponentEvent
	{
	public:
		JavaInputEvent(jobject me);
		virtual ~JavaInputEvent();

		static jclass GetClass();
	};
}
#endif
