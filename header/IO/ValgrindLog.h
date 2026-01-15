#ifndef _SM_IO_VALGRINDLOG
#define _SM_IO_VALGRINDLOG
#include "Data/ArrayListNN.hpp"
#include "IO/Stream.h"
#include "Text/String.h"

namespace IO
{
	class ValgrindLog
	{
	public:
		struct StackEntry
		{
			UInt64 address;
			NN<Text::String> funcName;
			Optional<Text::String> source;
		};

		struct LeakInfo
		{
			UIntOS threadId;
			NN<Text::String> message;
			Data::ArrayListNN<StackEntry> stacks;
		};

		enum class AddressType
		{
			NoAddress,
			Unknown,
			Stack,
			Alloc
		};
		struct ExceptionInfo
		{
			UIntOS threadId;
			NN<Text::String> message;
			UInt64 accessAddress;
			AddressType addrType;
			UInt32 blockOfst;
			UInt32 stackSize;
			Data::ArrayListNN<StackEntry> stacks;
			Data::ArrayListNN<StackEntry> allocStacks;
		};
	private:
		UIntOS ppid;
		UIntOS mainPID;
		Optional<Text::String> version;
		Optional<Text::String> commandLine;
		UInt64 bytesInUse;
		UInt32 blocksInUse;
		UInt32 blocksAllocs;
		UInt32 blocksFrees;
		UInt64 bytesAllocs;
		Data::ArrayListNN<LeakInfo> leaks;
		Data::ArrayListNN<ExceptionInfo> exceptions;

		Optional<LeakInfo> currLeak;
		Optional<ExceptionInfo> currException;
		Optional<Data::ArrayListNN<StackEntry>> currStack;

		void SetPPID(UIntOS ppid);
		void SetVersion(Text::CStringNN version);
		void SetCommandLine(Text::CStringNN commandLine);
		void BeginLeak(UIntOS threadId, Text::CStringNN message);
		void BeginException(UIntOS threadId, Text::CStringNN message);
		void NewStack(UInt64 address, Text::CStringNN funcName, Text::CString source);
		void SetExceptionAccessAddress(UInt64 address, AddressType addrType);
		void SetExceptionStackSize(UInt32 size);
		void ExceptionBeginAlloc(UInt64 address, UInt32 blockOfst, UInt32 blockSize);
		void SetHeapInUse(UInt64 bytesInUse, UInt32 blocksInUse);
		void SetTotalHeaps(UInt32 blocksAllocs, UInt32 blocksFrees, UInt64 bytesAllocs);

		static void __stdcall FreeStack(NN<StackEntry> stack);
		static void __stdcall FreeLeak(NN<LeakInfo> leak);
		static void __stdcall FreeException(NN<ExceptionInfo> ex);
	public:
		ValgrindLog(UIntOS mainPID);
		~ValgrindLog();

		UIntOS GetMainPID() const;
		UIntOS GetPPID() const;
		Optional<Text::String> GetVersion() const;
		Optional<Text::String> GetCommandLine() const;
		UInt64 GetBytesInUse() const;
		UInt32 GetBlocksInUse() const;
		UInt32 GetBlocksAllocs() const;
		UInt32 GetBlocksFrees() const;
		UInt64 GetBytesAllocs() const;
		NN<const Data::ArrayListNN<ExceptionInfo>> GetErrorList() const;
		NN<const Data::ArrayListNN<LeakInfo>> GetLeakList() const;

		static Optional<ValgrindLog> LoadFile(Text::CStringNN filePath);
		static Optional<ValgrindLog> LoadStream(NN<IO::Stream> stream);
	};
}
#endif
