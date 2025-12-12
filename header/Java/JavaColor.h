#ifndef _SM_JAVA_JAVACOLOR
#define _SM_JAVA_JAVACOLOR
#include "Java/JavaObject.h"

namespace Java
{
	class JavaColor : public JavaObject
	{
	public:
		JavaColor(jobject me);
		JavaColor(Int32 rgba, Bool hasalpha);
		virtual ~JavaColor();

		static jclass GetClass();
		static jobject NewObject(Int32 rgba, Bool hasalpha);
	};
}
#endif
