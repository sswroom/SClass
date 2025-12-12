#ifndef _SM_JAVA_JAVACONTAINER
#define _SM_JAVA_JAVACONTAINER
#include "Java/JavaComponent.h"

namespace Java
{
	class JavaContainer : public JavaComponent
	{
	private:
		static jmethodID add;
	public:
		JavaContainer(jobject me);
		virtual ~JavaContainer();

		JavaComponent Add(NN<JavaComponent> comp);

		static jclass GetClass();
	};
}
#endif
