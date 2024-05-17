#include "Stdafx.h"
#include "Manage/CodeSymbol.h"
#include "Parser/FileParser/ELFParser.h"

Manage::CodeSymbol::CodeSymbol(Text::CStringNN moduleName, Text::CStringNN funcName, OSInt ofst, UInt64 funcAddr)
{
	this->moduleName = Text::String::New(moduleName);
	this->funcName = Text::String::New(funcName);
	this->ofst = ofst;
	this->funcAddr = funcAddr;
}

Manage::CodeSymbol::~CodeSymbol()
{
	this->moduleName->Release();
	this->funcName->Release();
}

UTF8Char *Manage::CodeSymbol::ToString(UTF8Char *buff) const
{
	buff = this->moduleName->ConcatTo(buff);
	*buff++ = '(';
	buff = Parser::FileParser::ELFParser::ToFuncName(buff, funcName->v);
	if (this->ofst > 0)
	{
		buff = Text::StrConcatC(buff, UTF8STRC("+0x"));
		buff = Text::StrHexVal64V(buff, (UOSInt)this->ofst);
	}
	else if (this->ofst < 0)
	{
		buff = Text::StrConcatC(buff, UTF8STRC("-0x"));
		buff = Text::StrHexVal64V(buff, (UOSInt)-this->ofst);
	}
	*buff++ = ')';
	*buff = 0;
	return buff;
}

Optional<Manage::CodeSymbol> Manage::CodeSymbol::ParseFromStr(const UTF8Char *buff, UInt64 funcAddr)
{
	Text::CStringNN moduleName;
	Text::CStringNN funcName;
	UOSInt i = Text::StrIndexOfChar(buff, '(');
	if (i == INVALID_INDEX)
		return 0;
	moduleName = Text::CStringNN(buff, i);
	buff = &buff[i + 1];
	i = Text::StrIndexOfChar(buff, ')');
	if (i == INVALID_INDEX)
		return 0;
	funcName = Text::CStringNN(buff, i);
	;
	OSInt ofst;
	if ((i = funcName.IndexOf('+')) != INVALID_INDEX)
	{
		if (!funcName.Substring(i + 1).ToOSInt(ofst))
			return 0;
		funcName.leng = i;
	}
	else if ((i = funcName.IndexOf('-')) != INVALID_INDEX)
	{
		if (!funcName.Substring(i + 1).ToOSInt(ofst))
			return 0;
		ofst = -ofst;
		funcName.leng = i;
	}
	else
	{
		ofst = 0;
	}
	NN<Manage::CodeSymbol> symbol;
	NEW_CLASSNN(symbol, Manage::CodeSymbol(moduleName, funcName, ofst, funcAddr));
	return symbol;
}
