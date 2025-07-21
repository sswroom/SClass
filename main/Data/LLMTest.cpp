#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/HTTPUtil.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Text/CString.h"
#include "Text/RegEx.h"
#include "Text/TextWriteUtil.h"

Int32 TestPage2_7()
{
	Net::OSSocketFactory sockf(false);
	Net::TCPClientFactory clif(sockf);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, false);
	Text::CStringNN url = CSTR("https://raw.githubusercontent.com/rasbt/LLMs-from-scratch/main/ch02/01_main-chapter-code/the-verdict.txt");
	Text::CStringNN filePath = CSTR("the-verdict.txt");
	NN<IO::Stream> f;
	Optional<Text::String> rawText = 0;
	NN<Text::String> s;
	NN<Text::String> s2;
	if (Net::HTTPUtil::DownloadAndOpen(clif, ssl, url, filePath, 1048576).SetTo(f))
	{
		rawText = f->ReadAsString();
		f.Delete();
	}
	if (rawText.SetTo(s))
	{
		printf("Total number of character: %d\r\n", (UInt32)s->leng);
		s2 = Text::String::NewSubstr(s->ToCString(), 0, 100);
		printf("%s\r\n", s2->v.Ptr());
		s2->Release();
		s->Release();
	}

	ssl.Delete();
	return 0;
}

Int32 TestPage2_9_1()
{
	IO::ConsoleWriter console;
	Text::CStringNN text = CSTR("Hello, world. This, is a test.");
	Data::ArrayListStringNN result;
	Text::RegEx("(\\s)").Split(text, result);
	Text::TextWriteUtil::WriteArray(console, result);
	result.FreeAll();
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UOSInt page = 20091;
	switch (page)
	{
	case 20070:
		return TestPage2_7();
	case 20091:
		return TestPage2_9_1();
	default:
		return 0;
	}
}
