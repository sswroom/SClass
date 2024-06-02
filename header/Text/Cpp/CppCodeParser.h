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
				Data::StringMapNN<DefineInfo> *defines;
				Data::ArrayList<Int32> *ifValid;
				Text::StringBuilderW *sb;
			} ParseStatus;*/
		private:
			Text::Cpp::CppEnv *env;
			
			static UnsafeArray<UTF8Char> RemoveSpace(UnsafeArray<UTF8Char> sptr);
			void LogError(NN<Text::Cpp::CppParseStatus> status, Text::CStringNN errMsg, NN<Data::ArrayListStringNN> errMsgs);
			Bool ParseSharpIfParam(Text::CStringNN cond, NN<Text::Cpp::CppParseStatus> status, NN<Data::ArrayListStringNN> errMsgs, NN<Data::ArrayListStringNN> codePhases, UOSInt cpIndex);
			Bool EvalSharpIfVal(NN<Data::ArrayListStringNN> codePhases, NN<Text::Cpp::CppParseStatus> status, NN<Data::ArrayListStringNN> errMsgs, UOSInt cpIndex, OutParam<Int32> outVal, OSInt priority);
			Bool EvalSharpIf(Text::CStringNN cond, NN<Text::Cpp::CppParseStatus> status, NN<Data::ArrayListStringNN> errMsgs, OutParam<Bool> result);
			Bool ParseLine(UnsafeArray<UTF8Char> lineBuff, UnsafeArray<UTF8Char> lineBuffEnd, NN<Text::Cpp::CppParseStatus> status, NN<Data::ArrayListStringNN> errMsgs);
		public:
			CppCodeParser(Text::Cpp::CppEnv *env);
			~CppCodeParser();

			Bool ParseFile(Text::CStringNN fileName, NN<Data::ArrayListStringNN> errMsgs, NN<Text::Cpp::CppParseStatus> parseStatus);
			void FreeErrMsgs(NN<Data::ArrayListStringNN> errMsgs);
		};
	}
}
#endif
