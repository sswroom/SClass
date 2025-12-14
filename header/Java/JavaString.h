#ifndef _SM_JAVA_JAVASTRING
#define _SM_JAVA_JAVASTRING
#include "Java/JavaObject.h"
#include "Text/StringBuilderUTF8.h"

namespace Java
{
	class JavaString : public JavaObject
	{
	public:
		JavaString(jobject me);
		virtual ~JavaString();

		UnsafeArray<UTF8Char> Get(UnsafeArray<UTF8Char> sbuff);
		void Get(NN<Text::StringBuilderUTF8> sb);

		static jclass GetClass();
	};
}
#endif
