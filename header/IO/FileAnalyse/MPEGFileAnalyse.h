#ifndef _SM_IO_FILEANALYSE_MPEGFILEANALYSE
#define _SM_IO_FILEANALYSE_MPEGFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/StringBuilder.h"

namespace IO
{
	namespace FileAnalyse
	{
		class MPEGFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef struct
			{
				UInt64 fileOfst;
				UOSInt packSize;
				UInt8 packType;
			} PackInfo;
		private:
			IO::StreamData *fd;
			Data::SyncArrayList<PackInfo*> packs;
			Int32 mpgVer;

			Bool pauseParsing;
			Sync::Thread thread;
			static void __stdcall ParseThread(NotNullPtr<Sync::Thread> thread);
		public:
			MPEGFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~MPEGFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual Bool GetFrameDetail(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
