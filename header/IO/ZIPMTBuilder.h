#ifndef _SM_IO_ZIPMTBUILDER
#define _SM_IO_ZIPMTBUILDER
#include "Data/SyncLinkedList.h"
#include "IO/IStreamData.h"
#include "IO/ZIPBuilder.h"
#include "Sync/Event.h"

namespace IO
{
	class ZIPMTBuilder
	{
	private:
		enum class ThreadState
		{
			NotRunning,
			Idle,
			Processing,
			Stopped
		};

		struct ThreadStat
		{
			ZIPMTBuilder *me;
			Sync::Event *evt;
			ThreadState status;
		};

		struct FileTask
		{
			Text::String *fileName;
			UInt8 *fileBuff;
			UOSInt fileSize;
			Int64 fileTimeTicks;
			Data::Compress::Inflate::CompressionLevel compLevel;
		};
	private:
		ZIPBuilder zip;
		UOSInt threadCnt;
		ThreadStat *threads;
		Bool toStop;
		Sync::Event mainEvt;
		Data::SyncLinkedList taskList;

		static UInt32 __stdcall ThreadProc(void *userObj);
		static void FreeTask(FileTask *task);
		void AddTask(FileTask *task);
	public:
		ZIPMTBuilder(IO::SeekableStream *stm);
		~ZIPMTBuilder();

		Bool AddFile(Text::CString fileName, IO::SeekableStream *stm, Int64 fileTimeTicks, Data::Compress::Inflate::CompressionLevel compLevel);
		Bool AddFile(Text::CString fileName, IO::IStreamData *fd, Int64 fileTimeTicks, Data::Compress::Inflate::CompressionLevel compLevel);
	};
}
#endif
