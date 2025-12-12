#ifndef _SM_JAVA_JAVAJCOMPONENT
#define _SM_JAVA_JAVAJCOMPONENT
#include "Java/JavaColor.h"
#include "Java/JavaContainer.h"

namespace Java
{
	class JavaJComponent : public JavaContainer
	{
	private:
		static jmethodID setForeground;
	public:
		JavaJComponent(jobject me);
		virtual ~JavaJComponent();

		void SetForeground(NN<JavaColor> fg);

		static jclass GetClass();
	};
}
#endif
