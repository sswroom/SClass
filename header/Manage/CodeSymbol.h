#ifndef _SM_MANAGE_CODESYMBOL
#define _SM_MANAGE_CODESYMBOL
#include "Text/String.h"

namespace Manage
{
	class CodeSymbol
	{
	private:
		NN<Text::String> moduleName;
		NN<Text::String> funcName;
		OSInt ofst;
		UInt64 funcAddr;
	public:
		CodeSymbol(Text::CStringNN moduleName, Text::CStringNN funcName, OSInt ofst, UInt64 funcAddr);
		~CodeSymbol();

		UTF8Char *ToString(UTF8Char *buff) const;

		static Optional<CodeSymbol> ParseFromStr(const UTF8Char *buff, UInt64 funcAddr);
	};
}

#endif
