#ifndef _SM_JAVA_JAVAOBJECT
#define _SM_JAVA_JAVAOBJECT
#include <jni.h>

namespace Java
{
	class JavaObject
	{
	protected:
		jobject me;
	public:
		JavaObject(jobject me);
		virtual ~JavaObject();

		jobject GetJObject() const;
		
		static jclass GetClass();
	};
}
#endif
