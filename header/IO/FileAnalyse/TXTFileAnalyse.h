#ifndef _SM_IO_FILEANALYSE_TXTFILEANALYSE
#define _SM_IO_FILEANALYSE_TXTFILEANALYSE
#include "Data/ArrayListUInt64.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class TXTFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			IO::StreamData *fd;
			Data::ArrayListUInt64 lineOfsts;
			Sync::Mutex mut;
			UInt64 fileSize;

			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NotNullPtr<Sync::Thread> thread);
		public:
			TXTFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~TXTFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
