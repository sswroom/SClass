#ifndef _SM_TEXT_CPP_CPPPARSESTATUS
#define _SM_TEXT_CPP_CPPPARSESTATUS
#include "Data/ArrayListICaseString.h"
#include "Data/FastStringMap.h"
#include "Data/StringUTF8Map.h"
#include "Text/String.h"
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
				Text::String *defineName;
				Text::String *defineVal;
				Text::String *defineParam;
				Text::String *fileName;
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
				Text::String *fileName;
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
			Text::String *fileName;
			Data::FastStringMap<DefineInfo*> *defines;
			Data::ArrayList<FileParseStatus*> *statuses;
			Data::ArrayListICaseString *fileNames;

			void FreeDefineInfo(DefineInfo *definfo);
			void FreeFileStatus(FileParseStatus *fileStatus);
		public:
			CppParseStatus(Text::String *rootFile);
			CppParseStatus(Text::CString rootFile);
			~CppParseStatus();

			FileParseStatus *GetFileStatus();
			Bool BeginParseFile(Text::CString fileName);
			Bool EndParseFile(const UTF8Char *fileName, UOSInt fileNameLen);

			Bool IsDefined(Text::CString defName);
			Bool AddGlobalDef(Text::CString defName, Text::CString defVal);
			Bool AddDef(Text::CString defName, Text::CString defParams, Text::CString defVal, Int32 lineNum);
			Bool Undefine(Text::CString defName);
			Bool GetDefineVal(Text::CString defName, Text::CString defParams, Text::StringBuilderUTF8 *sb);

			UOSInt GetDefineCount();
			Bool GetDefineInfo(UOSInt index, DefineInfo *defInfo);

			UOSInt GetFileCount();
			Text::String *GetFileName(UOSInt index);
			Text::String *GetCurrCodeFile();
		};
	}
}
#endif
