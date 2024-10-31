#include "Stdafx.h"
#include "Text/CharUtil.h"
#include "Text/StringCaseConverter.h"

Text::CStringNN Text::CaseTypeGetSample(CaseType caseType)
{
	switch (caseType)
	{
	case CaseType::CamelCase:
		return CSTR("camelCase");
	case CaseType::PascalCase:
		return CSTR("PascalCase");
	case CaseType::SnakeCase:
		return CSTR("snake_case");
	case CaseType::KebabCase:
		return CSTR("kebab-case");
	case CaseType::ScreamingSnakeCase:
		return CSTR("SCREAMING_SNAKE_CASE");
	case CaseType::UpperCase:
		return CSTR("Upper Case");
	case CaseType::LowerCase:
		return CSTR("lower case");
	case CaseType::BlockCase:
		return CSTR("BLOCK CASE");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Text::CaseTypeGetName(CaseType caseType)
{
	switch (caseType)
	{
	case CaseType::CamelCase:
		return CSTR("CamelCase");
	case CaseType::PascalCase:
		return CSTR("PascalCase");
	case CaseType::SnakeCase:
		return CSTR("SnakeCase");
	case CaseType::KebabCase:
		return CSTR("KebabCase");
	case CaseType::ScreamingSnakeCase:
		return CSTR("ScreamingSnakeCase");
	case CaseType::UpperCase:
		return CSTR("UpperCase");
	case CaseType::LowerCase:
		return CSTR("LowerCase");
	case CaseType::BlockCase:
		return CSTR("BlockCase");
	default:
		return CSTR("Unknown");
	}
}

UnsafeArray<const UTF8Char> Text::StringCaseConverter::ReadChar(UnsafeArray<const UTF8Char> buff, OutParam<UTF8Char> c, OutParam<Bool> hasSpace) const
{
	Bool space = false;
	UTF8Char ch;
	if (this->caseType == CaseType::CamelCase || this->caseType == CaseType::PascalCase)
	{
		while (true)
		{
			ch = *buff++;
			if (ch == 0)
			{
				hasSpace.Set(space);
				c.Set(0);
				return buff;
			}
			else if (ch >= 'A' && ch <= 'Z')
			{
				hasSpace.Set(true);
				c.Set(ch);
				return buff;
			}
			else if ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= 9))
			{
				hasSpace.Set(space);
				c.Set(ch);
				return buff;
			}
			else
			{
				space = true;
			}
		}
	}
	else
	{
		while (true)
		{
			ch = *buff++;
			if (ch == 0)
			{
				hasSpace.Set(space);
				c.Set(0);
				return buff;
			}
			else if (Text::CharUtil::IsAlphaNumeric(ch))
			{
				hasSpace.Set(space);
				c.Set(ch);
				return buff;
			}
			else
			{
				space = true;
			}
		}
	}
}

Text::StringCaseConverter::StringCaseConverter(CaseType srcCaseType)
{
	this->caseType = srcCaseType;
}

Text::StringCaseConverter::~StringCaseConverter()
{
}

void Text::StringCaseConverter::Convert(UnsafeArray<const UTF8Char> srcStr, CaseType outCaseType, NN<Text::StringBuilderUTF8> sbOut) const
{
	UTF8Char c;
	Bool hasSpace;
	switch (outCaseType)
	{
	case CaseType::CamelCase:
		srcStr = this->ReadChar(srcStr, c, hasSpace);
		if (c == 0) return;
		sbOut->AppendUTF8Char(Text::CharUtil::ToLower(c));
		while (true)
		{
			srcStr = this->ReadChar(srcStr, c, hasSpace);
			if (c == 0) return;
			if (hasSpace)
				sbOut->AppendUTF8Char(Text::CharUtil::ToUpper(c));
			else
				sbOut->AppendUTF8Char(Text::CharUtil::ToLower(c));
		}
		break;
	case CaseType::PascalCase:
		srcStr = this->ReadChar(srcStr, c, hasSpace);
		if (c == 0) return;
		sbOut->AppendUTF8Char(Text::CharUtil::ToUpper(c));
		while (true)
		{
			srcStr = this->ReadChar(srcStr, c, hasSpace);
			if (c == 0) return;
			if (hasSpace)
				sbOut->AppendUTF8Char(Text::CharUtil::ToUpper(c));
			else
				sbOut->AppendUTF8Char(Text::CharUtil::ToLower(c));
		}
		break;
	case CaseType::SnakeCase:
		while (true)
		{
			srcStr = this->ReadChar(srcStr, c, hasSpace);
			if (c == 0) return;
			if (hasSpace)
				sbOut->AppendUTF8Char('_');
			sbOut->AppendUTF8Char(Text::CharUtil::ToLower(c));
		}
		break;
	case CaseType::KebabCase:
		while (true)
		{
			srcStr = this->ReadChar(srcStr, c, hasSpace);
			if (c == 0) return;
			if (hasSpace)
				sbOut->AppendUTF8Char('-');
			sbOut->AppendUTF8Char(Text::CharUtil::ToLower(c));
		}
		break;
	case CaseType::ScreamingSnakeCase:
		while (true)
		{
			srcStr = this->ReadChar(srcStr, c, hasSpace);
			if (c == 0) return;
			if (hasSpace)
				sbOut->AppendUTF8Char('_');
			sbOut->AppendUTF8Char(Text::CharUtil::ToUpper(c));
		}
		break;
	case CaseType::UpperCase:
		srcStr = this->ReadChar(srcStr, c, hasSpace);
		if (c == 0) return;
		sbOut->AppendUTF8Char(Text::CharUtil::ToUpper(c));
		while (true)
		{
			srcStr = this->ReadChar(srcStr, c, hasSpace);
			if (c == 0) return;
			if (hasSpace)
			{
				sbOut->AppendUTF8Char(' ');
				sbOut->AppendUTF8Char(Text::CharUtil::ToUpper(c));
			}
			else
			{
				sbOut->AppendUTF8Char(Text::CharUtil::ToLower(c));
			}
		}
		break;
	case CaseType::LowerCase:
		while (true)
		{
			srcStr = this->ReadChar(srcStr, c, hasSpace);
			if (c == 0) return;
			if (hasSpace)
				sbOut->AppendUTF8Char(' ');
			sbOut->AppendUTF8Char(Text::CharUtil::ToLower(c));
		}
		break;
	case CaseType::BlockCase:
		while (true)
		{
			srcStr = this->ReadChar(srcStr, c, hasSpace);
			if (c == 0) return;
			if (hasSpace)
				sbOut->AppendUTF8Char(' ');
			sbOut->AppendUTF8Char(Text::CharUtil::ToUpper(c));
		}
		break;
	default:
		break;
	}
}
