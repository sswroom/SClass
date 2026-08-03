#ifndef _SM_IO_JAVA_JAVATYPE
#define _SM_IO_JAVA_JAVATYPE
#include "Data/ArrayListStringNN.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace Java
	{
		class JavaType
		{
		private:
			NN<Text::String> typeStr;
		public:
			JavaType(Text::CStringNN typeStr);
			~JavaType();

			NN<Text::String> GetTypeStr() const;
			void ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const;
			void ToString(NN<Text::StringBuilderUTF8> sb) const;

			static Optional<JavaType> ParseType(InOutParam<UnsafeArray<UTF8Char>> typeStr, UnsafeArray<UTF8Char> endPtr);
		};
	}
}
#endif