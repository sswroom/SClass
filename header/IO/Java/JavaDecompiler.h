#ifndef _SM_IO_JAVA_JAVADECOMPILER
#define _SM_IO_JAVA_JAVADECOMPILER
#include "IO/Java/JavaClass.h"

namespace IO
{
	namespace Java
	{
		class JavaDecompiler
		{
		private:
			struct ExceptionHdlrInfo
			{
				UInt16 startPC;
				UInt16 endPC;
				UInt16 handlerPC;
				UInt16 catchType;
			};

			struct LocalVariableInfo
			{
				UInt16 startPC;
				UInt16 length;
				UInt16 nameIndex;
				UInt16 descriptorIndex;
				UInt16 index;
			};

			struct LocalVariableTypeInfo
			{
				UInt16 startPC;
				UInt16 length;
				UInt16 nameIndex;
				UInt16 signatureIndex;
				UInt16 index;
			};

			struct LineNumberInfo
			{
				UInt16 startPC;
				UInt16 lineNumber;
			};

			struct MethodInfo
			{
				UInt16 accessFlags;
				UInt16 nameIndex;
				UInt16 descriptorIndex;
				UInt16 maxStacks;
				UInt16 maxLocals;
				UnsafeArrayOpt<UInt8> code;
				UInt32 codeLen;
				UInt16 signatureIndex;
				Data::ArrayListNN<ExceptionHdlrInfo> exHdlrList;
				Data::ArrayListNN<LocalVariableInfo> lvList;
				Data::ArrayListNN<LocalVariableTypeInfo> lvtList;
				Data::ArrayListNN<LineNumberInfo> lineNumList;
				Data::ArrayListNative<UInt16> exList;
			};
			
			struct DecompileEnv
			{
				NN<Data::ArrayListStringNN> stacks;
				NN<Data::ArrayListStringNN> stackTypes;
				UnsafeArray<Optional<Text::String>> localTypes;
				NN<const MethodInfo> method;
				UnsafeArray<const UInt8> codeStart;
				UnsafeArray<const UInt8> endPtr;
				Optional<Text::String> returnType;

				Optional<Data::ArrayListStringNN> importList;
				UnsafeArrayOpt<const UTF8Char> packageName;
			};

		private:
			void AppendIndent(NN<Text::StringBuilderUTF8> sb, UIntOS lev);
			void AppendCodeClassContent(NN<Text::StringBuilderUTF8> sb, NN<IO::Java::JavaClass> cls, UIntOS lev, UnsafeArray<const UTF8Char> className, NN<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName);

		public:
			JavaDecompiler();
			~JavaDecompiler();

			Bool Decompile(NN<Text::StringBuilderUTF8> sb, NN<IO::Java::JavaClass> cls);
		};
	}
}
#endif
