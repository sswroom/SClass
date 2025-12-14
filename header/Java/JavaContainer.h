#ifndef _SM_JAVA_JAVACONTAINER
#define _SM_JAVA_JAVACONTAINER
#include "Java/JavaComponent.h"
#include "Java/JavaLayoutManager.h"

namespace Java
{
	class JavaContainer : public JavaComponent
	{
	private:
		static jmethodID add;
		static jmethodID setLayout;
	public:
		JavaContainer(jobject me);
		virtual ~JavaContainer();

		JavaComponent Add(NN<JavaComponent> comp);
		void SetLayout(Optional<JavaLayoutManager> mgr);

		static jclass GetClass();
	};
}
#endif
