#include "Stdafx.h"
#include "Core/Core.h"
#include "Parser/FileParser/ELFParser.h"
#include <stdio.h>

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[512];
	//Text::CStringNN name = CSTR("_ZdlPv@GLIBCXX_3.4");
//	Text::CStringNN name = CSTR("_ZN2IO9DebugTool15PrintStackTraceEv");
//	Text::CStringNN name = CSTR("_Z6MyMain2NNIN4Core12IProgControlEE");
	Text::CStringNN name = CSTR("_ZNK9StoneRoad3PBG13PBGMobileCore19UsersPasswordExistsE2NNINS0_5UsersEEN4Text9CStringNNES2_IN4Sync10MutexUsageEE");
//	Text::CStringNN name = CSTR("_ZN9StoneRoad3PBG16PBGMobileHandler15SetPasswordFuncE2NNIN3Net9WebServer11IWebRequestEES2_INS4_12IWebResponseEEN4Text9CStringNNES2_INS4_17WebServiceHandlerEE");
//	Text::CStringNN name = CSTR("_ZN3Net9WebServer17WebServiceHandler14ProcessRequestE2NNINS0_11IWebRequestEES2_INS0_12IWebResponseEEN4Text9CStringNNE");
//	Text::CStringNN name = CSTR("_ZN3Net9WebServer18WebStandardHandler9DoRequestE2NNINS0_11IWebRequestEES2_INS0_12IWebResponseEEN4Text9CStringNNE");
//	Text::CStringNN name = CSTR("_ZN19PBGMobileAppHandler14ProcessRequestE2NNIN3Net9WebServer11IWebRequestEES0_INS2_12IWebResponseEEN4Text9CStringNNE");
//	Text::CStringNN name = CSTR("_ZN3Net9WebServer18WebStandardHandler10WebRequestE2NNINS0_11IWebRequestEES2_INS0_12IWebResponseEE");
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
