#ifndef _SM_TEXT_CPP_CPPCODEPARSER
#define _SM_TEXT_CPP_CPPCODEPARSER
#include "Data/StringUTF8Map.h"
#include "Text/StringBuilder.h"
#include "Text/Cpp/CppParseStatus.h"

namespace Text
{
	namespace Cpp
	{
		class CppEnv;

		class CppCodeParser
		{
		private:

		public:
/*			typedef struct
			{
				const WChar *fileName;
				Int32 lineNum;
				ParserMode currMode;
				Int32 modeStatus;
				Bool lineStart;
				Data::ArrayList<ParserMode> *pastModes;
				Data::StringMap<DefineInfo*> *defines;
				Data::ArrayList<Int32> *ifValid;
				Text::StringBuilderW *sb;
			} ParseStatus;*/
		private:
			Text::Cpp::CppEnv *env;
			
			static UTF8Char *RemoveSpace(UTF8Char *sptr);
			void LogError(Text::Cpp::CppParseStatus *status, const UTF8Char *errMsg, Data::ArrayListStrUTF8 *errMsgs);
			Bool ParseSharpIfParam(const UTF8Char *cond, Text::Cpp::CppParseStatus *status, Data::ArrayListStrUTF8 *errMsgs, Data::ArrayList<const UTF8Char *> *codePhases, UOSInt cpIndex);
			Bool EvalSharpIfVal(Data::ArrayList<const UTF8Char *> *codePhases, Text::Cpp::CppParseStatus *status, Data::ArrayListStrUTF8 *errMsgs, UOSInt cpIndex, Int32 *outVal, OSInt priority);
			Bool EvalSharpIf(const UTF8Char *cond, Text::Cpp::CppParseStatus *status, Data::ArrayListStrUTF8 *errMsgs, Bool *result);
			Bool ParseLine(UTF8Char *lineBuff, Text::Cpp::CppParseStatus *status, Data::ArrayListStrUTF8 *errMsgs);
		public:
			CppCodeParser(Text::Cpp::CppEnv *env);
			~CppCodeParser();

			Bool ParseFile(const UTF8Char *fileName, Data::ArrayListStrUTF8 *errMsgs, Text::Cpp::CppParseStatus *parseStatus);
			void FreeErrMsgs(Data::ArrayListStrUTF8 *errMsgs);
		};
	}
}
#endif
