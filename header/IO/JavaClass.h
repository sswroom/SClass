#ifndef _SM_IO_JAVACLASS
#define _SM_IO_JAVACLASS
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListString.h"
#include "Data/ArrayListStringNN.h"
#include "Data/ArrayListStrUTF8.h"
#include "IO/ParsedObject.h"
#include "Text/CString.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class JavaClass : public IO::ParsedObject
	{
	private:
		enum class EndType
		{
			Error,
			CodeEnd,
			Return,
			Goto,
			Throw
		};

		enum class CondType
		{
			EQ,
			NE,
			LE,
			GE,
			LT,
			GT
		};

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
		UInt8 *fileBuff;
		UIntOS fileBuffSize;
		UIntOS constPoolCnt;
		UInt8 **constPool;
		UInt16 accessFlags;
		UInt16 thisClass;
		UInt16 superClass;
		UIntOS interfaceCnt;
		UInt8 *interfaces;
		UIntOS fieldsCnt;
		UInt8 **fields;
		UIntOS methodCnt;
		UInt8 **methods;
		UIntOS attrCnt;
		UInt8 **attrs;
		UInt16 signatureIndex;

		static UnsafeArray<const UInt8> Type2String(UnsafeArray<const UInt8> typeStr, NN<Text::StringBuilderUTF8> sb);
		static Text::CStringNN CondType2String(CondType ct);
		static Text::CStringNN CondType2IString(CondType ct);
		static void DetailAccessFlags(UInt16 accessFlags, NN<Text::StringBuilderUTF8> sb);
		static void AppendCond(NN<Text::StringBuilderUTF8> sb, NN<DecompileEnv> env, UIntOS index, CondType ct, Bool inv);
		static UInt32 GetParamId(UInt32 paramIndex, Optional<const MethodInfo> method);
		void AppendIndent(NN<Text::StringBuilderUTF8> sb, UIntOS lev) const;
		void AppendLineNum(NN<Text::StringBuilderUTF8> sb, NN<DecompileEnv> env, UnsafeArray<const UInt8> codePtr) const;
		UnsafeArray<const UInt8> DetailAttribute(UnsafeArray<const UInt8> attr, UIntOS lev, NN<Text::StringBuilderUTF8> sb) const;
		void DetailConstVal(UInt16 index, NN<Text::StringBuilderUTF8> sb, Bool brankets) const;
		void DetailName(UInt16 index, NN<Text::StringBuilderUTF8> sb, Bool brankets) const;
		void DetailClassName(UInt16 index, NN<Text::StringBuilderUTF8> sb) const;
		void DetailClassNameStr(UInt16 index, NN<Text::StringBuilderUTF8> sb) const;
		void DetailFieldRef(UInt16 index, NN<Text::StringBuilderUTF8> sb) const;
		void DetailMethodRef(UInt16 index, NN<Text::StringBuilderUTF8> sb) const;
		Bool MethodGetReturnType(UInt16 index, NN<Text::StringBuilderUTF8> sb) const;
		void DetailNameAndType(UInt16 index, UInt16 classIndex, NN<Text::StringBuilderUTF8> sb) const;
		void DetailNameType(UInt16 nameIndex, UInt16 typeIndex, UInt16 classIndex, UnsafeArrayOpt<const UTF8Char> prefix, NN<Text::StringBuilderUTF8> sb, UnsafeArrayOpt<UTF8Char> typeBuff, Optional<MethodInfo> method, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const;
		void DetailType(UInt16 typeIndex, NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const;
		void DetailCode(UnsafeArray<const UInt8> code, UIntOS codeLen, UIntOS lev, NN<Text::StringBuilderUTF8> sb) const;
		UnsafeArray<const UInt8> DetailAnnotation(UnsafeArray<const UInt8> annoPtr, UnsafeArray<const UInt8> annoEnd, NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const;
		UnsafeArray<const UInt8> DetailElementValue(UnsafeArray<const UInt8> annoPtr, UnsafeArray<const UInt8> annoEnd, NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const;
		UnsafeArray<const UInt8> DetailStackMapFrame(UnsafeArray<const UInt8> currPtr, UnsafeArray<const UInt8> ptrEnd, UIntOS lev, NN<Text::StringBuilderUTF8> sb) const;
		UnsafeArray<const UInt8> DetailVerificationTypeInfo(UnsafeArray<const UInt8> currPtr, UnsafeArray<const UInt8> ptrEnd, UIntOS lev, NN<Text::StringBuilderUTF8> sb) const;
		UnsafeArrayOpt<UTF8Char> GetConstName(UnsafeArray<UTF8Char> sbuff, UInt16 index) const;
		Bool ClassNameString(UInt16 index, NN<Text::StringBuilderUTF8> sb) const;
		UnsafeArray<UTF8Char> GetLVName(UnsafeArray<UTF8Char> sbuff, UInt16 index, NN<const MethodInfo> method, UIntOS codeOfst) const;
		UnsafeArray<UTF8Char> GetLVType(UnsafeArray<UTF8Char> sbuff, UInt16 index, NN<const MethodInfo> method, UIntOS codeOfst, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const;

		Bool MethodParse(NN<MethodInfo> method, UnsafeArray<const UInt8> methodBuff) const;
		void MethodFree(NN<MethodInfo> method) const;

		void AppendCodeClassName(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> className, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const;
		void AppendCodeClassContent(NN<Text::StringBuilderUTF8> sb, UIntOS lev, UnsafeArray<const UTF8Char> className, NN<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName);
		void AppendCodeField(NN<Text::StringBuilderUTF8> sb, UIntOS index, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const;
		void AppendCodeMethod(NN<Text::StringBuilderUTF8> sb, UIntOS index, UIntOS lev, Bool disasm, Bool decompile, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const;
		void AppendCodeMethodCodes(NN<Text::StringBuilderUTF8> sb, UIntOS lev, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName, UnsafeArray<const UInt8> codeAttr, UnsafeArray<const UTF8Char> typeBuff, NN<const MethodInfo> method) const;
		static UnsafeArray<const UTF8Char> AppendCodeType2String(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> typeStr, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName);

		void Init(Data::ByteArrayR buff);
	public:
		JavaClass(NN<Text::String> sourceName, Data::ByteArrayR buff);
		JavaClass(Text::CStringNN sourceName, Data::ByteArrayR buff);
		virtual ~JavaClass();

		virtual IO::ParserType GetParserType() const;

		Bool GetClassNameFull(NN<Text::StringBuilderUTF8> sb) const;
		Bool GetSuperClass(NN<Text::StringBuilderUTF8> sb) const;

		Bool FileStructDetail(NN<Text::StringBuilderUTF8> sb) const;
		UIntOS FieldsGetCount() const;
		Bool FieldsGetDecl(UIntOS index, NN<Text::StringBuilderUTF8> sb) const;
		UIntOS MethodsGetCount() const;
		Bool MethodsGetDecl(UIntOS index, NN<Text::StringBuilderUTF8> sb) const;
		Bool MethodsGetDetail(UIntOS index, UIntOS lev, Bool disasm, NN<Text::StringBuilderUTF8> sb) const;

		void DecompileFile(NN<Text::StringBuilderUTF8> sb);
	private:
		EndType DecompileCode(UnsafeArray<const UInt8> codePtr, UnsafeArray<const UInt8> codeEnd, NN<DecompileEnv> env, UIntOS lev, NN<Text::StringBuilderUTF8> sb) const;
		void DecompileLDC(UInt16 index, NN<DecompileEnv> env) const;
		void DecompileStore(UInt16 index, NN<DecompileEnv> env, UIntOS lev, NN<Text::StringBuilderUTF8> sb, UIntOS codeOfst) const;
		EndType DecompileCondBranch(UnsafeArray<const UInt8> codePtr, UnsafeArray<const UInt8> codeEnd, CondType ct, NN<DecompileEnv> env, UIntOS lev, NN<Text::StringBuilderUTF8> sb) const;
		UnsafeArrayOpt<UTF8Char> DecompileMethod(UInt16 methodIndex, UnsafeArray<UTF8Char> nameBuff, OutParam<UInt16> classIndex, UnsafeArray<UTF8Char> retType, NN<DecompileEnv> env, NN<Text::StringBuilderUTF8> sb) const;
	public:
		static Optional<JavaClass> ParseFile(Text::CStringNN fileName);
		static Optional<JavaClass> ParseBuff(NN<Text::String> sourceName, Data::ByteArrayR buff);
		static Optional<JavaClass> ParseBuff(Text::CStringNN sourceName, Data::ByteArrayR buff);
		static Text::CStringNN EndTypeGetName(EndType et);
	};
}
#endif
