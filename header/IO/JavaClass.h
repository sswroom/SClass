#ifndef _SM_IO_JAVACLASS
#define _SM_IO_JAVACLASS
#include "Data/ArrayList.h"
#include "IO/ParsedObject.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	class JavaClass : public IO::ParsedObject
	{
	private:
		typedef enum
		{
			ET_ERROR,
			ET_CODEEND,
			ET_RETURN,
			ET_GOTO,
			ET_THROW
		} EndType;

		typedef enum
		{
			CT_EQ,
			CT_NE,
			CT_LE,
			CT_GE,
			CT_LT,
			CT_GT
		} CondType;

		typedef struct
		{
			Data::ArrayList<const UTF8Char*> *stacks;
			Data::ArrayList<const UTF8Char*> *stackTypes;
			const UTF8Char **localTypes;
			const UTF8Char *lvTable;
			UOSInt lvTableLen;
			const UInt8 *codeStart;
			Bool staticFunc;
			const UInt8 *endPtr;
		} DecompileEnv;
		
	private:
		UInt8 *fileBuff;
		OSInt fileBuffSize;
		OSInt constPoolCnt;
		UInt8 **constPool;
		UInt16 accessFlags;
		UInt16 thisClass;
		UInt16 superClass;
		OSInt interfaceCnt;
		UInt8 *interfaces;
		OSInt fieldsCnt;
		UInt8 **fields;
		OSInt methodCnt;
		UInt8 **methods;
		OSInt attrCnt;
		UInt8 **attrs;

		static const UInt8 *Type2String(const UInt8 *typeStr, Text::StringBuilderUTF *sb);
		static const UInt8 *CondType2String(CondType ct);
		static const UInt8 *CondType2IString(CondType ct);
		static void DetailAccessFlags(UInt16 accessFlags, Text::StringBuilderUTF *sb);
		const UInt8 *DetailAttribute(const UInt8 *attr, OSInt lev, Text::StringBuilderUTF *sb);
		void DetailConstVal(UInt16 index, Text::StringBuilderUTF *sb, Bool brankets);
		void DetailName(UInt16 index, Text::StringBuilderUTF *sb, Bool brankets);
		void DetailClassName(UInt16 index, Text::StringBuilderUTF *sb);
		void DetailClassNameStr(UInt16 index, Text::StringBuilderUTF *sb);
		void DetailFieldRef(UInt16 index, Text::StringBuilderUTF *sb);
		void DetailMethodRef(UInt16 index, Text::StringBuilderUTF *sb);
		void DetailNameAndType(UInt16 index, UInt16 classIndex, Text::StringBuilderUTF *sb);
		void DetailNameType(UInt16 nameIndex, UInt16 typeIndex, UInt16 classIndex, const UTF8Char *prefix, Text::StringBuilderUTF *sb, UTF8Char *typeBuff, const UInt8 *lvTable, UOSInt lvTableLen, Bool isStatic);
		void DetailType(UInt16 typeIndex, Text::StringBuilderUTF *sb);
		void DetailCode(const UInt8 *code, OSInt codeLen, OSInt lev, Text::StringBuilderUTF *sb);
		const UInt8 *DetailAnnotation(const UInt8 *annoPtr, const UInt8 *annoEnd, Text::StringBuilderUTF *sb);
		const UInt8 *DetailElementValue(const UInt8 *annoPtr, const UInt8 *annoEnd, Text::StringBuilderUTF *sb);
		UTF8Char *GetConstName(UTF8Char *sbuff, UInt16 index);
		void ClassNameString(UInt16 index, Text::StringBuilderUTF *sb);
		UTF8Char *GetLVName(UTF8Char *sbuff, UInt16 index, const UInt8 *lvTable, UOSInt lvTableLen, UOSInt codeOfst);
	public:
		JavaClass(const UTF8Char *sourceName, const UInt8 *buff, OSInt buffSize);
		virtual ~JavaClass();

		virtual IO::ParsedObject::ParserType GetParserType();

		Bool FileStructDetail(Text::StringBuilderUTF *sb);
		OSInt FieldsGetCount();
		Bool FieldsGetDecl(OSInt index, Text::StringBuilderUTF *sb);
		OSInt MethodsGetCount();
		Bool MethodsGetDecl(OSInt index, Text::StringBuilderUTF *sb);
		Bool MethodsGetDetail(OSInt index, OSInt lev, Bool disasm, Text::StringBuilderUTF *sb);

		void Decompile(const UInt8 *codeAttr, Bool staticFunc, const UTF8Char *typeBuff, const UInt8 *lvTable, UOSInt lvTableLen, OSInt lev, Text::StringBuilderUTF *sb);
	private:
		EndType DecompileCode(const UInt8 *codePtr, const UInt8 *codeEnd, DecompileEnv *env, OSInt lev, Text::StringBuilderUTF *sb);
		void DecompileLDC(UInt16 index, DecompileEnv *env);
		void DecompileStore(UInt16 index, DecompileEnv *env, OSInt lev, Text::StringBuilderUTF *sb, UOSInt codeOfst);
		EndType DecompileCondBranch(const UInt8 *codePtr, const UInt8 *codeEnd, CondType ct, DecompileEnv *env, OSInt lev, Text::StringBuilderUTF *sb);
		UTF8Char *DecompileMethod(UInt16 methodIndex, UTF8Char *nameBuff, UInt16 *classIndex, UTF8Char *retType, DecompileEnv *env, Text::StringBuilderUTF *sb);
	public:
		static JavaClass *ParseFile(const UTF8Char *fileName);
		static JavaClass *ParseBuff(const UTF8Char *sourceName, const UInt8 *buff, OSInt buffSize);
	};
}
#endif
