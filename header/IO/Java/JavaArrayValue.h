#ifndef _SM_IO_JAVA_JAVAARRAYVALUE
#define _SM_IO_JAVA_JAVAARRAYVALUE
#include "IO/Java/JavaElementValue.h"

namespace IO
{
	namespace Java
	{
		class JavaArrayValue : public JavaElementValue
		{
		private:
			Data::ArrayListNN<JavaElementValue> values;
		public:
			JavaArrayValue();
			virtual ~JavaArrayValue();

			void AddValue(NN<JavaElementValue> val);
			UIntOS GetCount() const;
			Optional<JavaElementValue> GetItem(UIntOS index) const;

			virtual void ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName);
			virtual ElementType GetElementType() const;
		};
	}
}
#endif