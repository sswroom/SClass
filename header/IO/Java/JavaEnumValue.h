#ifndef _SM_IO_JAVA_JAVAENUMVALUE
#define _SM_IO_JAVA_JAVAENUMVALUE
#include "IO/Java/JavaElementValue.h"
#include "IO/Java/JavaType.h"

namespace IO
{
	namespace Java
	{
		class JavaEnumValue : public JavaElementValue
		{
		private:
			NN<JavaType> type;
			NN<Text::String> constName;
		public:
			JavaEnumValue(NN<JavaType> type, Text::CStringNN constName);
			virtual ~JavaEnumValue();

			NN<JavaType> GetType() const;
			NN<Text::String> GetConstName() const;

			virtual void ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName);
			virtual ElementType GetElementType() const;
		};
	}
}
#endif