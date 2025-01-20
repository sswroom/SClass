#ifndef _SM_IO_FILEANALYSE_PNGFILEANALYSE
#define _SM_IO_FILEANALYSE_PNGFILEANALYSE
#include "Data/SyncArrayListNN.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class PNGFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef struct
			{
				Int32 tagType;
				UInt32 crc;
				UInt64 ofst;
				UOSInt size;
			} PNGTag;
		private:
			IO::StreamData *fd;
			Data::SyncArrayListNN<PNGTag> tags;

			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			PNGFileAnalyse(NN<IO::StreamData> fd);
			virtual ~PNGFileAnalyse();

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
