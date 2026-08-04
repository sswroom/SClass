#ifndef _SM_IO_JAVA_JAVACLASSVALUE
#define _SM_IO_JAVA_JAVACLASSVALUE
#include "IO/Java/JavaElementValue.h"
#include "IO/Java/JavaType.h"

namespace IO
{
	namespace Java
	{
		class JavaClassValue : public JavaElementValue
		{
		private:
			NN<JavaType> type;
		public:
			JavaClassValue(NN<JavaType> type);
			virtual ~JavaClassValue();

			NN<JavaType> GetType() const;

			virtual void ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName);
			virtual ElementType GetElementType() const;
		};
	}
}
#endif