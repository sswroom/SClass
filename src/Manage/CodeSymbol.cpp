#include "Stdafx.h"
#include "Manage/CodeSymbol.h"
#include "Text/Cpp/CppDemangler.h"

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

UnsafeArray<UTF8Char> Manage::CodeSymbol::ToString(UnsafeArray<UTF8Char> buff) const
{
	buff = this->moduleName->ConcatTo(buff);
	*buff++ = '(';
	buff = Text::Cpp::CppDemangler::ToFuncName(buff, funcName->v);
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

Optional<Manage::CodeSymbol> Manage::CodeSymbol::ParseFromStr(UnsafeArray<const UTF8Char> buff, UInt64 funcAddr)
{
	Text::CStringNN moduleName;
	Text::CStringNN funcName;
	UOSInt i = Text::StrIndexOfChar(buff, '(');
	if (i == INVALID_INDEX)
		return nullptr;
	moduleName = Text::CStringNN(buff, i);
	buff = &buff[i + 1];
	i = Text::StrIndexOfChar(buff, ')');
	if (i == INVALID_INDEX)
		return nullptr;
	funcName = Text::CStringNN(buff, i);
	;
	OSInt ofst;
	if ((i = funcName.IndexOf('+')) != INVALID_INDEX)
	{
		if (!funcName.Substring(i + 1).ToOSInt(ofst))
			return nullptr;
		funcName.leng = i;
	}
	else if ((i = funcName.IndexOf('-')) != INVALID_INDEX)
	{
		if (!funcName.Substring(i + 1).ToOSInt(ofst))
			return nullptr;
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
