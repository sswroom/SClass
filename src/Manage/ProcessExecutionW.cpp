#include "Stdafx.h"
#include "IO/Path.h"
#include "Manage/ProcessExecution.h"
#include "Text/MyStringW.h"
#include <stdio.h>
#include <stdlib.h>
#if !defined(_MSC_VER)
#include <unistd.h>
#endif
#include <windows.h>

typedef struct
{
	HANDLE out[2];
	HANDLE in[2];
} ClassData;

UOSInt Manage::ProcessExecution::NewProcess(const UTF8Char *cmd)
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	clsData->in[0] = INVALID_HANDLE_VALUE;
	clsData->in[1] = INVALID_HANDLE_VALUE;
	clsData->out[0] = INVALID_HANDLE_VALUE;
	clsData->out[1] = INVALID_HANDLE_VALUE;
	WChar buff[MAX_PATH];
	WChar progName[MAX_PATH];
	UOSInt cmdLen = Text::StrUTF8_WCharCnt(cmd);
	WChar *cmdLine = MemAlloc(WChar, cmdLen + 512);
	Text::StrUTF8_WChar(cmdLine, cmd, 0);

	WChar *cptr = cmdLine;
	WChar *pptr = progName;
	Bool isQuote = false;
	WChar c;
	while ((c = *cptr++) != 0)
	{
		if (c == '"')
			isQuote = !isQuote;
		else if (!isQuote && c == ' ')
		{
			break;
		}
		else
		{
			*pptr++ = c;
		}
	}
	*pptr = 0;

	IO::Path::GetFileDirectoryW(buff, progName);
	PROCESS_INFORMATION procInfo;
	STARTUPINFOW startInfo;
	BOOL createRet;

	SECURITY_ATTRIBUTES sa;
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	CreatePipe(&clsData->in[0], &clsData->in[1], &sa, 0);
	CreatePipe(&clsData->out[0], &clsData->out[1], &sa, 0);
	MemClear(&startInfo, sizeof(startInfo));
	startInfo.cb = sizeof(startInfo);
	startInfo.dwFlags = STARTF_USESTDHANDLES;
	startInfo.hStdOutput = clsData->out[1];
	startInfo.hStdError = startInfo.hStdOutput;
	startInfo.hStdInput = clsData->in[0];
	createRet = CreateProcessW(0, cmdLine, 0, 0, true, NORMAL_PRIORITY_CLASS, 0, buff, &startInfo, &procInfo);

	if(createRet)
	{
		return procInfo.dwProcessId;
	}
	else
	{
		return 0;
	}
}

Manage::ProcessExecution::ProcessExecution(const UTF8Char *cmdLine) : Process(NewProcess(cmdLine), false), IO::Stream(cmdLine)
{
}

Manage::ProcessExecution::~ProcessExecution()
{
	ClassData *clsData = (ClassData*)this->clsData;
	this->Close();
	MemFree(clsData);
}

UOSInt Manage::ProcessExecution::Read(UInt8 *buff, UOSInt size)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (clsData->in[0] == 0)
		return 0;
	DWORD readSize;
	if (ReadFile(clsData->out[0], buff, (DWORD)size, &readSize, 0))
	{
		return readSize;
	}
	else
	{
		return 0;
	}
}

UOSInt Manage::ProcessExecution::Write(const UInt8 *buff, UOSInt size)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (clsData->in[0] == 0)
		return 0;
	DWORD writeSize;
	if (WriteFile(clsData->in[1], buff, (DWORD)size, &writeSize, 0))
	{
		return writeSize;
	}
	else
	{
		return 0;
	}
}

Int32 Manage::ProcessExecution::Flush()
{
	return 0;
}

void Manage::ProcessExecution::Close()
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (this->IsRunning())
	{
		this->Kill();
	}
	if (clsData->in[0] != INVALID_HANDLE_VALUE)
	{
		CloseHandle(clsData->in[0]);
		CloseHandle(clsData->in[1]);
		CloseHandle(clsData->out[0]);
		CloseHandle(clsData->out[1]);
		clsData->in[0] = INVALID_HANDLE_VALUE;
		clsData->in[1] = INVALID_HANDLE_VALUE;
		clsData->out[0] = INVALID_HANDLE_VALUE;
		clsData->out[1] = INVALID_HANDLE_VALUE;
	}
}

Bool Manage::ProcessExecution::Recover()
{
	return false;
}
