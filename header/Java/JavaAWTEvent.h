#ifndef _SM_JAVA_JAVAAWTEVENT
#define _SM_JAVA_JAVAAWTEVENT
#include "Java/JavaEventObject.h"

namespace Java
{
	class JavaAWTEvent : public JavaEventObject
	{
	public:
		JavaAWTEvent(jobject me);
		virtual ~JavaAWTEvent();

		static jclass GetClass();
	};
}
#endif
