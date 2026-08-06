#ifndef _SM_IO_JAVA_JAVALONGVALUE
#define _SM_IO_JAVA_JAVALONGVALUE
#include "IO/Java/JavaElementValue.h"

namespace IO
{
	namespace Java
	{
		class JavaLongValue : public JavaElementValue
		{
		private:
			Int64 value;
		public:
			JavaLongValue(Int64 value);
			virtual ~JavaLongValue();

			Int64 GetValue() const;

			virtual void ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName);
			virtual ElementType GetElementType() const;
		};
	}
}
#endif