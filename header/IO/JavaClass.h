#ifndef _SM_IO_JAVACLASS
#define _SM_IO_JAVACLASS
#include "Data/ArrayList.h"
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
			UInt8 *code;
			UInt32 codeLen;
			UInt16 signatureIndex;
			Data::ArrayList<ExceptionHdlrInfo*> *exHdlrList;
			Data::ArrayList<LocalVariableInfo*> *lvList;
			Data::ArrayList<LocalVariableTypeInfo*> *lvtList;
			Data::ArrayList<LineNumberInfo*> *lineNumList;
			Data::ArrayList<UInt16> *exList;
		};
		
		struct DecompileEnv
		{
			Data::ArrayListStringNN *stacks;
			Data::ArrayListStringNN *stackTypes;
			Text::String **localTypes;
			const MethodInfo *method;
			const UInt8 *codeStart;
			const UInt8 *endPtr;
			Text::String *returnType;

			Data::ArrayListString *importList;
			const UTF8Char *packageName;
		};

	private:
		UInt8 *fileBuff;
		UOSInt fileBuffSize;
		UOSInt constPoolCnt;
		UInt8 **constPool;
		UInt16 accessFlags;
		UInt16 thisClass;
		UInt16 superClass;
		UOSInt interfaceCnt;
		UInt8 *interfaces;
		UOSInt fieldsCnt;
		UInt8 **fields;
		UOSInt methodCnt;
		UInt8 **methods;
		UOSInt attrCnt;
		UInt8 **attrs;
		UInt16 signatureIndex;

		static const UInt8 *Type2String(const UInt8 *typeStr, NotNullPtr<Text::StringBuilderUTF8> sb);
		static Text::CString CondType2String(CondType ct);
		static Text::CString CondType2IString(CondType ct);
		static void DetailAccessFlags(UInt16 accessFlags, NotNullPtr<Text::StringBuilderUTF8> sb);
		static void AppendCond(NotNullPtr<Text::StringBuilderUTF8> sb, DecompileEnv *env, UOSInt index, CondType ct, Bool inv);
		static UInt32 GetParamId(UInt32 paramIndex, const MethodInfo *method);
		void AppendIndent(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt lev) const;
		void AppendLineNum(NotNullPtr<Text::StringBuilderUTF8> sb, DecompileEnv *env, const UInt8 *codePtr) const;
		const UInt8 *DetailAttribute(const UInt8 *attr, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		void DetailConstVal(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb, Bool brankets) const;
		void DetailName(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb, Bool brankets) const;
		void DetailClassName(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		void DetailClassNameStr(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		void DetailFieldRef(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		void DetailMethodRef(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		Bool MethodGetReturnType(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		void DetailNameAndType(UInt16 index, UInt16 classIndex, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		void DetailNameType(UInt16 nameIndex, UInt16 typeIndex, UInt16 classIndex, const UTF8Char *prefix, NotNullPtr<Text::StringBuilderUTF8> sb, UTF8Char *typeBuff, MethodInfo *method, Data::ArrayListString *importList, const UTF8Char *packageName) const;
		void DetailType(UInt16 typeIndex, NotNullPtr<Text::StringBuilderUTF8> sb, Data::ArrayListString *importList, const UTF8Char *packageName) const;
		void DetailCode(const UInt8 *code, UOSInt codeLen, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		const UInt8 *DetailAnnotation(const UInt8 *annoPtr, const UInt8 *annoEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Data::ArrayListString *importList, const UTF8Char *packageName) const;
		const UInt8 *DetailElementValue(const UInt8 *annoPtr, const UInt8 *annoEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Data::ArrayListString *importList, const UTF8Char *packageName) const;
		const UInt8 *DetailStackMapFrame(const UInt8 *currPtr, const UInt8 *ptrEnd, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		const UInt8 *DetailVerificationTypeInfo(const UInt8 *currPtr, const UInt8 *ptrEnd, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		UTF8Char *GetConstName(UTF8Char *sbuff, UInt16 index) const;
		Bool ClassNameString(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		UTF8Char *GetLVName(UTF8Char *sbuff, UInt16 index, const MethodInfo *method, UOSInt codeOfst) const;
		UTF8Char *GetLVType(UTF8Char *sbuff, UInt16 index, const MethodInfo *method, UOSInt codeOfst, Data::ArrayListString *importList, const UTF8Char *packageName) const;

		Bool MethodParse(MethodInfo *method, const UInt8 *methodBuff) const;
		void MethodFree(MethodInfo *method) const;

		void AppendCodeClassName(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *className, Data::ArrayListString *importList, const UTF8Char *packageName) const;
		void AppendCodeClassContent(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt lev, const UTF8Char *className, Data::ArrayListString *importList, const UTF8Char *packageName);
		void AppendCodeField(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt index, Data::ArrayListString *importList, const UTF8Char *packageName) const;
		void AppendCodeMethod(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt index, UOSInt lev, Bool disasm, Bool decompile, Data::ArrayListString *importList, const UTF8Char *packageName) const;
		void AppendCodeMethodCodes(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt lev, Data::ArrayListString *importList, const UTF8Char *packageName, const UInt8 *codeAttr, const UTF8Char *typeBuff, const MethodInfo *method) const;
		static const UTF8Char *AppendCodeType2String(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *typeStr, Data::ArrayListString *importList, const UTF8Char *packageName);

		void Init(Data::ByteArrayR buff);
	public:
		JavaClass(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff);
		JavaClass(Text::CStringNN sourceName, Data::ByteArrayR buff);
		virtual ~JavaClass();

		virtual IO::ParserType GetParserType() const;

		Bool GetClassNameFull(NotNullPtr<Text::StringBuilderUTF8> sb) const;
		Bool GetSuperClass(NotNullPtr<Text::StringBuilderUTF8> sb) const;

		Bool FileStructDetail(NotNullPtr<Text::StringBuilderUTF8> sb) const;
		UOSInt FieldsGetCount() const;
		Bool FieldsGetDecl(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		UOSInt MethodsGetCount() const;
		Bool MethodsGetDecl(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		Bool MethodsGetDetail(UOSInt index, UOSInt lev, Bool disasm, NotNullPtr<Text::StringBuilderUTF8> sb) const;

		void DecompileFile(NotNullPtr<Text::StringBuilderUTF8> sb);
	private:
		EndType DecompileCode(const UInt8 *codePtr, const UInt8 *codeEnd, DecompileEnv *env, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		void DecompileLDC(UInt16 index, DecompileEnv *env) const;
		void DecompileStore(UInt16 index, DecompileEnv *env, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt codeOfst) const;
		EndType DecompileCondBranch(const UInt8 *codePtr, const UInt8 *codeEnd, CondType ct, DecompileEnv *env, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb) const;
		UTF8Char *DecompileMethod(UInt16 methodIndex, UTF8Char *nameBuff, UInt16 *classIndex, UTF8Char *retType, DecompileEnv *env, NotNullPtr<Text::StringBuilderUTF8> sb) const;
	public:
		static JavaClass *ParseFile(Text::CStringNN fileName);
		static JavaClass *ParseBuff(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff);
		static JavaClass *ParseBuff(Text::CStringNN sourceName, Data::ByteArrayR buff);
		static Text::CString EndTypeGetName(EndType et);
	};
}
#endif
