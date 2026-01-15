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
				UnsafeArray<UTF8Char> sbuff;
				UnsafeArray<const UTF8Char> funcName;
				Bool valid;
				Bool constFunc;
				UIntOS strId;
				Text::PString substr[32];
				UIntOS nstrId;
				Text::PString nstr[32];
				UIntOS tplId;
				Text::PString tplstr[4];
			};

			static void AppendStr(NN<ParseEnv> env, UnsafeArray<UTF8Char> strStart);
			static void AppendNStr(NN<ParseEnv> env, UnsafeArray<UTF8Char> strStart);
			static void AppendTpl(NN<ParseEnv> env, UnsafeArray<UTF8Char> strStart);
			static Bool ParseType(NN<ParseEnv> env, Bool firstPart);
		public:
			static UnsafeArray<UTF8Char> ToFuncName(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> funcName);
		};
	}
}
#endif
