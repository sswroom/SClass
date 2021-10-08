#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Manage/Process.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#define PSAPI_VERSION 1
#include <psapi.h>
#include <tlhelp32.h>
#include <shellapi.h>
#undef DeleteFile

#ifndef _WIN32_WCE
#define CloseToolhelp32Snapshot(hand) CloseHandle(hand)
#else
extern "C" DWORD SetProcPermissions(DWORD newperms);
#endif

static Int32 Process_ExecFileId = 0;

Manage::Process::Process(UOSInt procId, Bool controlRight)
{
	this->procId = procId;
	DWORD access = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ;
	if (controlRight)
	{
		access |= PROCESS_TERMINATE;
	}
	this->handle = OpenProcess(access, FALSE, (DWORD)procId);
	if (this->handle == 0)
	{
		access = PROCESS_QUERY_INFORMATION;
		if (controlRight)
		{
			access |= PROCESS_TERMINATE;
		}
		this->handle = OpenProcess(access, FALSE, (DWORD)procId);
	}
	this->needRelease = true;
}

Manage::Process::Process()
{
	this->procId = GetCurrentProcessId();
	this->handle = GetCurrentProcess();
	this->needRelease = false;
}

Manage::Process::Process(const UTF8Char *cmdLine)
{
	WChar buff[MAX_PATH];
	WChar progName[MAX_PATH];
	const WChar *wptr = Text::StrToWCharNew(cmdLine);
	const WChar *cptr = wptr;
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
	ZeroMemory(&startInfo, sizeof(startInfo));
	startInfo.cb = sizeof(startInfo);
#ifdef _WIN32_WCE
	CreateProcessW(0, (LPWSTR)wptr, 0, 0, false, 0, 0, buff, &startInfo, &procInfo);
#else
	CreateProcessW(0, (LPWSTR)wptr, 0, 0, false, NORMAL_PRIORITY_CLASS, 0, buff, &startInfo, &procInfo);
#endif
	Text::StrDelNew(wptr);
	this->handle = procInfo.hProcess;
	this->procId = procInfo.dwProcessId;
	this->needRelease = true;
	CloseHandle(procInfo.hThread);
}

Manage::Process::Process(const WChar *cmdLine)
{
	WChar buff[MAX_PATH];
	WChar progName[MAX_PATH];
	const WChar *cptr = cmdLine;
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
	ZeroMemory(&startInfo, sizeof(startInfo));
	startInfo.cb = sizeof(startInfo);
#ifdef _WIN32_WCE
	CreateProcessW(0, (LPWSTR)cmdLine, 0, 0, false, 0, 0, buff, &startInfo, &procInfo);
#else
	CreateProcessW(0, (LPWSTR)cmdLine, 0, 0, false, NORMAL_PRIORITY_CLASS, 0, buff, &startInfo, &procInfo);
#endif
	this->handle = procInfo.hProcess;
	this->procId = procInfo.dwProcessId;
	this->needRelease = true;
	CloseHandle(procInfo.hThread);
}

Manage::Process::~Process()
{
	if (needRelease && this->handle)
	{
		CloseHandle((HANDLE)this->handle);
		this->handle = 0;
	}
}

UOSInt Manage::Process::GetCurrProcId()
{
	return GetCurrentProcessId();
}

UOSInt Manage::Process::GetProcId()
{
	return this->procId;
}

Bool Manage::Process::IsRunning()
{
	if (this->handle)
	{
		UInt32 code;
		if (GetExitCodeProcess(this->handle, (DWORD*)&code))
		{
			return code == STILL_ACTIVE;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

Bool Manage::Process::Kill()
{
//	printf("Process: Kill Process\r\n");
	if (this->handle)
	{
//		printf("Process: Handle Found\r\n");
		if (TerminateProcess(this->handle, 0))
			return true;
//		printf("Process: Kill Error, err = %d\r\n", GetLastError());
	}
	return false;
}

WChar *Manage::Process::GetFilename(WChar *buff)
{
	if (this->handle)
	{
		UInt32 retSize;
#ifdef _WIN32_WCE
		retSize = GetModuleFileNameW((HMODULE)this->handle, buff, 1024);
#else
		//retSize = GetProcessImageFileNameW(this->handle, buff, 1024);
		retSize = GetModuleFileNameExW(this->handle, 0, buff, 1024);
#endif
		buff[retSize] = 0;
		return &buff[retSize];
	}
	else
	{
		return 0;
	}
}

Bool Manage::Process::GetFilename(Text::StringBuilderUTF *sb)
{
	if (this->handle)
	{
		UInt32 retSize;
		WChar *buff = MemAlloc(WChar, 1024);
#ifdef _WIN32_WCE
		retSize = GetModuleFileNameW((HMODULE)this->handle, buff, 1024);
#else
		//retSize = GetProcessImageFileNameW(this->handle, buff, 1024);
		retSize = GetModuleFileNameExW(this->handle, 0, buff, 1024);
#endif
		buff[retSize] = 0;
		const UTF8Char *csptr = Text::StrToUTF8New(buff);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		MemFree(buff);
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Manage::Process::GetMemorySize()
{
#ifdef _WIN32_WCE
	return 0;
#else
	PROCESS_MEMORY_COUNTERS cnt;
	cnt.cb = sizeof(cnt);
	if (GetProcessMemoryInfo((HANDLE)this->handle, &cnt, sizeof(cnt)))
	{
		return cnt.WorkingSetSize;
	}
	else
	{
		return 0;
	}
#endif
}

Bool Manage::Process::SetMemorySize(UOSInt minSize, UOSInt maxSize)
{
#ifdef _WIN32_WCE
	return false;
#else
	BOOL ret = SetProcessWorkingSetSize((HANDLE)this->handle, minSize, maxSize);
//	Int32 lastErr;
//	if (ret == 0)
//	{
//		lastErr = GetLastError();
//	}
	return ret != 0;
#endif
}

UOSInt Manage::Process::GetThreadIds(Data::ArrayList<UInt32> *threadList)
{
	THREADENTRY32 threadEntry;
	UOSInt threadCnt = 0;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	threadEntry.dwSize = sizeof(threadEntry);
	if (Thread32First(hSnapShot, &threadEntry))
	{
		while (true)
		{
			if (threadEntry.th32OwnerProcessID == (UInt32)this->procId)
			{
				threadList->Add(threadEntry.th32ThreadID);
				threadCnt++;
			}

			if (Thread32Next(hSnapShot, &threadEntry) != TRUE)
			{
				break;
			}
		}
	}
	CloseToolhelp32Snapshot(hSnapShot);
	return threadCnt;
}

void *Manage::Process::GetHandle()
{
	return this->handle;
}

UOSInt Manage::Process::GetModules(Data::ArrayList<Manage::ModuleInfo *> *modList)
{
#ifdef _WIN32_WCE
	MODULEENTRY32 moduleInfo;
	Manage::ModuleInfo *mod;
	HANDLE hSnapShot;
	UOSInt i;
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
	moduleInfo.dwSize = sizeof(moduleInfo);
	i = 0;
	if (Module32First(hSnapShot, &moduleInfo))
	{
		while (true)
		{
			if (moduleInfo.th32ProcessID == this->procId)
			{
				NEW_CLASS(mod, Manage::ModuleInfo(this->handle, moduleInfo.hModule));
				modList->Add(mod);
				i++;
			}

			if (Module32Next(hSnapShot, &moduleInfo) != TRUE)
			{
				break;
			}
		}
	}
	CloseToolhelp32Snapshot(hSnapShot);
	return i;
#else
	UOSInt i;
	Manage::ModuleInfo *mod;
	HMODULE mods[512];
	UInt32 modCnt;
	if (EnumProcessModules((HANDLE)this->handle, mods, sizeof(mods), (LPDWORD)&modCnt))
	{
		i = 0;
		while (i < (modCnt / sizeof(HMODULE)))
		{
			NEW_CLASS(mod, Manage::ModuleInfo(this->handle, mods[i]));
			modList->Add(mod);

			i++;
		}

	}
	return modCnt;
#endif
}

UOSInt Manage::Process::GetThreads(Data::ArrayList<Manage::ThreadInfo *> *threadList)
{
	Manage::ThreadInfo *tInfo;
	UOSInt threadCnt = 0;
	THREADENTRY32 threadEntry;
	HANDLE hSnapShot;
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
//		Int32 ret = GetLastError();
	}
	else
	{
		threadEntry.dwSize = sizeof(threadEntry);
		if (Thread32First(hSnapShot, &threadEntry))
		{
			while (true)
			{
				if (threadEntry.th32OwnerProcessID == this->procId)
				{
					NEW_CLASS(tInfo, Manage::ThreadInfo(this->procId, threadEntry.th32ThreadID));
					threadList->Add(tInfo);
					threadCnt++;
				}

				if (Thread32Next(hSnapShot, &threadEntry) != TRUE)
				{
					break;
				}
			}
		}
		CloseToolhelp32Snapshot(hSnapShot);
	}
	return threadCnt;
}

UOSInt Manage::Process::GetHeapLists(Data::ArrayList<UInt32> *heapList)
{
	HEAPLIST32 heapListInfo;
	HANDLE hSnapShot;
	UOSInt i;
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, (DWORD)this->procId);
	heapListInfo.dwSize = sizeof(heapListInfo);
	i = 0;
	if (Heap32ListFirst(hSnapShot, &heapListInfo))
	{
		while (true)
		{
			if (heapListInfo.th32ProcessID == this->procId)
			{
				heapList->Add((UInt32)heapListInfo.th32HeapID);
				i++;
			}

			if (Heap32ListNext(hSnapShot, &heapListInfo) != TRUE)
			{
				break;
			}
		}
	}
	CloseToolhelp32Snapshot(hSnapShot);
	return i;
}

UOSInt Manage::Process::GetHeaps(Data::ArrayList<HeapInfo*> *heapList, UInt32 heapListId, UOSInt maxCount)
{
	HEAPENTRY32 ent;
	HeapInfo *heap;
	UOSInt cnt = 0;
	if (maxCount <= 0)
	{
		maxCount = (UOSInt)-1;
		maxCount = (maxCount >> 1);
	}
	ent.dwSize = sizeof(ent);
#ifdef _WIN32_WCE
	HANDLE hand = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, this->procId);
	if (hand != INVALID_HANDLE_VALUE)
	{
		if (Heap32First(hand, &ent, this->procId, heapListId))
		{
			while (true)
			{
				heap = MemAlloc(HeapInfo, 1);
				heap->startAddr = ent.dwAddress;
				heap->size = ent.dwBlockSize;
				switch (ent.dwFlags)
				{
				case LF32_FIXED:
					heap->heapType = Manage::Process::HT_FIXED;
					break;
				case LF32_FREE:
					heap->heapType = Manage::Process::HT_FREE;
					break;
				case LF32_MOVEABLE:
					heap->heapType = Manage::Process::HT_MOVABLE;
					break;
				default:
					heap->heapType = Manage::Process::HT_UNKNOWN;
					break;
				}
				heapList->Add(heap);
				cnt++;
				if (cnt >= maxCount || !Heap32Next(hand, &ent))
					break;
			}
		}
		CloseToolhelp32Snapshot(hand);
	}
#else
	if (Heap32First(&ent, (DWORD)this->procId, heapListId))
	{
		while (true)
		{
			heap = MemAlloc(HeapInfo, 1);
			heap->startAddr = ent.dwAddress;
			heap->size = ent.dwBlockSize;
			switch (ent.dwFlags)
			{
			case LF32_FIXED:
				heap->heapType = Manage::Process::HT_FIXED;
				break;
			case LF32_FREE:
				heap->heapType = Manage::Process::HT_FREE;
				break;
			case LF32_MOVEABLE:
				heap->heapType = Manage::Process::HT_MOVABLE;
				break;
			default:
				heap->heapType = Manage::Process::HT_UNKNOWN;
				break;
			}
			heapList->Add(heap);
			cnt++;
			if (cnt >= maxCount || !Heap32Next(&ent))
				break;
		}
	}
#endif
	return cnt;

}

void Manage::Process::FreeHeaps(Data::ArrayList<HeapInfo*> *heapList)
{
	UOSInt i;
	i = heapList->GetCount();
	while (i-- > 0)
	{
		MemFree(heapList->RemoveAt(i));
	}
}

Bool Manage::Process::GetWorkingSetSize(UOSInt *minSize, UOSInt *maxSize)
{
#ifdef _WIN32_WCE
	return false;
#else
	return GetProcessWorkingSetSize(this->handle, (PSIZE_T)minSize, (PSIZE_T)maxSize) != 0;
#endif

}

Bool Manage::Process::GetMemoryInfo(UOSInt *pageFault, UOSInt *workingSetSize, UOSInt *pagedPoolUsage, UOSInt *nonPagedPoolUsage, UOSInt *pageFileUsage)
{
#ifdef _WIN32_WCE
	return false;
#else
	PROCESS_MEMORY_COUNTERS memInfo;
	BOOL ret;
	memInfo.cb = sizeof(memInfo);
	ret = GetProcessMemoryInfo(this->handle, &memInfo, sizeof(memInfo));
	if (ret)
	{
		if (pageFault)
			*pageFault = memInfo.PageFaultCount;
		if (workingSetSize)
			*workingSetSize = memInfo.WorkingSetSize;
		if (pagedPoolUsage)
			*pagedPoolUsage = memInfo.QuotaPagedPoolUsage;
		if (nonPagedPoolUsage)
			*nonPagedPoolUsage = memInfo.QuotaNonPagedPoolUsage;
		if (pageFileUsage)
			*pageFileUsage = memInfo.PagefileUsage;
		return true;
	}
//	UInt32 err = GetLastError();
	return false;
#endif
}

Bool Manage::Process::GetTimeInfo(Data::DateTime *createTime, Data::DateTime *kernelTime, Data::DateTime *userTime)
{
#ifdef _WIN32_WCE
	FILETIME ttime1 = {0, 0};
	FILETIME ttime2 = {0, 0};
	FILETIME ttime3 = {0, 0};
	FILETIME ttime4 = {0, 0};
	FILETIME time1;
	FILETIME time2;
	FILETIME time3;
	FILETIME time4;
	SYSTEMTIME sysTime;
	Bool found = false;
	THREADENTRY32 threadEntry;
	HANDLE hSnapShot;
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapShot != INVALID_HANDLE_VALUE)
	{
		DWORD oriPerm;
		threadEntry.dwSize = sizeof(threadEntry);
		oriPerm = SetProcPermissions(0xffffffff);
		if (Thread32First(hSnapShot, &threadEntry))
		{
			while (true)
			{
				if (threadEntry.th32OwnerProcessID == this->procId)
				{
					if (GetThreadTimes((HANDLE)threadEntry.th32ThreadID, &time1, &time2, &time3, &time4))
					{
						ttime1 = time1;
						ttime2 = time2;
						ttime3.dwLowDateTime += time3.dwLowDateTime;
						ttime3.dwHighDateTime += time3.dwHighDateTime;
						if (ttime3.dwLowDateTime < time3.dwLowDateTime)
						{
							ttime3.dwHighDateTime += 1;
						}
						ttime4.dwLowDateTime += time4.dwLowDateTime;
						ttime4.dwHighDateTime += time4.dwHighDateTime;
						if (ttime4.dwLowDateTime < time4.dwLowDateTime)
						{
							ttime4.dwHighDateTime += 1;
						}
						found = true;
					}
				}

				if (Thread32Next(hSnapShot, &threadEntry) != TRUE)
				{
					break;
				}
			}
		}
		SetProcPermissions(oriPerm);
		CloseToolhelp32Snapshot(hSnapShot);
	}
	if (found)
	{
		if (createTime)
		{
			FileTimeToSystemTime(&time1, &sysTime);
			createTime->SetValueSYSTEMTIME(&sysTime);
		}
		if (kernelTime)
		{
			FileTimeToSystemTime(&time3, &sysTime);
			kernelTime->SetValueSYSTEMTIME(&sysTime);
		}
		if (userTime)
		{
			FileTimeToSystemTime(&time4, &sysTime);
			userTime->SetValueSYSTEMTIME(&sysTime);
		}
	}
	return found;
#else
	FILETIME time1;
	FILETIME time2;
	FILETIME time3;
	FILETIME time4;
	SYSTEMTIME sysTime;
	BOOL ret;
	ret = GetProcessTimes(this->handle, &time1, &time2, &time3, &time4);
	if (ret)
	{
		if (createTime)
		{
			FileTimeToSystemTime(&time1, &sysTime);
			createTime->SetValueSYSTEMTIME(&sysTime);
		}
		if (kernelTime)
		{
			FileTimeToSystemTime(&time3, &sysTime);
			kernelTime->SetValueSYSTEMTIME(&sysTime);
		}
		if (userTime)
		{
			FileTimeToSystemTime(&time4, &sysTime);
			userTime->SetValueSYSTEMTIME(&sysTime);
		}
		return true;
	}
//	UInt32 err = GetLastError();
	return false;
#endif
}


UInt32 Manage::Process::GetGDIObjCount()
{
#ifdef _WIN32_WCE
	return 0;
#else
	return GetGuiResources(this->handle, GR_GDIOBJECTS);
#endif
}

UInt32 Manage::Process::GetUserObjCount()
{
#ifdef _WIN32_WCE
	return 0;
#else
	return GetGuiResources(this->handle, GR_USEROBJECTS);
#endif
}

UInt32 Manage::Process::GetHandleCount()
{
#ifdef _WIN32_WCE
	return 0;
#else
	if (this->handle == 0)
		return 0;
	DWORD handleCount;
	if (GetProcessHandleCount(this->handle, &handleCount))
		return handleCount;
	return 0;
#endif
}

Manage::Process::ProcessPriority Manage::Process::GetPriority()
{
#ifdef _WIN32_WCE
	return PP_UNKNOWN;
#else
	if (this->handle == 0)
		return PP_UNKNOWN;
	return (ProcessPriority)GetPriorityClass(this->handle);
#endif
}

Manage::ThreadContext::ContextType Manage::Process::GetContextType()
{
#if defined(_WIN32_WCE)
#  if defined(ARM)
	return Manage::ThreadContext::CT_ARM;
#  elif defined(_ALPHA_)
	return Manage::ThreadContext::CT_ARM;
#  elif defined(_X86_)
	return Manage::ThreadContext::CT_X86_32;
#  elif defined(_MIPS_)
	return Manage::ThreadContext::CT_ARM;
#  elif defined(_PPC_)
	return Manage::ThreadContext::CT_ARM;
#  elif defined(_MPPC_)
	return Manage::ThreadContext::CT_ARM;
#  elif defined(_IA64_)
	return Manage::ThreadContext::CT_ARM;
#  elif defined(SHx)
	return Manage::ThreadContext::CT_ARM;
#  endif

#elif defined(_WIN64)
	if (this->handle == 0)
		return Manage::ThreadContext::CT_X86_32;
	BOOL isWow64;
	if (!IsWow64Process((HANDLE)this->handle, &isWow64))
		return Manage::ThreadContext::CT_X86_32;
	if (isWow64)
	{
		return Manage::ThreadContext::CT_X86_32;
	}
	else
	{
		return Manage::ThreadContext::CT_X86_64;
	}
#else
	return Manage::ThreadContext::CT_X86_32;
#endif
}

UInt8 Manage::Process::ReadMemUInt8(UInt64 addr)
{
	UInt8 buff;
	SIZE_T size;
	if (ReadProcessMemory(this->handle, (void*)addr, &buff, 1, &size))
	{
		return buff;
	}
	return 0;
}

UInt16 Manage::Process::ReadMemUInt16(UInt64 addr)
{
	UInt8 buff[2];
	SIZE_T size;
	if (ReadProcessMemory(this->handle, (void*)addr, buff, 2, &size))
	{
		if (size == 2)
			return ReadUInt16(&buff[0]);
	}
	return 0;
}

UInt32 Manage::Process::ReadMemUInt32(UInt64 addr)
{
	UInt8 buff[4];
	SIZE_T size;
	if (ReadProcessMemory(this->handle, (void*)addr, buff, 4, &size))
	{
		if (size == 4)
			return ReadUInt32(&buff[0]);
	}
	return 0;
}

UInt64 Manage::Process::ReadMemUInt64(UInt64 addr)
{
	UInt8 buff[8];
	SIZE_T size;
	if (ReadProcessMemory(this->handle, (void*)addr, buff, 8, &size))
	{
		if (size == 8)
			return ReadUInt64(&buff[0]);
	}
	return 0;
}

UOSInt Manage::Process::ReadMemory(UInt64 addr, UInt8 *buff, UOSInt reqSize)
{
	SIZE_T size;
	if (ReadProcessMemory(this->handle, (void*)addr, buff, reqSize, &size))
	{
		return size;
	}
	return 0;
}

struct Manage::Process::FindProcSess
{
	const WChar *fileName;
	HANDLE hand;
	Bool isFirst;
};

Manage::Process::FindProcSess *Manage::Process::FindProcess(const UTF8Char *processName)
{
	Manage::Process::FindProcSess *sess;
	HANDLE hand;
	hand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hand == INVALID_HANDLE_VALUE)
		return 0;
	sess = MemAlloc(Manage::Process::FindProcSess, 1);
	sess->hand = hand;
	if (processName == 0)
	{
		sess->fileName = 0;
	}
	else
	{
		sess->fileName = Text::StrToWCharNew(processName);
	}
	sess->isFirst = true;
	return sess;
}

Manage::Process::FindProcSess *Manage::Process::FindProcessW(const WChar *processName)
{
	Manage::Process::FindProcSess *sess;
	HANDLE hand;
	hand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hand == INVALID_HANDLE_VALUE)
		return 0;
	sess = MemAlloc(Manage::Process::FindProcSess, 1);
	sess->hand = hand;
	if (processName == 0)
	{
		sess->fileName = 0;
	}
	else
	{
		sess->fileName = Text::StrCopyNew(processName);
	}
	sess->isFirst = true;
	return sess;
}

UTF8Char *Manage::Process::FindProcessNext(UTF8Char *processNameBuff, Manage::Process::FindProcSess *pfsess, Manage::Process::ProcessInfo *info)
{
#ifdef _WIN32_WCE
	PROCESSENTRY32 pe32;
	BOOL ret;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (pfsess->isFirst)
	{
		ret = Process32First(pfsess->hand, &pe32);
		pfsess->isFirst = false;
	}
	else
	{
		ret = Process32Next(pfsess->hand, &pe32);
	}

	while (ret)
	{
		if (pfsess->fileName)
		{
			if (IO::Path::FileNameCompareW(pfsess->fileName, pe32.szExeFile) == 0)
			{
				info->parentId = pe32.th32ParentProcessID;
				info->processId = pe32.th32ProcessID;
				info->threadCnt = pe32.cntThreads;
				return Text::StrWChar_UTF8(processNameBuff, pe32.szExeFile, -1);
			}
		}
		else
		{
			info->parentId = pe32.th32ParentProcessID;
			info->processId = pe32.th32ProcessID;
			info->threadCnt = pe32.cntThreads;
			return Text::StrWChar_UTF8(processNameBuff, pe32.szExeFile, -1);
		}
		ret = Process32Next(pfsess->hand, &pe32);
	}
	return 0;
#else
	PROCESSENTRY32W pe32;
	BOOL ret;
	pe32.dwSize = sizeof(PROCESSENTRY32W);
	if (pfsess->isFirst)
	{
		ret = Process32FirstW(pfsess->hand, &pe32);
		pfsess->isFirst = false;
	}
	else
	{
		ret = Process32NextW(pfsess->hand, &pe32);
	}

	while (ret)
	{
		if (pfsess->fileName)
		{
			if (IO::Path::FileNameCompareW(pfsess->fileName, pe32.szExeFile) == 0)
			{
				info->parentId = pe32.th32ParentProcessID;
				info->processId = pe32.th32ProcessID;
				info->threadCnt = pe32.cntThreads;
				return Text::StrWChar_UTF8(processNameBuff, pe32.szExeFile);
			}
		}
		else
		{
			info->parentId = pe32.th32ParentProcessID;
			info->processId = pe32.th32ProcessID;
			info->threadCnt = pe32.cntThreads;
			return Text::StrWChar_UTF8(processNameBuff, pe32.szExeFile);
		}
		ret = Process32NextW(pfsess->hand, &pe32);
	}
	return 0;
#endif
}

WChar *Manage::Process::FindProcessNextW(WChar *processNameBuff, Manage::Process::FindProcSess *pfsess, Manage::Process::ProcessInfo *info)
{
#ifdef _WIN32_WCE
	PROCESSENTRY32 pe32;
	BOOL ret;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (pfsess->isFirst)
	{
		ret = Process32First(pfsess->hand, &pe32);
		pfsess->isFirst = false;
	}
	else
	{
		ret = Process32Next(pfsess->hand, &pe32);
	}

	while (ret)
	{
		if (pfsess->fileName)
		{
			if (IO::Path::FileNameCompareW(pfsess->fileName, pe32.szExeFile) == 0)
			{
				info->parentId = pe32.th32ParentProcessID;
				info->processId = pe32.th32ProcessID;
				info->threadCnt = pe32.cntThreads;
				return Text::StrConcat(processNameBuff, pe32.szExeFile);
			}
		}
		else
		{
			info->parentId = pe32.th32ParentProcessID;
			info->processId = pe32.th32ProcessID;
			info->threadCnt = pe32.cntThreads;
			return Text::StrConcat(processNameBuff, pe32.szExeFile);
		}
		ret = Process32Next(pfsess->hand, &pe32);
	}
	return 0;
#else
	PROCESSENTRY32W pe32;
	BOOL ret;
	pe32.dwSize = sizeof(PROCESSENTRY32W);
	if (pfsess->isFirst)
	{
		ret = Process32FirstW(pfsess->hand, &pe32);
		pfsess->isFirst = false;
	}
	else
	{
		ret = Process32NextW(pfsess->hand, &pe32);
	}

	while (ret)
	{
		if (pfsess->fileName)
		{
			if (IO::Path::FileNameCompareW(pfsess->fileName, pe32.szExeFile) == 0)
			{
				info->parentId = pe32.th32ParentProcessID;
				info->processId = pe32.th32ProcessID;
				info->threadCnt = pe32.cntThreads;
				return Text::StrConcat(processNameBuff, pe32.szExeFile);
			}
		}
		else
		{
			info->parentId = pe32.th32ParentProcessID;
			info->processId = pe32.th32ProcessID;
			info->threadCnt = pe32.cntThreads;
			return Text::StrConcat(processNameBuff, pe32.szExeFile);
		}
		ret = Process32NextW(pfsess->hand, &pe32);
	}
	return 0;
#endif
}

void Manage::Process::FindProcessClose(Manage::Process::FindProcSess *pfsess)
{
	if (pfsess->fileName)
	{
		Text::StrDelNew(pfsess->fileName);
	}
	CloseToolhelp32Snapshot(pfsess->hand);
	MemFree(pfsess);
}

Int32 Manage::Process::ExecuteProcess(const UTF8Char *cmd, Text::StringBuilderUTF *result)
{
	const WChar *wptr = Text::StrToWCharNew(cmd);
	Int32 ret = ExecuteProcessW(wptr, result);
	Text::StrDelNew(wptr);
	return ret;
}

Int32 Manage::Process::ExecuteProcessW(const WChar *cmd, Text::StringBuilderUTF *result)
{
	WChar buff[MAX_PATH];
	WChar progName[MAX_PATH];
	UTF8Char tmpFile[MAX_PATH];
	UOSInt cmdLen = Text::StrCharCnt(cmd);
	WChar *cmdLine = MemAlloc(WChar, cmdLen + 512);
	UTF8Char *sptr;
	Text::StrConcat(cmdLine, cmd);

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
	sptr = IO::Path::GetTempFile(tmpFile, (const UTF8Char*)"ProcessTmp");
	sptr = Text::StrUInt32(sptr, (UInt32)GetCurrProcId());
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
	sptr = Text::StrInt32(sptr, Sync::Interlocked::Increment(&Process_ExecFileId));
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".dat");

	SECURITY_ATTRIBUTES sa;
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	ZeroMemory(&startInfo, sizeof(startInfo));
#ifdef _WIN32_WCE
	cptr = &cmdLine[Text::StrCharCnt(cmdLine)];
	cptr = Text::StrConcat(cptr, L" > \"");
	cptr = Text::StrUTF8_WChar(cptr, tmpFile, -1, 0);
	cptr = Text::StrConcat(cptr, L"\"");
	createRet = CreateProcessW(0, cmdLine, 0, 0, true, 0, 0, buff, 0, &procInfo);
#else
	startInfo.cb = sizeof(startInfo);
	startInfo.dwFlags = STARTF_USESTDHANDLES;
	const WChar *wptr = Text::StrToWCharNew(tmpFile);
	startInfo.hStdOutput = CreateFileW(wptr, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	Text::StrDelNew(wptr);
	startInfo.hStdError = startInfo.hStdOutput;
	startInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	createRet = CreateProcessW(0, cmdLine, 0, 0, true, NORMAL_PRIORITY_CLASS, 0, buff, &startInfo, &procInfo);
#endif

	if(createRet)
	{
		UInt32 exitCode;
		WaitForSingleObject(procInfo.hProcess, INFINITE);
		GetExitCodeProcess(procInfo.hProcess, (LPDWORD)&exitCode);
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
		CloseHandle(startInfo.hStdOutput);
		CloseHandle(startInfo.hStdError);

		if (result)
		{
			IO::StreamReader *reader;
			IO::FileStream *fs;
			UTF8Char lineBuff[128];
			UTF8Char *linePtr;
			UOSInt retryCnt = 20;
			while (true)
			{
				NEW_CLASS(fs, IO::FileStream(tmpFile, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
				if (!fs->IsError() || retryCnt-- <= 0)
				{
					break;
				}
				DEL_CLASS(fs);
				Sync::Thread::Sleep(100);
			}
			NEW_CLASS(reader, IO::StreamReader(fs));
			while ((linePtr = reader->ReadLine(lineBuff, 124)) != 0)
			{
				reader->GetLastLineBreak(linePtr);
				result->Append(lineBuff);
			}
			DEL_CLASS(reader);
			DEL_CLASS(fs);
		}
		IO::FileUtil::DeleteFile(tmpFile, false);
		MemFree(cmdLine);
		return (Int32)exitCode;
	}
	else
	{
		CloseHandle(startInfo.hStdOutput);
		IO::FileUtil::DeleteFile(tmpFile, false);
		//UInt32 ret = GetLastError();
		MemFree(cmdLine);
		return -1;
	}

}

Bool Manage::Process::IsAlreadyStarted()
{
	WChar sbuff[512];
	Manage::Process::FindProcSess *sess;
	Bool found = false;
	UOSInt procId = GetCurrProcId();
	UOSInt i;
	IO::Path::GetProcessFileNameW(sbuff);
	Manage::Process::ProcessInfo info;

	i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
	{
		sess = Manage::Process::FindProcessW(&sbuff[i + 1]);
	}
	else
	{
		sess = Manage::Process::FindProcessW(sbuff);
	}
	if (sess)
	{
		while (Manage::Process::FindProcessNextW(sbuff, sess, &info))
		{
			if (info.processId != procId)
			{
				found = true;
				break;
			}
		}
		Manage::Process::FindProcessClose(sess);
	}
	return found;
}

Bool Manage::Process::OpenPath(const UTF8Char *path)
{
#ifdef _WIN32_WCE
	return false;
#else
	UOSInt strLen = Text::StrUTF8_WCharCnt(path);
	WChar *s = MemAlloc(WChar, strLen + 1);
	Text::StrUTF8_WChar(s, path, 0);
	Bool succ = 32 < (OSInt)ShellExecuteW(0, L"open", s, 0, 0, SW_SHOW);
	MemFree(s);
	return succ;
#endif
}

Bool Manage::Process::OpenPathW(const WChar *path)
{
#ifdef _WIN32_WCE
	return false;
#else
	Bool succ = 32 < (OSInt)ShellExecuteW(0, L"open", path, 0, 0, SW_SHOW);
	return succ;
#endif
}

const UTF8Char *Manage::Process::GetPriorityName(ProcessPriority priority)
{
	switch (priority)
	{
	case PP_REALTIME:
		return (const UTF8Char*)"Realtime";
	case PP_HIGH:
		return (const UTF8Char*)"High";
	case PP_ABOVE_NORMAL:
		return (const UTF8Char*)"Above Normal";
	case PP_NORMAL:
		return (const UTF8Char*)"Normal";
	case PP_BELOW_NORMAL:
		return (const UTF8Char*)"Below Normal";
	case PP_IDLE:
		return (const UTF8Char*)"Idle";
	case PP_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	}
}
