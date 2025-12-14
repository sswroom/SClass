#ifndef _SM_JAVA_JAVAINTERFACE
#define _SM_JAVA_JAVAINTERFACE
#include "Java/JavaObject.h"
#include <jni.h>

namespace Java
{
	class JavaInterface
	{
	public:
		virtual NN<const JavaObject> ToObject() const = 0;
		virtual jobject GetInterfaceObject() const { return this->ToObject()->GetJObject(); }
	};
}
#endif
