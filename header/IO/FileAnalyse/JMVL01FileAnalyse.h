#ifndef _SM_IO_FILEANALYSE_JMVL01FILEANALYSE
#define _SM_IO_FILEANALYSE_JMVL01FILEANALYSE
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/CString.h"
#include "Text/StringBuilder.hpp"

namespace IO
{
	namespace FileAnalyse
	{
		class JMVL01FileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef struct
			{
				UInt8 tagType;
				UInt64 ofst;
				UOSInt size;
			} JMVL01Tag;
		private:
			IO::StreamData *fd;
			Data::SyncArrayListNN<JMVL01Tag> tags;

			Bool pauseParsing;
			Sync::Thread thread;

			static Text::CString GetTagName(UInt8 tagType);
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			JMVL01FileAnalyse(NN<IO::StreamData> fd);
			virtual ~JMVL01FileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
