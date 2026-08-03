#ifndef _SM_IO_JAVA_JAVAANNOTATION
#define _SM_IO_JAVA_JAVAANNOTATION
#include "Data/ArrayListNN.hpp"
#include "IO/Java/JavaClass.h"
#include "IO/Java/JavaElementValue.h"
#include "IO/Java/JavaType.h"

namespace IO
{
	namespace Java
	{
		class JavaAnnotation : public JavaElementValue
		{
		private:
			NN<JavaType> type;
			Optional<Data::ArrayListStringNN> names;
			Optional<Data::ArrayListNN<JavaElementValue>> values;
		public:
			JavaAnnotation(NN<JavaType> type, Optional<Data::ArrayListStringNN> names, Optional<Data::ArrayListNN<JavaElementValue>> values);
			virtual ~JavaAnnotation();

			NN<JavaType> GetAnnoType() const;
			Optional<JavaElementValue> GetValue(Text::CStringNN name) const;

			virtual void ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName);
			virtual ElementType GetElementType() const;
		};
	}
}
#endif