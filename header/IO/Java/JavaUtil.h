#ifndef _SM_IO_JAVA_JAVAUTIL
#define _SM_IO_JAVA_JAVAUTIL
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace Java
	{
		class JavaUtil
		{
		public:
			static void AccessFlags(NN<Text::StringBuilderUTF8> sb, UInt16 accessFlags);
			static void ConstVal(NN<Text::StringBuilderUTF8> sb, UnsafeArray<UInt8> constVal, Bool brankets, UnsafeArray<UnsafeArrayOpt<UInt8>> constPool);
		};
	}
}
#endif