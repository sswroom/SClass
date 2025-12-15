#ifndef _SM_JAVA_JAVACOMPONENTEVENT
#define _SM_JAVA_JAVACOMPONENTEVENT
#include "Java/JavaAWTEvent.h"

namespace Java
{
	class JavaComponentEvent : public JavaAWTEvent
	{
	public:
		JavaComponentEvent(jobject me);
		virtual ~JavaComponentEvent();

		static jclass GetClass();
	};
}
#endif
