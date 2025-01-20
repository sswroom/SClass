#ifndef _SM_IO_FILEANALYSE_MPEGFILEANALYSE
#define _SM_IO_FILEANALYSE_MPEGFILEANALYSE
#include "Data/SyncArrayListNN.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/StringBuilder.h"

namespace IO
{
	namespace FileAnalyse
	{
		class MPEGFileAnalyse : public IO::FileAnalyse::FileAnalyser
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
			Data::SyncArrayListNN<PackInfo> packs;
			Int32 mpgVer;

			Bool pauseParsing;
			Sync::Thread thread;
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			MPEGFileAnalyse(NN<IO::StreamData> fd);
			virtual ~MPEGFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual Bool GetFrameDetail(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
