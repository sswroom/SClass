#ifndef _SM_IO_FILEANALYSE_QTFILEANALYSE
#define _SM_IO_FILEANALYSE_QTFILEANALYSE
#include "Data/SyncArrayListNN.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class QTFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef struct
			{
				UOSInt lev;
				UInt64 fileOfst;
				UInt64 packSize;
				Int32 packType;
			} PackInfo;
		private:
			IO::StreamData *fd;
			Data::SyncArrayListNN<PackInfo> packs;

			Bool pauseParsing;
			Sync::Thread thread;
			UOSInt maxLev;

			void ParseRange(UOSInt lev, UInt64 ofst, UInt64 size);
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
			UOSInt GetFrameIndex(UOSInt lev, UInt64 ofst);
		public:
			QTFileAnalyse(NN<IO::StreamData> fd);
			virtual ~QTFileAnalyse();

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
