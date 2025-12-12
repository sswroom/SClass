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
		JavaObject(const JavaObject& o);
		JavaObject(jobject me);
		virtual ~JavaObject();

		jobject GetJObject() const;
		Bool IsNull() const;
		
		static jclass GetClass();
	};
}
#endif
