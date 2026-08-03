#ifndef _SM_IO_JAVA_JAVASTRINGVALUE
#define _SM_IO_JAVA_JAVASTRINGVALUE
#include "IO/Java/JavaElementValue.h"

namespace IO
{
	namespace Java
	{
		class JavaStringValue : public JavaElementValue
		{
		private:
			NN<Text::String> s;
		public:
			JavaStringValue(Text::CStringNN s);
			virtual ~JavaStringValue();

			NN<Text::String> GetString() const;

			virtual void ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName);
			virtual ElementType GetElementType() const;
		};
	}
}
#endif