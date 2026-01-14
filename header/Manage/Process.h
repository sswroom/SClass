// link with Psapi.lib
#ifndef _SM_MANAGE_PROCESS
#define _SM_MANAGE_PROCESS
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListNN.hpp"
#include "Manage/HandleType.h"
#include "Manage/MemoryReader.h"
#include "Manage/ModuleInfo.h"
#include "Manage/ThreadInfo.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Manage
{
	class Process : public Manage::MemoryReader
	{
	public:
		struct FindProcSess;

		struct HandleInfo
		{
			Int32 id;
			Data::Timestamp createTime;

			HandleInfo() = default;
			HandleInfo(Int32 id)
			{
				this->id = id;
				this->createTime = 0;
			}

			HandleInfo(Int32 id, const Data::Timestamp &createTime)
			{
				this->id = id;
				this->createTime = createTime;
			}

			Bool operator==(const HandleInfo &val)
			{
				return this->id == val.id;
			}
		};

		typedef enum
		{
			PP_REALTIME = 0x00000100,
			PP_HIGH = 0x00000080,
			PP_ABOVE_NORMAL = 0x00008000,
			PP_NORMAL = 0x00000020,
			PP_BELOW_NORMAL = 0x00004000,
			PP_IDLE = 0x00000040,
			PP_UNKNOWN = 0
		} ProcessPriority;

		typedef enum
		{
			HT_UNKNOWN,
			HT_FIXED,
			HT_MOVABLE,
			HT_FREE
		} HeapType;

		typedef struct
		{
			UOSInt startAddr;
			UOSInt size;
			HeapType heapType;
		} HeapInfo;

	private:
		UOSInt procId;
		void *handle;
		Bool needRelease;

	public:
		Process(UOSInt procId, Bool controlRight);
		Process();
		Process(UnsafeArray<const UTF8Char> cmdLine);
		Process(const WChar *cmdLine);
		virtual ~Process();

		static UOSInt GetCurrProcId();

		UOSInt GetProcId();
		Bool IsRunning() const;
		Bool Kill();
		UnsafeArray<WChar> GetFilename(UnsafeArray<WChar> buff);
		Bool GetFilename(NN<Text::StringBuilderUTF8> sb);
		Bool GetCommandLine(NN<Text::StringBuilderUTF8> sb);
		Bool GetWorkingDir(NN<Text::StringBuilderUTF8> sb);
		Bool GetTrueProgramPath(NN<Text::StringBuilderUTF8> sb);
		UOSInt GetMemorySize();
		Bool SetMemorySize(UOSInt minSize, UOSInt maxSize);
		UOSInt GetThreadIds(NN<Data::ArrayListNative<UInt32>> threadList);
		void *GetHandle();
		UOSInt GetModules(NN<Data::ArrayListNN<Manage::ModuleInfo>> modList);
		UOSInt GetThreads(NN<Data::ArrayListNN<Manage::ThreadInfo>> threadList);
		UOSInt GetHeapLists(NN<Data::ArrayListNative<UInt32>> heapList);
		UOSInt GetHeaps(NN<Data::ArrayListNN<HeapInfo>> heapList, UInt32 heapListId, UOSInt maxCount);
		void FreeHeaps(NN<Data::ArrayListNN<HeapInfo>> heapList);
		Data::Timestamp GetStartTime();
		UOSInt GetHandles(NN<Data::ArrayListNative<HandleInfo>> handleList);
		Bool GetHandleDetail(Int32 id, OutParam<HandleType> handleType, NN<Text::StringBuilderUTF8> sbDetail);

		Bool GetWorkingSetSize(UOSInt *minSize, UOSInt *maxSize);
		Bool GetMemoryInfo(OptOut<UOSInt> pageFault, OptOut<UOSInt> workingSetSize, OptOut<UOSInt> pagedPoolUsage, OptOut<UOSInt> nonPagedPoolUsage, OptOut<UOSInt> pageFileUsage);
		Bool GetTimeInfo(OptOut<Data::Timestamp> createTime, OptOut<Data::Timestamp> kernelTime, OptOut<Data::Timestamp> userTime);
		UInt32 GetGDIObjCount();
		UInt32 GetUserObjCount();
		UInt32 GetHandleCount();
		ProcessPriority GetPriority();
		Manage::ThreadContext::ContextType GetContextType();

		virtual UInt8 ReadMemUInt8(UInt64 addr);
		virtual UInt16 ReadMemUInt16(UInt64 addr);
		virtual UInt32 ReadMemUInt32(UInt64 addr);
		virtual UInt64 ReadMemUInt64(UInt64 addr);
		virtual UOSInt ReadMemory(UInt64 addr, UnsafeArray<UInt8> buff, UOSInt reqSize);
	public:
		typedef struct
		{
			UInt32 processId;
			UInt32 threadCnt;
			UInt32 parentId;
		} ProcessInfo;

		static Optional<FindProcSess> FindProcess(Text::CString processName);
		static Optional<FindProcSess> FindProcessW(UnsafeArrayOpt<const WChar> processName);
		static UnsafeArrayOpt<UTF8Char> FindProcessNext(UnsafeArray<UTF8Char> processNameBuff, NN<FindProcSess> sess, NN<ProcessInfo> info);
		static UnsafeArrayOpt<WChar> FindProcessNextW(UnsafeArray<WChar> processNameBuff, NN<FindProcSess> sess, NN<ProcessInfo> info);
		static void FindProcessClose(NN<FindProcSess> sess);
		static Int32 ExecuteProcess(Text::CStringNN cmdLine, NN<Text::StringBuilderUTF8> result);
		static Int32 ExecuteProcessW(UnsafeArray<const WChar> cmdLine, NN<Text::StringBuilderUTF8> result);
		static Bool IsAlreadyStarted();
		static Bool OpenPath(Text::CStringNN path);
		static Bool OpenPathW(UnsafeArray<const WChar> path);
		static Text::CStringNN GetPriorityName(ProcessPriority priority);
	};
}
#endif
