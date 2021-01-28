#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Manage/ProcessEnumerator.h"
#include <windows.h>
#include <Tlhelp32.h>

Manage::ProcessEnumerator::ProcessEnumerator(void *hand)
{
	PROCESSENTRY32W *pent;
	this->hand = hand;
	this->procEntry = pent = MemAlloc(PROCESSENTRY32W, 1);
	this->first = true;
	pent->dwSize = sizeof(PROCESSENTRY32W);
	pent->th32ProcessID = 0;
	pent->cntThreads = 0;
	pent->th32ParentProcessID = 0;
	pent->szExeFile[0] = 0;
}

Manage::ProcessEnumerator::~ProcessEnumerator()
{
	MemFree(this->procEntry);
	CloseHandle(this->hand);
}

Bool Manage::ProcessEnumerator::NextItem()
{
	if (first)
	{
		first = false;
		return Process32FirstW(this->hand, (PROCESSENTRY32W*)this->procEntry) == TRUE;
	}
	else
	{
		return Process32NextW(this->hand, (PROCESSENTRY32W*)this->procEntry) == TRUE;
	}
}

UInt32 Manage::ProcessEnumerator::GetProcId()
{
	return ((PROCESSENTRY32W*)this->procEntry)->th32ProcessID;
}

UInt32 Manage::ProcessEnumerator::GetParentProcId()
{
	return ((PROCESSENTRY32W*)this->procEntry)->th32ParentProcessID;
}

UInt32 Manage::ProcessEnumerator::GetThreadCnt()
{
	return ((PROCESSENTRY32W*)this->procEntry)->cntThreads;
}

WChar *Manage::ProcessEnumerator::GetFileName(WChar *buff)
{
	return Text::StrConcat(buff, ((PROCESSENTRY32W*)this->procEntry)->szExeFile);
}

Manage::ProcessEnumerator *Manage::ProcessEnumerator::EnumAllProcesses()
{
	HANDLE hand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hand == INVALID_HANDLE_VALUE)
		return 0;
	Manage::ProcessEnumerator *penum;
	NEW_CLASS(penum, Manage::ProcessEnumerator(hand));
	return penum;
}

