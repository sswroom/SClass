#include "Stdafx.h"
#include "IO/DebugTool.h"
#include "Manage/Process.h"
#include "Manage/StackTracer.h"
#include "Manage/SymbolResolver.h"
#include "Manage/ThreadInfo.h"
#include <stdio.h>

void IO::DebugTool::PrintStackTrace()
{
	NN<Manage::ThreadInfo> thread;
	if (thread.Set(Manage::ThreadInfo::GetCurrThread()))
	{
		Manage::Process proc;
		Manage::SymbolResolver addrResol(proc);
		Optional<Manage::ThreadContext> tContext = thread->GetThreadContext();
		Manage::StackTracer tracer(tContext);
		if (tracer.IsSupported())
		{
			Text::StringBuilderUTF8 sb;
			while (tracer.GoToNextLevel())
			{
				sb.ClearStr();
				sb.AppendHex64(tracer.GetCurrentAddr());
				sb.AppendC(UTF8STRC(" "));
				addrResol.ResolveNameSB(sb, tracer.GetCurrentAddr());
				printf("%s\r\n", sb.ToString());
			}
		}
		tContext.Delete();
		thread.Delete();
	}

}
