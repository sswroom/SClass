#ifndef _SM_JAVA_JAVACONTAINER
#define _SM_JAVA_JAVACONTAINER
#include "Java/JavaComponent.h"

namespace Java
{
	class JavaContainer : public JavaComponent
	{
	public:
		JavaContainer(jobject me);
		virtual ~JavaContainer();

		static jclass GetClass();
	};
}
#endif
