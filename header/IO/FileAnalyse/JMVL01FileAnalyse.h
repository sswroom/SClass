#ifndef _SM_IO_FILEANALYSE_JMVL01FILEANALYSE
#define _SM_IO_FILEANALYSE_JMVL01FILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/CString.h"
#include "Text/StringBuilder.h"

namespace IO
{
	namespace FileAnalyse
	{
		class JMVL01FileAnalyse : public IO::FileAnalyse::IFileAnalyse
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
			Data::SyncArrayList<JMVL01Tag*> tags;

			Bool pauseParsing;
			Sync::Thread thread;

			static Text::CString GetTagName(UInt8 tagType);
			static void __stdcall ParseThread(NotNullPtr<Sync::Thread> thread);
		public:
			JMVL01FileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~JMVL01FileAnalyse();

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
