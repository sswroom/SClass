#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/Cpp/CppDemangler.h"
#include <stdio.h>

void Demangling(Text::CStringNN name)
{
	UTF8Char sbuff[512];
	Text::Cpp::CppDemangler::ToFuncName(sbuff, name.v);
	printf("%s\r\n%s\r\n\r\n", name.v.Ptr(), sbuff);
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	//operator delete(void*)
	Demangling(CSTR("_ZdlPv@GLIBCXX_3.4"));
	//IO::DebugTool::PrintStackTrace(void)
	Demangling(CSTR("_ZN2IO9DebugTool15PrintStackTraceEv"));
	//MyMain(NN<Core::ProgControl>)
	Demangling(CSTR("_Z6MyMain2NNIN4Core12ProgControlEE"));
	//StoneRoad::PBG::PBGMobileCore::UsersPasswordExists(NN<Users>, Text::CStringNN, NN<Sync::MutexUsage>) const
	Demangling(CSTR("_ZNK9StoneRoad3PBG13PBGMobileCore19UsersPasswordExistsE2NNINS0_5UsersEEN4Text9CStringNNES2_IN4Sync10MutexUsageEE"));
	//static StoneRoad::PBG::PBGMobileHandler::SetPasswordFunc(NN<Net::WebServer::WebRequest>, NN<Net::WebServer::WebResponse>, Text::CStringNN, NN<WebServiceHandler>)
	Demangling(CSTR("_ZN9StoneRoad3PBG16PBGMobileHandler15SetPasswordFuncE2NNIN3Net9WebServer11WebRequestEES2_INS4_12WebResponseEEN4Text9CStringNNES2_INS4_17WebServiceHandlerEE"));
	//virtual Bool Net::WebServer::WebServiceHandler::ProcessRequest(NN<Net::WebServer::WebRequest>, NN<Net::WebServer::WebResponse>, Text::CStringNN);
	Demangling(CSTR("_ZN3Net9WebServer17WebServiceHandler14ProcessRequestE2NNINS0_11WebRequestEES2_INS0_12WebResponseEEN4Text9CStringNNE"));
	//Bool Net::WebServer::WebStandardHandler::DoRequest(NN<Net::WebServer::WebRequest>, NN<Net::WebServer::WebResponse>, Text::CStringNN);
	Demangling(CSTR("_ZN3Net9WebServer18WebStandardHandler9DoRequestE2NNINS0_11WebRequestEES2_INS0_12WebResponseEEN4Text9CStringNNE"));
	//virtual Bool PBGMobileAppHandler::ProcessRequest(NN<Net::WebServer::WebRequest>, NN<Net::WebServer::WebResponse>, Text::CStringNN);
	Demangling(CSTR("_ZN19PBGMobileAppHandler14ProcessRequestE2NNIN3Net9WebServer11WebRequestEES0_INS2_12WebResponseEEN4Text9CStringNNE"));
	//virtual void Net::WebServer::WebStandardHandler::WebRequest(NN<Net::WebServer::WebRequest>, NN<Net::WebServer::WebResponse>);
	Demangling(CSTR("_ZN3Net9WebServer18WebStandardHandler10WebRequestE2NNINS0_11WebRequestEES2_INS0_12WebResponseEE"));
	//void h(int)
	Demangling(CSTR("_Z1hi"));
	//void h(int, char)
	Demangling(CSTR("_Z1hic"));
	//void h(void)
	Demangling(CSTR("_Z1hv"));
	//Map<StringName, Ref<GDScript>, Comparator<StringName>, DefaultAllocator>::has(StringName const&) const
	Demangling(CSTR("_ZNK3MapI10StringName3RefI8GDScriptE10ComparatorIS0_E16DefaultAllocatorE3hasERKS0_"));
	return 0;
}
