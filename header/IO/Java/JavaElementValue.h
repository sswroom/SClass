#ifndef _SM_IO_JAVA_JAVAELEMENTVALUE
#define _SM_IO_JAVA_JAVAELEMENTVALUE
#include "Data/ArrayListNN.hpp"
#include "IO/Java/JavaClass.h"

namespace IO
{
	namespace Java
	{
		class JavaAnnotation;
		class JavaElementValue
		{
		public:
			enum class ElementType
			{
				String,
				Annotation,
				Array,
				Integer
			};
		public:
			virtual ~JavaElementValue(){};

			virtual void ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) = 0;
			void ToString(NN<Text::StringBuilderUTF8> sb) { ToString(sb, nullptr, nullptr); }
			virtual ElementType GetElementType() const = 0;

			static Optional<JavaElementValue> ParseElementValue(NN<JavaClass> cls, InOutParam<UnsafeArray<const UInt8>> annoPtr, UnsafeArray<const UInt8> annoEnd);
			static Optional<JavaAnnotation> ParseAnnotation(NN<JavaClass> cls, InOutParam<UnsafeArray<const UInt8>> annoPtr, UnsafeArray<const UInt8> annoEnd);
			static UIntOS ParseAnnotations(NN<Data::ArrayListNN<JavaAnnotation>> annoList, NN<JavaClass> cls, UnsafeArray<const UInt8> annoPtr, UnsafeArray<const UInt8> annoEnd);
		};
	}
}
#endif