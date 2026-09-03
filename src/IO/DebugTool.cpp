#include "Stdafx.h"
#include "IO/DebugTool.h"
#include "Manage/Process.h"
#include "Manage/StackTracer.h"
#include "Manage/SymbolResolver.h"
#include "Manage/ThreadInfo.h"
#include <stdio.h>

void __stdcall DebugTool_ContextHandler(NN<Manage::ThreadContext> context, AnyType userObj)
{
	Manage::Process proc;
	Manage::SymbolResolver addrResol(proc);
	Manage::StackTracer tracer(context);
	if (tracer.IsSupported())
	{
		Text::StringBuilderUTF8 sb;
		while (tracer.GoToNextLevel())
		{
			sb.ClearStr();
			sb.AppendHex64(tracer.GetCurrentAddr());
			sb.AppendC(UTF8STRC(" "));
			addrResol.ResolveNameSB(sb, tracer.GetCurrentAddr());
			printf("%s\r\n", sb.ToPtr());
		}
	}
}

void IO::DebugTool::PrintStackTrace()
{
	NN<Manage::ThreadInfo> thread;
	if (Manage::ThreadInfo::GetCurrThread().SetTo(thread))
	{
		thread->GetThreadContext(DebugTool_ContextHandler, 0);
		thread.Delete();
	}

}
