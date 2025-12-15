#ifndef _SM_JAVA_JAVAEVENTOBJECT
#define _SM_JAVA_JAVAEVENTOBJECT
#include "Java/JavaObject.h"

namespace Java
{
	class JavaEventObject : public JavaObject
	{
	public:
		JavaEventObject(jobject me);
		virtual ~JavaEventObject();

		static jclass GetClass();
	};
}
#endif
