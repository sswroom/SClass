#ifndef _SM_TEXT_CPP_CPPPARSESTATUS
#define _SM_TEXT_CPP_CPPPARSESTATUS
#include "Data/StringUTF8Map.h"
#include "Data/ArrayListICaseStrUTF8.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	namespace Cpp
	{
		class CppParseStatus
		{
		public:
//Define info			
//http://msdn.microsoft.com/en-us/library/b0084kay.aspx
			typedef struct
			{
				const UTF8Char *defineName;
				const UTF8Char *defineVal;
				const UTF8Char *defineParam;
				const UTF8Char *fileName;
				Int32 lineNum;
				Bool undefined;
			} DefineInfo;

			typedef enum
			{
				PM_NORMAL,
				PM_SHARP,
				PM_COMMENTPARA,
				PM_INCLUDE,
				PM_INCLUDEQUOTE,
				PM_INCLUDEARROW,
				PM_PRAGMA,
				PM_IFNDEF,
				PM_IFDEF,
				PM_ELIF,
				PM_IF,
				PM_DEFINE,
				PM_UNDEF,
				PM_ERROR,
				PM_SHARPEND,
				PM_QUOTE,
				PM_SQUOTE
			} ParserMode;

			typedef struct
			{
				const UTF8Char *fileName;
				Int32 lineNum;
				Bool lineStart;
				ParserMode currMode;
				Int32 modeStatus;
				Data::ArrayList<Int32> *ifValid;
				Data::ArrayList<ParserMode> *pastModes;
				Text::StringBuilderUTF8 *lineBuffSB;
				UTF8Char *lineBuffWS;
			} FileParseStatus;
		private:
			const UTF8Char *fileName;
			Data::StringUTF8Map<DefineInfo*> *defines;
			Data::ArrayList<FileParseStatus*> *statuses;
			Data::ArrayListICaseStrUTF8 *fileNames;

			void FreeDefineInfo(DefineInfo *definfo);
			void FreeFileStatus(FileParseStatus *fileStatus);
		public:
			CppParseStatus(const UTF8Char *rootFile);
			~CppParseStatus();

			FileParseStatus *GetFileStatus();
			Bool BeginParseFile(const UTF8Char *fileName);
			Bool EndParseFile(const UTF8Char *fileName);

			Bool IsDefined(const UTF8Char *defName);
			Bool AddGlobalDef(const UTF8Char *defName, const UTF8Char *defVal);
			Bool AddDef(const UTF8Char *defName, const UTF8Char *defParams, const UTF8Char *defVal, Int32 lineNum);
			Bool Undefine(const UTF8Char *defName);
			Bool GetDefineVal(const UTF8Char *defName, const UTF8Char *defParams, Text::StringBuilderUTF *sb);

			OSInt GetDefineCount();
			Bool GetDefineInfo(OSInt index, DefineInfo *defInfo);

			OSInt GetFileCount();
			const UTF8Char *GetFileName(OSInt index);
			const UTF8Char *GetCurrCodeFile();
		};
	};
};
#endif
