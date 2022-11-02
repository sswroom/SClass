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

struct Manage::ProcessExecution::ClassData
{
	HANDLE out[2];
	HANDLE in[2];
};

UOSInt Manage::ProcessExecution::NewProcess(Text::CString cmd)
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	clsData->in[0] = INVALID_HANDLE_VALUE;
	clsData->in[1] = INVALID_HANDLE_VALUE;
	clsData->out[0] = INVALID_HANDLE_VALUE;
	clsData->out[1] = INVALID_HANDLE_VALUE;
	WChar buff[MAX_PATH];
	WChar progName[MAX_PATH];
	UOSInt cmdLen = Text::StrUTF8_WCharCnt(cmd.v);
	WChar *cmdLine = MemAlloc(WChar, cmdLen + 512);
	Text::StrUTF8_WChar(cmdLine, cmd.v, 0);

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

Manage::ProcessExecution::ProcessExecution(Text::CString cmdLine) : Process(NewProcess(cmdLine), false), IO::Stream(cmdLine)
{
}

Manage::ProcessExecution::~ProcessExecution()
{
	this->Close();
	MemFree(this->clsData);
}

UOSInt Manage::ProcessExecution::Read(UInt8 *buff, UOSInt size)
{
	if (this->clsData->in[0] == 0)
		return 0;
	DWORD readSize;
	if (ReadFile(this->clsData->out[0], buff, (DWORD)size, &readSize, 0))
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
	if (this->clsData->in[0] == 0)
		return 0;
	DWORD writeSize;
	if (WriteFile(this->clsData->in[1], buff, (DWORD)size, &writeSize, 0))
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
	if (this->IsRunning())
	{
		this->Kill();
	}
	if (this->clsData->in[0] != INVALID_HANDLE_VALUE)
	{
		CloseHandle(this->clsData->in[0]);
		CloseHandle(this->clsData->in[1]);
		CloseHandle(this->clsData->out[0]);
		CloseHandle(this->clsData->out[1]);
		this->clsData->in[0] = INVALID_HANDLE_VALUE;
		this->clsData->in[1] = INVALID_HANDLE_VALUE;
		this->clsData->out[0] = INVALID_HANDLE_VALUE;
		this->clsData->out[1] = INVALID_HANDLE_VALUE;
	}
}

Bool Manage::ProcessExecution::Recover()
{
	return false;
}

IO::StreamType Manage::ProcessExecution::GetStreamType() const
{
	return IO::StreamType::ProcessExecution;
}
