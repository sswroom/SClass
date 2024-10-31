#ifndef _SM_TEXT_STRINGCASECONVERTER
#define _SM_TEXT_STRINGCASECONVERTER
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	enum class CaseType
	{
		CamelCase,
		PascalCase,
		SnakeCase,
		KebabCase,
		ScreamingSnakeCase,
		UpperCase,
		LowerCase,
		BlockCase
	};

	Text::CStringNN CaseTypeGetSample(CaseType caseType);
	Text::CStringNN CaseTypeGetName(CaseType caseType);

	class StringCaseConverter
	{
	private:
		CaseType caseType;

		UnsafeArray<const UTF8Char> ReadChar(UnsafeArray<const UTF8Char> buff, OutParam<UTF8Char> c, OutParam<Bool> hasSpace) const;
	public:
		StringCaseConverter(CaseType srcCaseType);
		~StringCaseConverter();

		void Convert(UnsafeArray<const UTF8Char> srcStr, CaseType outCaseType, NN<Text::StringBuilderUTF8> sbOut) const;
	};
}
#endif
