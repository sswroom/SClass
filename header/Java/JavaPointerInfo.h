#ifndef _SM_JAVA_JAVAPOINTERINFO
#define _SM_JAVA_JAVAPOINTERINFO
#include "Java/JavaGraphicsDevice.h"
#include "Java/JavaObject.h"

namespace Java
{
	class JavaPointerInfo : public JavaObject
	{
	private:
		static jmethodID getDevice;
		static jmethodID getLocation;
	public:
		JavaPointerInfo(jobject me);
		virtual ~JavaPointerInfo();

		JavaGraphicsDevice GetDevice();
		
		static jclass GetClass();
	};
}
#endif
