#ifndef _SM_TEXT_CPP_CPPPARSESTATUS
#define _SM_TEXT_CPP_CPPPARSESTATUS
#include "Data/ArrayListICaseString.h"
#include "Data/ArrayListNN.h"
#include "Data/FastStringMapNN.h"
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
				NN<Text::String> defineName;
				Optional<Text::String> defineVal;
				Optional<Text::String> defineParam;
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
				NN<Text::String> fileName;
				Int32 lineNum;
				Bool lineStart;
				ParserMode currMode;
				Int32 modeStatus;
				Data::ArrayList<Int32> ifValid;
				Data::ArrayList<ParserMode> pastModes;
				Text::StringBuilderUTF8 lineBuffSB;
				UTF8Char *lineBuffWS;
			} FileParseStatus;
		private:
			NN<Text::String> fileName;
			Data::FastStringMapNN<DefineInfo> defines;
			Data::ArrayListNN<FileParseStatus> statuses;
			Data::ArrayListICaseString fileNames;

			static void __stdcall FreeDefineInfo(NN<DefineInfo> definfo);
			static void __stdcall FreeFileStatus(NN<FileParseStatus> fileStatus);
		public:
			CppParseStatus(NN<Text::String> rootFile);
			CppParseStatus(Text::CStringNN rootFile);
			~CppParseStatus();

			Optional<FileParseStatus> GetFileStatus();
			Bool BeginParseFile(Text::CStringNN fileName);
			Bool EndParseFile(Text::CStringNN fileName);

			Bool IsDefined(Text::CStringNN defName);
			Bool AddGlobalDef(Text::CStringNN defName, Text::CString defVal);
			Bool AddDef(Text::CStringNN defName, Text::CString defParams, Text::CString defVal, Int32 lineNum);
			Bool Undefine(Text::CStringNN defName);
			Bool GetDefineVal(Text::CStringNN defName, Text::CString defParams, NN<Text::StringBuilderUTF8> sb);

			UOSInt GetDefineCount();
			Bool GetDefineInfo(UOSInt index, NN<DefineInfo> defInfo);

			UOSInt GetFileCount();
			Optional<Text::String> GetFileName(UOSInt index);
			NN<Text::String> GetCurrCodeFile() const;
		};
	}
}
#endif
