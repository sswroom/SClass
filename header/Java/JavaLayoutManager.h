#ifndef _SM_JAVA_JAVALAYOUTMANAGER
#define _SM_JAVA_JAVALAYOUTMANAGER
#include "Java/JavaObject.h"

namespace Java
{
	class JavaLayoutManager : public JavaObject
	{
	public:
		JavaLayoutManager(jobject me);
		virtual ~JavaLayoutManager();

		static jclass GetClass();
	};
}
#endif
