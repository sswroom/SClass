#ifndef _SM_IO_JAVA_JAVAMETHOD
#define _SM_IO_JAVA_JAVAMETHOD
#include "IO/Java/JavaAnnotation.h"
#include "IO/Java/JavaClass.h"
#include "IO/Java/JavaType.h"

namespace IO
{
	namespace Java
	{
		class JavaMethod
		{
		private:
			UInt16 accessFlags;
			NN<Text::String> name;
			NN<JavaType> returnType;
			Data::ArrayListNN<JavaType> paramTypes;
			Data::ArrayListStringNN paramNames;
			Data::ArrayListNN<JavaAnnotation> annoList;
		public:
			JavaMethod(UInt16 accessFlags, Text::CStringNN name, NN<JavaType> returnType);
			~JavaMethod();

			void AddParam(NN<JavaType> paramType, Text::CStringNN paramName);
			void AddAnnotation(NN<JavaAnnotation> anno);
			UInt16 GetAccessFlags() const;
			NN<Text::String> GetName() const;
			NN<JavaType> GetReturnType() const;
			UIntOS GetAnnotationCount() const;
			Optional<JavaAnnotation> GetAnnotation(UIntOS index) const;

			void ToAnnotation(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const;
			void ToDeclaration(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const;
			void ToDeclarationNameParams(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const;

			static Optional<JavaMethod> ParseMethod(NN<JavaClass> cls, UnsafeArray<const UInt8> methodPtr);
		};
	}
}
#endif