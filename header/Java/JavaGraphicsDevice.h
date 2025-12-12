#ifndef _SM_JAVA_JAVAGRAPHICSDEVICE
#define _SM_JAVA_JAVAGRAPHICSDEVICE
#include "Java/JavaDisplayMode.h"
#include "Java/JavaObject.h"

namespace Java
{
	class JavaGraphicsDevice : public JavaObject
	{
	private:
		static jmethodID getDisplayMode;
	public:
		JavaGraphicsDevice(jobject me);
		virtual ~JavaGraphicsDevice();

		JavaDisplayMode GetDisplayMode();

		static jclass GetClass();
	};
}
#endif
