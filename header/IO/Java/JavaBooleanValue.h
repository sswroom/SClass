#ifndef _SM_IO_JAVA_JAVABOOLEANVALUE
#define _SM_IO_JAVA_JAVABOOLEANVALUE
#include "IO/Java/JavaElementValue.h"

namespace IO
{
	namespace Java
	{
		class JavaBooleanValue : public JavaElementValue
		{
		private:
			Bool value;
		public:
			JavaBooleanValue(Bool value);
			virtual ~JavaBooleanValue();

			Bool GetValue() const;

			virtual void ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName);
			virtual ElementType GetElementType() const;
		};
	}
}
#endif