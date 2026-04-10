#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/DasmX86_32.h"
#include "Manage/Process.h"
#include "Manage/SymbolResolver.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Manage::DasmX86_32 *dasm;
	NN<Manage::Process> proc;
	Manage::SymbolResolver *symbResol;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	IO::ConsoleWriter *console;

	NEW_CLASSNN(proc, Manage::Process());
	NEW_CLASS(symbResol, Manage::SymbolResolver(proc));
	NEW_CLASS(dasm, Manage::DasmX86_32());
	NEW_CLASS(console, IO::ConsoleWriter());

	UnsafeArray<UInt8> addr = (UInt8*)Sync::ThreadUtil::GetCurrAddr().p;
	NN<Manage::DasmX86_32::Session> sess = dasm->StartDasm(symbResol, addr, proc);
	while (true)
	{
		sptr = Text::StrHexVal32(sbuff, (UInt32)dasm->SessGetCodeOffset(sess));
		sptr = Text::StrConcatC(sptr, UTF8STRC(" "));
		if (dasm->DasmNext(sess, sptr).SetTo(sptr))
		{
			console->Write(CSTRP(sbuff, sptr));
		}
		else
		{
			Manage::DasmX86_32::EndType et = dasm->SessGetEndType(sess);
			if (et == Manage::DasmX86_32::EndType::Jmp)
			{
				if (dasm->SessContJmp(sess))
				{
					continue;
				}
			}
			break;
		}
	}
	dasm->EndDasm(sess);

	DEL_CLASS(console);
	DEL_CLASS(dasm);
	DEL_CLASS(symbResol);
	proc.Delete();
	return 0;
}
