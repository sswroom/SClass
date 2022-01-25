#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/Int32Map.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Manage/Process.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <errno.h>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#define __pid_t pid_t
#else
#include <link.h>
#endif
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>

#include <wchar.h>

#if __GLIBC__ < 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ < 15)
ssize_t process_vm_readv(pid_t pid, const struct iovec *local_iov, unsigned long liovcnt,
	const struct iovec *remote_iov, unsigned long riovcnt, unsigned long flags)
{
	MemCopyNO(local_iov->iov_base, remote_iov->iov_base, remote_iov->iov_len);
	return remote_iov->iov_len;
}
#endif

Manage::Process::Process(UOSInt procId, Bool controlRight)
{
	this->procId = procId;
	this->needRelease = true;
}

Manage::Process::Process()
{
	this->procId = (UOSInt)getpid();
	this->needRelease = false;
}

Manage::Process::Process(const UTF8Char *ccmdLine)
{
	UTF8Char progName[512];
	const UTF8Char *cptr = ccmdLine;
	UTF8Char *args[10];
	Int32 argc = 0;
	Bool argStart = false;


	UOSInt progNameLen = 0;
	UTF8Char *pptr = progName;
	Bool isQuote = false;
	UTF8Char c;
	args[0] = pptr;
	while ((c = *cptr++) != 0)
	{
		if (c == '"')
			isQuote = !isQuote;
		else if (!isQuote && c == ' ')
		{
			if (!argStart)
			{
				*pptr++ = 0;
				argStart = true;
				if (argc == 0)
				{
					progNameLen = (UOSInt)(pptr - progName);
				}
			}
		}
		else
		{
			if (argStart)
			{
				argc++;
				args[argc] = pptr;
				argStart = false;
			}
			*pptr++ = c;
		}
	}
	*pptr = 0;
	if (argc == 0)
	{
		progNameLen = (UOSInt)(pptr - progName);
	}
	args[++argc] = 0;

	pid_t pid = fork();
	if (pid == 0)
	{
		UOSInt i = Text::StrLastIndexOfCharC(progName, progNameLen, IO::Path::PATH_SEPERATOR);
		if (i != INVALID_INDEX)
		{
			progName[i] = 0;
			if (chdir((Char*)progName) == -1)
			{
				printf("Process: cannot change to program path\r\n");
			}
			progName[i] = IO::Path::PATH_SEPERATOR;
		}
		int ret = execvp((Char*)progName, (Char**)args);
		exit(ret);
		return;
	}
	this->procId = (UOSInt)pid;
	this->needRelease = true;
}

Manage::Process::Process(const WChar *cmdLine)
{
	UTF8Char progName[512];
	const UTF8Char *ccmdLine = Text::StrToUTF8New(cmdLine);
	const UTF8Char *cptr = ccmdLine;
	UTF8Char *args[10];
	Int32 argc = 0;
	Bool argStart = false;

	UOSInt progNameLen = 0;
	UTF8Char *pptr = progName;
	Bool isQuote = false;
	UTF8Char c;
	args[0] = pptr;
	while ((c = *cptr++) != 0)
	{
		if (c == '"')
			isQuote = !isQuote;
		else if (!isQuote && c == ' ')
		{
			if (!argStart)
			{
				*pptr++ = 0;
				argStart = true;
				if (argc == 0)
				{
					progNameLen = (UOSInt)(pptr - progName);
				}
			}
		}
		else
		{
			if (argStart)
			{
				argc++;
				args[argc] = pptr;
				argStart = false;
			}
			*pptr++ = c;
		}
	}
	*pptr = 0;
	if (argc == 0)
	{
		progNameLen = (UOSInt)(pptr - progName);
	}
	args[++argc] = 0;

	pid_t pid = fork();
	if (pid == 0)
	{
		UOSInt i = Text::StrLastIndexOfCharC(progName, progNameLen, IO::Path::PATH_SEPERATOR);
		if (i != INVALID_INDEX)
		{
			progName[i] = 0;
			if (chdir((Char*)progName) == -1)
			{
				printf("Process: cannot change to program path\r\n");
			}
			progName[i] = IO::Path::PATH_SEPERATOR;
		}
		int ret = execvp((Char*)progName, (Char**)args);
		Text::StrDelNew(ccmdLine);
		exit(ret);
		return;
	}
	Text::StrDelNew(ccmdLine);
	this->procId = (UOSInt)pid;
	this->needRelease = true;
}

Manage::Process::~Process()
{
	if (needRelease)
	{
		//CloseHandle((HANDLE)this->handle);
//		this->handle = 0;
	}
}

UOSInt Manage::Process::GetCurrProcId()
{
	return (UOSInt)getpid();
}

UOSInt Manage::Process::GetProcId()
{
	return this->procId;
}

Bool Manage::Process::IsRunning()
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Int32 exitCode;
	waitpid((__pid_t)this->procId, &exitCode, WNOHANG);
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("/proc/")), this->procId);
	if (IO::Path::GetPathType(sbuff, (UOSInt)(sptr - sbuff)) == IO::Path::PathType::Directory)
	{
		return true;
	}
	return false;
}

Bool Manage::Process::Kill()
{
	kill((__pid_t)this->procId, SIGKILL);
	return true;
}

WChar *Manage::Process::GetFilename(WChar *buff)
{
	UTF8Char sbuff2[512];
	UTF8Char sbuff[128];
	Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("/proc/")), this->procId), UTF8STRC("/exe"));
	OSInt sz = readlink((const Char*)sbuff, (Char*)sbuff2, 511);
	if (sz < 0)
	{
		Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("/proc/")), this->procId), UTF8STRC("/cmdline"));
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		sz = (OSInt)fs->Read((UInt8*)sbuff2, 511);
		DEL_CLASS(fs);
		sbuff2[sz] = 0;
		buff = Text::StrUTF8_WChar(buff, (const UTF8Char*)sbuff2, 0);
	}
	else
	{
		sbuff2[sz] = 0;
		buff = Text::StrUTF8_WChar(buff, (const UTF8Char*)sbuff2, 0);
	}
	return buff;
}

Bool Manage::Process::GetFilename(Text::StringBuilderUTF8 *sb)
{
	UTF8Char sbuff2[512];
	UTF8Char sbuff[128];
	Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("/proc/")), this->procId), UTF8STRC("/exe"));
	OSInt sz = readlink((const Char*)sbuff, (Char*)sbuff2, 511);
	if (sz < 0)
	{
		Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("/proc/")), this->procId), UTF8STRC("/cmdline"));
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (fs->IsError())
		{
			sz = 0;
		}
		else
		{
			sz = (OSInt)fs->Read((UInt8*)sbuff2, 511);
		}
		DEL_CLASS(fs);
		sbuff2[sz] = 0;
		sb->Append((const UTF8Char*)sbuff2);
	}
	else
	{
		sbuff2[sz] = 0;
		sb->Append((const UTF8Char*)sbuff2);
	}
	return true;
}

UOSInt Manage::Process::GetMemorySize()
{
	return 0;
}

Bool Manage::Process::SetMemorySize(UOSInt minSize, UOSInt maxSize)
{
	return false;
}

UOSInt Manage::Process::GetThreadIds(Data::ArrayList<UInt32> *threadList)
{
	return 0;
}

void *Manage::Process::GetHandle()
{
	return 0;
}

typedef struct
{
	const UTF8Char *fileName;
	UOSInt addr;
	UOSInt size;
} ModuleInfoData;

#if !defined(__APPLE__)
int Process_GetModulesCB(struct dl_phdr_info *info, size_t size, void *data)
{
	Data::ArrayList<Manage::ModuleInfo *> *modList = (Data::ArrayList<Manage::ModuleInfo *> *)data;
	Manage::ModuleInfo *mod;
	ModuleInfoData midata;
	UTF8Char sbuff[512];

	midata.addr = info->dlpi_addr;
	if (info->dlpi_name[0])
	{
		Text::StrConcat(sbuff, (UTF8Char*)info->dlpi_name);
	}
	else
	{
		IO::Path::GetProcessFileName(sbuff);
	}
	midata.fileName = sbuff;
	UOSInt totalSize = 0;
	UOSInt i;
	i = 0;
	while (i < info->dlpi_phnum)
	{
		totalSize += info->dlpi_phdr[i].p_memsz;
		i++;
	}
	midata.size = totalSize;

	NEW_CLASS(mod, Manage::ModuleInfo(0, &midata));
	modList->Add(mod);
	return 0;
}
#endif

UOSInt Manage::Process::GetModules(Data::ArrayList<Manage::ModuleInfo *> *modList)
{
#if defined(__APPLE__)
	if (this->procId == getpid())
	{
		Manage::ModuleInfo *mod;
		ModuleInfoData midata;
		OSInt i;
		OSInt ret = (OSInt)_dyld_image_count;
		UTF8Char sbuff[512];
		const Char *name;
		const struct mach_header *hdr;
		i = 0;
		while (i < ret)
		{
			name = _dyld_get_image_name((UInt32)i);
			if (name)
			{
				Text::StrConcat(sbuff, (UTF8Char*)name);
				midata.fileName = sbuff;
			}
			else
			{
				midata.fileName = (const UTF8Char*)"Unknown";
			}
			midata.addr = (OSInt)_dyld_get_image_vmaddr_slide((UInt32)i);
			midata.size = 0;
			NEW_CLASS(mod, Manage::ModuleInfo(0, &midata));
			modList->Add(mod);
			i++;
		}
		return ret;
	}
	else
#elif !defined(__ANDROID__)
	if (this->procId == (UOSInt)getpid())
	{
		UOSInt ret = modList->GetCount();
		dl_iterate_phdr(Process_GetModulesCB, modList);
		return modList->GetCount() - ret;
	}
	else
#endif
	{
		UTF8Char sbuff[128];
		UTF8Char *sarr[6];
		UTF8Char *sarr2[2];
		IO::FileStream *fs;
		Text::UTF8Reader *reader;
		Data::Int32Map<ModuleInfoData*> dataMap;
		Data::ArrayList<ModuleInfoData*> *dataList;
		ModuleInfoData *data;
		Manage::ModuleInfo *mod;
		UOSInt ret = 0;
		UOSInt i;
		Text::StringBuilderUTF8 sb;
		Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("/proc/")), this->procId), UTF8STRC("/maps"));
		NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		while (reader->ReadLine(&sb, 512))
		{
			ret = Text::StrSplitTrim(sarr, 6, sb.ToString(), ' ');
			if (ret == 6)
			{
				Int32 inode = Text::StrToInt32(sarr[4]);
				if (inode != 0)
				{
					if (Text::StrSplit(sarr2, 2, sarr[0], '-') == 2)
					{
						UOSInt startAddr = (UOSInt)Text::StrHex2Int64C(sarr2[0]);
						UOSInt endAddr = (UOSInt)Text::StrHex2Int64C(sarr2[1]);
						data = dataMap.Get(inode);
						if (data)
						{
							data->size += (UOSInt)(endAddr - startAddr);
						}
						else
						{
							data = MemAlloc(ModuleInfoData, 1);
							data->fileName = Text::StrCopyNew(sarr[5]);
							data->addr = startAddr;
							data->size = (UOSInt)(endAddr - startAddr);
							dataMap.Put(inode, data);
						}
					}
				}
			}
			sb.ClearStr();
		}
		DEL_CLASS(reader);
		DEL_CLASS(fs);

		dataList = dataMap.GetValues();
		i = 0;
		ret = dataList->GetCount();
		while (i < ret)
		{
			data = dataList->GetItem(i);

			NEW_CLASS(mod, Manage::ModuleInfo(0, data));
			modList->Add(mod);
			Text::StrDelNew(data->fileName);
			MemFree(data);
			i++;
		}
		return ret;
	}
}

UOSInt Manage::Process::GetThreads(Data::ArrayList<Manage::ThreadInfo *> *threadList)
{
	UTF8Char sbuff[128];
	DIR *dir;
	UOSInt retCnt = 0;
	UInt32 threadId;
	Manage::ThreadInfo *thread;
	Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("/proc/")), this->procId), UTF8STRC("/task"));
	dir = opendir((Char*)sbuff);
	if (dir)
	{
		struct dirent *dirent;
		while ((dirent = readdir(dir)) != 0)
		{
			threadId = Text::StrToUInt32(dirent->d_name);
			if (threadId != 0)
			{
				NEW_CLASS(thread, Manage::ThreadInfo(this->procId, threadId));
				threadList->Add(thread);
				retCnt++;
			}
		}
		closedir(dir);
	}
	return retCnt;
}

UOSInt Manage::Process::GetHeapLists(Data::ArrayList<UInt32> *heapList)
{
	return 0;
}

UOSInt Manage::Process::GetHeaps(Data::ArrayList<HeapInfo*> *heapList, UInt32 heapListId, UOSInt maxCount)
{
	return 0;

}

void Manage::Process::FreeHeaps(Data::ArrayList<HeapInfo*> *heapList)
{
}

Bool Manage::Process::GetWorkingSetSize(UOSInt *minSize, UOSInt *maxSize)
{
	return false;
}

Bool Manage::Process::GetMemoryInfo(UOSInt *pageFault, UOSInt *workingSetSize, UOSInt *pagedPoolUsage, UOSInt *nonPagedPoolUsage, UOSInt *pageFileUsage)
{
	UOSInt pageSize = (UOSInt)sysconf(_SC_PAGESIZE);
	UTF8Char sbuff[128];
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Bool succ = false;
	Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("/proc/")), this->procId), UTF8STRC("/statm"));
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	sb.ClearStr();
	if (reader->ReadLine(&sb, 512))
	{
		UTF8Char *sarr[8];
		if (Text::StrSplit(sarr, 8, sb.ToString(), ' ') == 7)
		{
			UOSInt wsSize = (UOSInt)Text::StrToUInt64(sarr[1]);
			succ = true;
			if (pageFault)
			{
				*pageFault = 0;
			}
			if (workingSetSize)
			{
				*workingSetSize = wsSize * pageSize;
			}
			if (pagedPoolUsage)
			{
				*pagedPoolUsage = (UOSInt)Text::StrToUInt64(sarr[2]) * pageSize;
			}
			if (nonPagedPoolUsage)
			{
				*nonPagedPoolUsage = (UOSInt)Text::StrToUInt64(sarr[5]) * pageSize;
			}
			if (pageFileUsage)
			{
				*pageFileUsage = (UOSInt)Text::StrToUInt64(sarr[0]) * pageSize;
			}
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	return succ;
}

Bool Manage::Process::GetTimeInfo(Data::DateTime *createTime, Data::DateTime *kernelTime, Data::DateTime *userTime)
{
	OSInt hertz = (OSInt)sysconf(_SC_CLK_TCK);
	UTF8Char sbuff[128];
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Bool succ = false;
	Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("/proc/")), this->procId), UTF8STRC("/stat"));
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	sb.ClearStr();
	if (reader->ReadLine(&sb, 512))
	{
		UTF8Char *sarr[24];
		if (Text::StrSplit(sarr, 24, sb.ToString(), ' ') >= 23)
		{
			succ = true;
			if (createTime)
			{
				createTime->SetTicks(Text::StrToInt64(sarr[21]) * 1000LL / hertz);
			}
			if (kernelTime)
			{
				kernelTime->SetTicks(Text::StrToInt64(sarr[14]) * 1000LL / hertz);
			}
			if (userTime)
			{
				userTime->SetTicks(Text::StrToInt64(sarr[13]) * 1000LL / hertz);
			}
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	return succ;
}


UInt32 Manage::Process::GetGDIObjCount()
{
	return 0;
}

UInt32 Manage::Process::GetUserObjCount()
{
	return 0;
}

UInt32 Manage::Process::GetHandleCount()
{
	UTF8Char sbuff[128];
	DIR *dir;
	UInt32 retCnt = 0;
	Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("/proc/")), this->procId), UTF8STRC("/fd"));
	dir = opendir((Char*)sbuff);
	if (dir)
	{
		struct dirent *dirent;
		while ((dirent = readdir(dir)) != 0)
		{
			retCnt++;
		}
		closedir(dir);
	}
	return retCnt;
}

Manage::Process::ProcessPriority Manage::Process::GetPriority()
{
	int priority = getpriority(PRIO_PROCESS, (id_t)this->procId);
	if (priority < -16)
	{
		return PP_REALTIME;
	}
	else if (priority < -8)
	{
		return PP_HIGH;
	}
	else if (priority < 0)
	{
		return PP_ABOVE_NORMAL;
	}
	else if (priority < 8)
	{
		return PP_NORMAL;
	}
	else if (priority < 16)
	{
		return PP_BELOW_NORMAL;
	}
	else
	{
		return PP_IDLE;
	}
}

Manage::ThreadContext::ContextType Manage::Process::GetContextType()
{
#if defined(CPU_X86_32)
	return Manage::ThreadContext::CT_X86_32;
#elif defined(CPU_X86_64)
	return Manage::ThreadContext::CT_X86_64;
#elif defined(CPU_ARM)
	return Manage::ThreadContext::CT_ARM;
#elif defined(CPU_MIPS)
	return Manage::ThreadContext::CT_MIPS;
#else
	return Manage::ThreadContext::CT_X86_32;
#endif
}
UInt8 Manage::Process::ReadMemUInt8(UInt64 addr)
{
	UInt8 buff[1];
	UOSInt size = this->ReadMemory(addr, buff, 1);
	if (size == 1)
		return buff[0];
	return 0;
}

UInt16 Manage::Process::ReadMemUInt16(UInt64 addr)
{
	UInt8 buff[2];
	UOSInt size = this->ReadMemory(addr, buff, 2);
	if (size == 2)
		return ReadUInt16(&buff[0]);
	return 0;
}

UInt32 Manage::Process::ReadMemUInt32(UInt64 addr)
{
	UInt8 buff[4];
	UOSInt size = this->ReadMemory(addr, buff, 4);
	if (size == 4)
		return ReadUInt32(&buff[0]);
	return 0;
}

UInt64 Manage::Process::ReadMemUInt64(UInt64 addr)
{
	UInt8 buff[8];
	UOSInt size = this->ReadMemory(addr, buff, 8);
	if (size == 8)
		return ReadUInt64(&buff[0]);
	return 0;
}

UOSInt Manage::Process::ReadMemory(UInt64 addr, UInt8 *buff, UOSInt reqSize)
{
	struct iovec srcInfo;
	struct iovec destInfo;
	destInfo.iov_base = buff;
	destInfo.iov_len = reqSize;
	srcInfo.iov_base = (void*)(UOSInt)addr;
	srcInfo.iov_len = reqSize;
	OSInt i = process_vm_readv((pid_t)this->procId, &destInfo, 1, &srcInfo, 1, 0);
	if (i < 0)
	{
		i = 0;
	}
	return (UOSInt)i;
}

struct Manage::Process::FindProcSess
{
	IO::Path::FindFileSession *findFileSess;
	Text::String *procName;
};

Manage::Process::FindProcSess *Manage::Process::FindProcess(const UTF8Char *processName)
{
	IO::Path::FindFileSession *ffsess = IO::Path::FindFile(UTF8STRC("/proc/*"));
	FindProcSess *sess;
	if (ffsess == 0)
	{
		return 0;
	}
	sess = MemAlloc(FindProcSess, 1);
	sess->findFileSess = ffsess;
	if (processName)
	{
		sess->procName = Text::String::NewNotNull(processName);
	}
	else
	{
		sess->procName = 0;
	}
	return sess;
}

Manage::Process::FindProcSess *Manage::Process::FindProcessW(const WChar *processName)
{
	IO::Path::FindFileSession *ffsess = IO::Path::FindFile(UTF8STRC("/proc/*"));
	FindProcSess *sess;
	if (ffsess == 0)
	{
		return 0;
	}
	sess = MemAlloc(FindProcSess, 1);
	sess->findFileSess = ffsess;
	if (processName)
	{
		sess->procName = Text::String::NewNotNull(processName);
	}
	else
	{
		sess->procName = 0;
	}
	return sess;
}

UTF8Char *Manage::Process::FindProcessNext(UTF8Char *processNameBuff, Manage::Process::FindProcSess *fpsess, Manage::Process::ProcessInfo *info)
{
	UInt32 pid;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::Path::PathType pt;
	Text::StringBuilderUTF8 sb;
	Bool found = false;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("/proc/"));
	while ((sptr2 = IO::Path::FindNextFile(sptr, fpsess->findFileSess, 0, &pt, 0)) != 0)
	{
		if (pt == IO::Path::PathType::Directory && Text::StrToUInt32(sptr, &pid))
		{
			IO::FileStream *fs;
			Text::UTF8Reader *reader;

			info->processId = pid;
			info->threadCnt = 0;
			info->parentId = 0;

			Text::StrConcatC(sptr2, UTF8STRC("/status"));
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			if (!fs->IsError())
			{
				NEW_CLASS(reader, Text::UTF8Reader(fs));
				sb.ClearStr();
				while (reader->ReadLine(&sb, 512))
				{
					if (sb.StartsWith(UTF8STRC("PPid:\t")))
					{
						info->parentId = Text::StrToUInt32(sb.ToString() + 6);
					}
					else if (sb.StartsWith(UTF8STRC("Threads:\t")))
					{
						info->threadCnt = Text::StrToUInt32(sb.ToString() + 9);
					}
					sb.ClearStr();
				}
				DEL_CLASS(reader);
			}
			DEL_CLASS(fs);

			Text::StrConcatC(sptr2, UTF8STRC("/comm"));
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			if (!fs->IsError())
			{
				NEW_CLASS(reader, Text::UTF8Reader(fs));
				sb.ClearStr();
				while (reader->ReadLine(&sb, 512))
				{
				}
				DEL_CLASS(reader);
				if (fpsess->procName == 0 || sb.Equals(fpsess->procName))
				{
					found = true;
				}
			}
			DEL_CLASS(fs);

			if (found)
			{
				return Text::StrConcatC(processNameBuff, sb.ToString(), sb.GetLength());
			}
		}
	}
	return 0;
}

WChar *Manage::Process::FindProcessNextW(WChar *processNameBuff, Manage::Process::FindProcSess *fpsess, Manage::Process::ProcessInfo *info)
{
	UInt32 pid;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::Path::PathType pt;
	Text::StringBuilderUTF8 sb;
	Bool found = false;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("/proc/"));
	while ((sptr2 = IO::Path::FindNextFile(sptr, fpsess->findFileSess, 0, &pt, 0)) != 0)
	{
		if (pt == IO::Path::PathType::Directory && Text::StrToUInt32(sptr, &pid))
		{
			IO::FileStream *fs;
			Text::UTF8Reader *reader;

			info->processId = pid;
			info->threadCnt = 0;
			info->parentId = 0;

			Text::StrConcatC(sptr2, UTF8STRC("/status"));
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			if (!fs->IsError())
			{
				NEW_CLASS(reader, Text::UTF8Reader(fs));
				sb.ClearStr();
				while (reader->ReadLine(&sb, 512))
				{
					if (sb.StartsWith(UTF8STRC("PPid:\t")))
					{
						info->parentId = Text::StrToUInt32(sb.ToString() + 6);
					}
					else if (sb.StartsWith(UTF8STRC("Threads:\t")))
					{
						info->threadCnt = Text::StrToUInt32(sb.ToString() + 9);
					}
					sb.ClearStr();
				}
				DEL_CLASS(reader);
			}
			DEL_CLASS(fs);

			Text::StrConcatC(sptr2, UTF8STRC("/comm"));
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			if (!fs->IsError())
			{
				NEW_CLASS(reader, Text::UTF8Reader(fs));
				sb.ClearStr();
				while (reader->ReadLine(&sb, 512))
				{
				}
				DEL_CLASS(reader);
				if (fpsess->procName == 0 || sb.Equals(fpsess->procName))
				{
					found = true;
				}
			}
			DEL_CLASS(fs);

			if (found)
			{
				return Text::StrUTF8_WChar(processNameBuff, sb.ToString(), 0);
			}
		}
	}
	return 0;
}

void Manage::Process::FindProcessClose(Manage::Process::FindProcSess *fpsess)
{
	IO::Path::FindFileClose(fpsess->findFileSess);
	SDEL_STRING(fpsess->procName);
	MemFree(fpsess);
}

Int32 Manage::Process::ExecuteProcess(Text::PString *cmd, Text::StringBuilderUTF8 *result)
{
	UTF8Char progName[64];
	UTF8Char *progBuff = 0;
	const UTF8Char *cptr = cmd->v;
	Data::ArrayList<UTF8Char *> args;
	Bool argStart = false;

	UOSInt cmdLen = cmd->leng;
	UTF8Char *pptr;
	if (cmdLen >= 64)
	{
		progBuff = MemAlloc(UTF8Char, cmdLen + 1);
		pptr = progBuff;
	}
	else
	{
		pptr = progName;
	}
	Bool isQuote = false;
	UTF8Char c;
	args.Add(pptr);
	while ((c = *cptr++) != 0)
	{
		if (c == '"')
			isQuote = !isQuote;
		else if (!isQuote && c == ' ')
		{
			if (!argStart)
			{
				*pptr++ = 0;
				argStart = true;
			}
		}
		else
		{
			if (argStart)
			{
				args.Add(pptr);
				argStart = false;
			}
			*pptr++ = c;
		}
	}
	*pptr = 0;
	args.Add(0);
	UOSInt argc;
	UTF8Char **arr = args.GetArray(&argc);

	static Int32 Process_Id = 0;
	UTF8Char tmpFile[512];
	UTF8Char *sptr;
	if (IO::Path::GetPathType(UTF8STRC("/tmp")) == IO::Path::PathType::Directory)
	{
		sptr = Text::StrConcatC(tmpFile, UTF8STRC("/tmp/ExecuteProcess"));
	}
	else
	{
		sptr = Text::StrConcatC(tmpFile, UTF8STRC("ExecuteProcess"));
	}
	sptr = Text::StrUInt32(sptr, (UInt32)GetCurrProcId());
	sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
	sptr = Text::StrInt32(sptr, Sync::Interlocked::Increment(&Process_Id));
	sptr = Text::StrConcatC(sptr, UTF8STRC(".tmp"));
	int fd = open((Char*)tmpFile, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	pid_t pid = fork();
	int ret = -1;
	if (pid == 0)
	{
		dup2(fd, 1);
		ret = execvp((Char*)arr[0], (Char**)arr);
		exit(ret);
	}
	int status = -1;
	if (waitpid(pid, &status, 0) == -1)
	{
		//printf("execvp: waitpid failed, pid = %d, errno = %d\r\n", pid, errno);
	}
	else
	{
		if (WIFEXITED(status))
		{
			ret = WEXITSTATUS(status);
		}
	}

	UInt8 buff[129];
	OSInt readSize;
	lseek(fd, 0, SEEK_SET);
	while ((readSize = read(fd, buff, 128)) > 0)
	{
//		printf("Read %d bytes\r\n", readSize);
		result->AppendC((const UTF8Char*)buff, (UOSInt)readSize);
	}
//	printf("Process exited\r\n");
	close(fd);
	unlink((Char*)tmpFile);
	if (progBuff)
	{
		MemFree(progBuff);
	}
	return ret;
}

Int32 Manage::Process::ExecuteProcess(const UTF8Char *cmd, UOSInt cmdLen, Text::StringBuilderUTF8 *result)
{
	UTF8Char progName[64];
	UTF8Char *progBuff = 0;
	const UTF8Char *cptr = cmd;
	Data::ArrayList<UTF8Char *> args;
	Bool argStart = false;

	UTF8Char *pptr;
	if (cmdLen >= 64)
	{
		progBuff = MemAlloc(UTF8Char, cmdLen + 1);
		pptr = progBuff;
	}
	else
	{
		pptr = progName;
	}
	Bool isQuote = false;
	UTF8Char c;
	args.Add(pptr);
	while ((c = *cptr++) != 0)
	{
		if (c == '"')
			isQuote = !isQuote;
		else if (!isQuote && c == ' ')
		{
			if (!argStart)
			{
				*pptr++ = 0;
				argStart = true;
			}
		}
		else
		{
			if (argStart)
			{
				args.Add(pptr);
				argStart = false;
			}
			*pptr++ = c;
		}
	}
	*pptr = 0;
	args.Add(0);
	UOSInt argc;
	UTF8Char **arr = args.GetArray(&argc);

	static Int32 Process_Id = 0;
	UTF8Char tmpFile[512];
	UTF8Char *sptr;
	if (IO::Path::GetPathType(UTF8STRC("/tmp")) == IO::Path::PathType::Directory)
	{
		sptr = Text::StrConcatC(tmpFile, UTF8STRC("/tmp/ExecuteProcess"));
	}
	else
	{
		sptr = Text::StrConcatC(tmpFile, UTF8STRC("ExecuteProcess"));
	}
	sptr = Text::StrUInt32(sptr, (UInt32)GetCurrProcId());
	sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
	sptr = Text::StrInt32(sptr, Sync::Interlocked::Increment(&Process_Id));
	sptr = Text::StrConcatC(sptr, UTF8STRC(".tmp"));
	int fd = open((Char*)tmpFile, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	pid_t pid = fork();
	int ret = -1;
	if (pid == 0)
	{
		dup2(fd, 1);
		ret = execvp((Char*)arr[0], (Char**)arr);
		exit(ret);
	}
	int status = -1;
	if (waitpid(pid, &status, 0) == -1)
	{
		//printf("execvp: waitpid failed, pid = %d, errno = %d\r\n", pid, errno);
	}
	else
	{
		if (WIFEXITED(status))
		{
			ret = WEXITSTATUS(status);
		}
	}

	UInt8 buff[129];
	OSInt readSize;
	lseek(fd, 0, SEEK_SET);
	while ((readSize = read(fd, buff, 128)) > 0)
	{
//		printf("Read %d bytes\r\n", readSize);
		result->AppendC((const UTF8Char*)buff, (UOSInt)readSize);
	}
//	printf("Process exited\r\n");
	close(fd);
	unlink((Char*)tmpFile);
	if (progBuff)
	{
		MemFree(progBuff);
	}
	return ret;
}

Int32 Manage::Process::ExecuteProcessW(const WChar *cmd, Text::StringBuilderUTF8 *result)
{
	Text::String *s = Text::String::NewNotNull(cmd);
	Int32 ret = ExecuteProcess(s->v, s->leng, result);
	s->Release();
	return ret;
}

Bool Manage::Process::IsAlreadyStarted()
{
	return false;
}

Bool Manage::Process::OpenPath(const UTF8Char *path)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("xdg-open "));
	sb.Append(path);
	Int32 ret = ExecuteProcess(sb.ToString(), sb.GetLength(), &sb);
	return ret == 0;
}

Bool Manage::Process::OpenPathW(const WChar *path)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("xdg-open "));
	Text::String *s = Text::String::NewNotNull(path);
	sb.Append(s);
	s->Release();
	Int32 ret = ExecuteProcess(sb.ToString(), sb.GetLength(), &sb);
	return ret == 0;
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
