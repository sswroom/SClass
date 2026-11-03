#ifndef _SM_IO_JAVA_JAVASHORTVALUE
#define _SM_IO_JAVA_JAVASHORTVALUE
#include "IO/Java/JavaElementValue.h"

namespace IO
{
	namespace Java
	{
		class JavaShortValue : public JavaElementValue
		{
		private:
			Int16 value;
		public:
			JavaShortValue(Int16 value);
			virtual ~JavaShortValue();

			Int16 GetValue() const;

			virtual void ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName);
			virtual ElementType GetElementType() const;
		};
	}
}
#endif