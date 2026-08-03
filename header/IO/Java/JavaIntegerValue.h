#ifndef _SM_IO_JAVA_JAVAINTEGERVALUE
#define _SM_IO_JAVA_JAVAINTEGERVALUE
#include "IO/Java/JavaElementValue.h"

namespace IO
{
	namespace Java
	{
		class JavaIntegerValue : public JavaElementValue
		{
		private:
			Int32 value;
		public:
			JavaIntegerValue(Int32 value);
			virtual ~JavaIntegerValue();

			Int32 GetValue() const;

			virtual void ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName);
			virtual ElementType GetElementType() const;
		};
	}
}
#endif