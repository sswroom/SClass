#ifndef _SM_IO_JAVACLASS
#define _SM_IO_JAVACLASS
#include "Data/ArrayList.h"
#include "Data/ArrayListStrUTF8.h"
#include "IO/ParsedObject.h"
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
			Data::ArrayList<Text::String*> *stacks;
			Data::ArrayList<Text::String*> *stackTypes;
			Text::String **localTypes;
			const MethodInfo *method;
			const UInt8 *codeStart;
			const UInt8 *endPtr;
			Text::String *returnType;

			Data::ArrayListStrUTF8 *importList;
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

		static const UInt8 *Type2String(const UInt8 *typeStr, Text::StringBuilderUTF8 *sb);
		static const UInt8 *CondType2String(CondType ct);
		static const UInt8 *CondType2IString(CondType ct);
		static void DetailAccessFlags(UInt16 accessFlags, Text::StringBuilderUTF8 *sb);
		static void AppendCond(Text::StringBuilderUTF8 *sb, DecompileEnv *env, UOSInt index, CondType ct, Bool inv);
		static UInt32 GetParamId(UInt32 paramIndex, const MethodInfo *method);
		void AppendIndent(Text::StringBuilderUTF8 *sb, UOSInt lev);
		void AppendLineNum(Text::StringBuilderUTF8 *sb, DecompileEnv *env, const UInt8 *codePtr);
		const UInt8 *DetailAttribute(const UInt8 *attr, UOSInt lev, Text::StringBuilderUTF8 *sb);
		void DetailConstVal(UInt16 index, Text::StringBuilderUTF8 *sb, Bool brankets);
		void DetailName(UInt16 index, Text::StringBuilderUTF8 *sb, Bool brankets);
		void DetailClassName(UInt16 index, Text::StringBuilderUTF8 *sb);
		void DetailClassNameStr(UInt16 index, Text::StringBuilderUTF8 *sb);
		void DetailFieldRef(UInt16 index, Text::StringBuilderUTF8 *sb);
		void DetailMethodRef(UInt16 index, Text::StringBuilderUTF8 *sb);
		Bool MethodGetReturnType(UInt16 index, Text::StringBuilderUTF8 *sb);
		void DetailNameAndType(UInt16 index, UInt16 classIndex, Text::StringBuilderUTF8 *sb);
		void DetailNameType(UInt16 nameIndex, UInt16 typeIndex, UInt16 classIndex, const UTF8Char *prefix, Text::StringBuilderUTF8 *sb, UTF8Char *typeBuff, MethodInfo *method, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName);
		void DetailType(UInt16 typeIndex, Text::StringBuilderUTF8 *sb, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName);
		void DetailCode(const UInt8 *code, UOSInt codeLen, UOSInt lev, Text::StringBuilderUTF8 *sb);
		const UInt8 *DetailAnnotation(const UInt8 *annoPtr, const UInt8 *annoEnd, Text::StringBuilderUTF8 *sb, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName);
		const UInt8 *DetailElementValue(const UInt8 *annoPtr, const UInt8 *annoEnd, Text::StringBuilderUTF8 *sb, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName);
		const UInt8 *DetailStackMapFrame(const UInt8 *currPtr, const UInt8 *ptrEnd, UOSInt lev, Text::StringBuilderUTF8 *sb);
		const UInt8 *DetailVerificationTypeInfo(const UInt8 *currPtr, const UInt8 *ptrEnd, UOSInt lev, Text::StringBuilderUTF8 *sb);
		UTF8Char *GetConstName(UTF8Char *sbuff, UInt16 index);
		Bool ClassNameString(UInt16 index, Text::StringBuilderUTF8 *sb);
		UTF8Char *GetLVName(UTF8Char *sbuff, UInt16 index, const MethodInfo *method, UOSInt codeOfst);
		UTF8Char *GetLVType(UTF8Char *sbuff, UInt16 index, const MethodInfo *method, UOSInt codeOfst, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName);

		Bool MethodParse(MethodInfo *method, const UInt8 *methodBuff);
		void MethodFree(MethodInfo *method);

		void AppendCodeClassName(Text::StringBuilderUTF8 *sb, const UTF8Char *className, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName);
		void AppendCodeClassContent(Text::StringBuilderUTF8 *sb, UOSInt lev, const UTF8Char *className, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName);
		void AppendCodeField(Text::StringBuilderUTF8 *sb, UOSInt index, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName);
		void AppendCodeMethod(Text::StringBuilderUTF8 *sb, UOSInt index, UOSInt lev, Bool disasm, Bool decompile, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName);
		void AppendCodeMethodCodes(Text::StringBuilderUTF8 *sb, UOSInt lev, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName, const UInt8 *codeAttr, const UTF8Char *typeBuff, const MethodInfo *method);
		static const UTF8Char *AppendCodeType2String(Text::StringBuilderUTF8 *sb, const UTF8Char *typeStr, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName);

		void Init(const UInt8 *buff, UOSInt buffSize);
	public:
		JavaClass(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize);
		JavaClass(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
		virtual ~JavaClass();

		virtual IO::ParserType GetParserType();

		Bool GetClassNameFull(Text::StringBuilderUTF8 *sb);
		Bool GetSuperClass(Text::StringBuilderUTF8 *sb);

		Bool FileStructDetail(Text::StringBuilderUTF8 *sb);
		UOSInt FieldsGetCount();
		Bool FieldsGetDecl(UOSInt index, Text::StringBuilderUTF8 *sb);
		UOSInt MethodsGetCount();
		Bool MethodsGetDecl(UOSInt index, Text::StringBuilderUTF8 *sb);
		Bool MethodsGetDetail(UOSInt index, UOSInt lev, Bool disasm, Text::StringBuilderUTF8 *sb);

		void DecompileFile(Text::StringBuilderUTF8 *sb);
	private:
		EndType DecompileCode(const UInt8 *codePtr, const UInt8 *codeEnd, DecompileEnv *env, UOSInt lev, Text::StringBuilderUTF8 *sb);
		void DecompileLDC(UInt16 index, DecompileEnv *env);
		void DecompileStore(UInt16 index, DecompileEnv *env, UOSInt lev, Text::StringBuilderUTF8 *sb, UOSInt codeOfst);
		EndType DecompileCondBranch(const UInt8 *codePtr, const UInt8 *codeEnd, CondType ct, DecompileEnv *env, UOSInt lev, Text::StringBuilderUTF8 *sb);
		UTF8Char *DecompileMethod(UInt16 methodIndex, UTF8Char *nameBuff, UInt16 *classIndex, UTF8Char *retType, DecompileEnv *env, Text::StringBuilderUTF8 *sb);
	public:
		static JavaClass *ParseFile(const UTF8Char *fileName);
		static JavaClass *ParseBuff(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize);
		static JavaClass *ParseBuff(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
		static const UTF8Char *EndTypeGetName(EndType et);
	};
}
#endif
