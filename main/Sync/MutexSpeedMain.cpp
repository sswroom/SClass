#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Sync/Mutex.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff2[256];
	UnsafeArray<UTF8Char> sptr;
	NN<Manage::HiResClock> clk;
	NN<Sync::Mutex> mut;

	NEW_CLASSNN(clk, Manage::HiResClock());
	NEW_CLASSNN(mut, Sync::Mutex());
	clk->Start();

	Int32 i = 10000;
	while (i-- > 0)
	{
		mut->Lock();
		mut->Unlock();
	}

	Double t1 = clk->GetTimeDiff();
	mut.Delete();
	clk.Delete();

	NN<IO::ConsoleWriter> console;
	NEW_CLASSNN(console, IO::ConsoleWriter());
	sptr = Text::StrConcatC(Text::StrDouble(sbuff2, t1), UTF8STRC(" s"));
	console->WriteLine(CSTRP(sbuff2, sptr));
	console.Delete();
	return 0;
}
