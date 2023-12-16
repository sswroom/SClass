#ifndef _SM_TEXT_CPP_CPPCODEPARSER
#define _SM_TEXT_CPP_CPPCODEPARSER
#include "Data/ArrayListStringNN.h"
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
			void LogError(Text::Cpp::CppParseStatus *status, const UTF8Char *errMsg, UOSInt msgLen, Data::ArrayListStringNN *errMsgs);
			Bool ParseSharpIfParam(Text::CString cond, Text::Cpp::CppParseStatus *status, Data::ArrayListStringNN *errMsgs, Data::ArrayListStringNN *codePhases, UOSInt cpIndex);
			Bool EvalSharpIfVal(Data::ArrayListStringNN *codePhases, Text::Cpp::CppParseStatus *status, Data::ArrayListStringNN *errMsgs, UOSInt cpIndex, Int32 *outVal, OSInt priority);
			Bool EvalSharpIf(Text::CString cond, Text::Cpp::CppParseStatus *status, Data::ArrayListStringNN *errMsgs, Bool *result);
			Bool ParseLine(UTF8Char *lineBuff, UTF8Char *lineBuffEnd, Text::Cpp::CppParseStatus *status, Data::ArrayListStringNN *errMsgs);
		public:
			CppCodeParser(Text::Cpp::CppEnv *env);
			~CppCodeParser();

			Bool ParseFile(const UTF8Char *fileName, UOSInt fileNameLen, Data::ArrayListStringNN *errMsgs, Text::Cpp::CppParseStatus *parseStatus);
			void FreeErrMsgs(Data::ArrayListStringNN *errMsgs);
		};
	}
}
#endif
