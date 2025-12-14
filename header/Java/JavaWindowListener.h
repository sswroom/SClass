#ifndef _SM_JAVA_JAVAWINDOWLISTENER
#define _SM_JAVA_JAVAWINDOWLISTENER
#include "Java/JavaEventListener.h"
#include "Java/JavaObject.h"

namespace Java
{
	class JavaWindowListener : public JavaObject, public JavaEventListener
	{
	public:
		JavaWindowListener(jobject me);
		virtual ~JavaWindowListener();

		NN<const JavaObject> ToObject() const { return *this; }
		
		static jclass GetClass();
	};
}
#endif
