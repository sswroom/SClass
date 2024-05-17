#include "Stdafx.h"
#include "Core/Core.h"
#include "Parser/FileParser/ELFParser.h"
#include <stdio.h>

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[512];
	//Text::CStringNN name = CSTR("_ZdlPv@GLIBCXX_3.4");
	Text::CStringNN name = CSTR("_ZN2IO9DebugTool15PrintStackTraceEv");
//	Text::CStringNN name = CSTR("_Z6MyMain2NNIN4Core12IProgControlEE");
	/*
	Data
	Data::Sort
	Data::Sort::InsertionSort
	Data::Sort::InsertionSort::Sort
	Data::VariObject
	Data::VariObject*
	Data::Sort::InsertionSort::SortB<Data::VariObject*>
	Data::VariObject**
	Data::Sort::InsertionSort::SortB<Data::VariObject*>(Data::VariObject**, Data::Comparator<
	*/
	Parser::FileParser::ELFParser::ToFuncName(sbuff, name.v);
	printf("%s\r\n", sbuff);
	return 0;
}
