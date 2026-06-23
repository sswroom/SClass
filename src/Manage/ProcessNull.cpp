#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/FileStream.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringBuilderW.h"
#include "Text/UTF8Reader.h"

Manage::Process::Process(UIntOS procId, Bool controlRight)
{
	this->procId = procId;
	this->needRelease = true;
}

Manage::Process::Process()
{
	this->procId = 0;
	this->needRelease = false;
}
Manage::Process::Process(UnsafeArray<const WChar> cmdLine)
{
	this->procId = 0;
	this->needRelease = true;
}

Manage::Process::~Process()
{
}

UIntOS Manage::Process::GetCurrProcId()
{
	return 0;
}

UIntOS Manage::Process::GetProcId()
{
	return this->procId;
}

Bool Manage::Process::IsRunning() const
{
	return true;
}

Bool Manage::Process::Kill()
{
	return false;
}

UnsafeArray<WChar> Manage::Process::GetFilename(UnsafeArray<WChar> buff)
{
	return buff;
}

Bool Manage::Process::GetFilename(NN<Text::StringBuilderUTF8> sb)
{
	return false;
}

UIntOS Manage::Process::GetMemorySize()
{
	return 0;
}

Bool Manage::Process::SetMemorySize(UIntOS minSize, UIntOS maxSize)
{
	return false;
}

UIntOS Manage::Process::GetThreadIds(NN<Data::ArrayListNative<UInt32>> threadList)
{
	return 0;
}

void *Manage::Process::GetHandle()
{
	return 0;
}


UIntOS Manage::Process::GetModules(NN<Data::ArrayListNN<Manage::ModuleInfo>> modList)
{
	return 0;
}

UIntOS Manage::Process::GetThreads(NN<Data::ArrayListNN<Manage::ThreadInfo>> threadList)
{
	return 0;
}

UIntOS Manage::Process::GetHeapLists(NN<Data::ArrayListNative<UInt32>> heapList)
{
	return 0;
}

UIntOS Manage::Process::GetHeaps(NN<Data::ArrayListNN<HeapInfo>> heapList, UInt32 heapListId, UIntOS maxCount)
{
	return 0;

}

void Manage::Process::FreeHeaps(NN<Data::ArrayListNN<HeapInfo>> heapList)
{
}

Data::Timestamp Manage::Process::GetStartTime()
{
	return Data::Timestamp(0);
}

Bool Manage::Process::GetWorkingSetSize(OptOut<UIntOS> minSize, OptOut<UIntOS> maxSize)
{
	return false;
}

Bool Manage::Process::GetMemoryInfo(OptOut<UIntOS> pageFault, OptOut<UIntOS> workingSetSize, OptOut<UIntOS> pagedPoolUsage, OptOut<UIntOS> nonPagedPoolUsage, OptOut<UIntOS> pageFileUsage)
{
	return false;
}

Bool Manage::Process::GetTimeInfo(OptOut<Data::Timestamp> createTime, OptOut<Data::Timestamp> kernelTime, OptOut<Data::Timestamp> userTime)
{
	return false;
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
	return 0;
}

Manage::Process::ProcessPriority Manage::Process::GetPriority()
{
	return PP_NORMAL;
}

Manage::ThreadContext::ContextType Manage::Process::GetContextType()
{
#if defined(CPU_X86_32)
	return Manage::ThreadContext::ContextType::X86_32;
#elif defined(CPU_X86_64)
	return Manage::ThreadContext::ContextType::X86_64;
#elif defined(CPU_ARM)
	return Manage::ThreadContext::ContextType::ARM;
#elif defined(CPU_MIPS)
	return Manage::ThreadContext::ContextType::MIPS;
#elif defined(CPU_AVR)
	return Manage::ThreadContext::ContextType::AVR;
#else
	return Manage::ThreadContext::ContextType::X86_32;
#endif
}
UInt8 Manage::Process::ReadMemUInt8(UInt64 addr)
{
	UInt8 buff[1];
	IntOS size = this->ReadMemory(addr, buff, 1);
	if (size == 1)
		return buff[0];
	return 0;
}

UInt16 Manage::Process::ReadMemUInt16(UInt64 addr)
{
	UInt8 buff[2];
	IntOS size = this->ReadMemory(addr, buff, 2);
	if (size == 2)
		return ReadUInt16(&buff[0]);
	return 0;
}

UInt32 Manage::Process::ReadMemUInt32(UInt64 addr)
{
	UInt8 buff[4];
	IntOS size = this->ReadMemory(addr, buff, 4);
	if (size == 4)
		return ReadUInt32(&buff[0]);
	return 0;
}

UInt64 Manage::Process::ReadMemUInt64(UInt64 addr)
{
	UInt8 buff[8];
	IntOS size = this->ReadMemory(addr, buff, 8);
	if (size == 8)
		return ReadUInt64(&buff[0]);
	return 0;
}

UIntOS Manage::Process::ReadMemory(UInt64 addr, UnsafeArray<UInt8> buff, UIntOS reqSize)
{
	const UInt8 *srcPtr = (const UInt8*)(UIntOS)addr;
	MemCopyNO(buff.Ptr(), srcPtr, reqSize);
	return reqSize;
}

Optional<Manage::Process::FindProcSess> Manage::Process::FindProcess(Text::CString processName)
{
	return nullptr;
}

Optional<Manage::Process::FindProcSess> Manage::Process::FindProcessW(UnsafeArrayOpt<const WChar> processName)
{
	return nullptr;
}

UnsafeArrayOpt<UTF8Char> Manage::Process::FindProcessNext(UnsafeArray<UTF8Char> processNameBuff, NN<FindProcSess> sess, NN<ProcessInfo> info)
{
	return nullptr;
}

UnsafeArrayOpt<WChar> Manage::Process::FindProcessNextW(UnsafeArray<WChar> processNameBuff, NN<FindProcSess> sess, NN<ProcessInfo> info)
{
	return nullptr;
}

void Manage::Process::FindProcessClose(NN<FindProcSess> sess)
{
}

Int32 Manage::Process::ExecuteProcess(Text::CStringNN cmd, NN<Text::StringBuilderUTF8> result)
{
	return -1;
}

Int32 Manage::Process::ExecuteProcessW(UnsafeArray<const WChar> cmd, NN<Text::StringBuilderUTF8> result)
{
	return -1;
}

Bool Manage::Process::IsAlreadyStarted()
{
	return false;
}

Bool Manage::Process::OpenPath(Text::CStringNN path)
{
	return false;
}

Text::CStringNN Manage::Process::GetPriorityName(ProcessPriority priority)
{
	switch (priority)
	{
	case PP_REALTIME:
		return CSTR("Realtime");
	case PP_HIGH:
		return CSTR("High");
	case PP_ABOVE_NORMAL:
		return CSTR("Above Normal");
	case PP_NORMAL:
		return CSTR("Normal");
	case PP_BELOW_NORMAL:
		return CSTR("Below Normal");
	case PP_IDLE:
		return CSTR("Idle");
	default:
		return CSTR("Unknown");
	}
}

