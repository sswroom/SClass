#ifndef _SM_TEXT_CPP_CPPDEMANGLER
#define _SM_TEXT_CPP_CPPDEMANGLER
#include "Text/PString.h"

namespace Text
{
	namespace Cpp
	{
		class CppDemangler
		{
		private:
			struct ParseEnv
			{
				UTF8Char *sbuff;
				const UTF8Char *funcName;
				Bool valid;
				Bool constFunc;
				UOSInt strId;
				Text::PString substr[32];
				UOSInt nstrId;
				Text::PString nstr[32];
				UOSInt tplId;
				Text::PString tplstr[4];
			};

			static void AppendStr(NN<ParseEnv> env, UTF8Char *strStart);
			static void AppendNStr(NN<ParseEnv> env, UTF8Char *strStart);
			static void AppendTpl(NN<ParseEnv> env, UTF8Char *strStart);
			static Bool ParseType(NN<ParseEnv> env, Bool firstPart);
		public:
			static UTF8Char *ToFuncName(UTF8Char *sbuff, const UTF8Char *funcName);
		};
	}
}
#endif
