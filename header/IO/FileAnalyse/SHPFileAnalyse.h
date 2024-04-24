#ifndef _SM_IO_FILEANALYSE_SHPFILEANALYSE
#define _SM_IO_FILEANALYSE_SHPFILEANALYSE
#include "Data/SyncArrayListNN.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class SHPFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			struct PackInfo
			{
				UInt64 fileOfst;
				UOSInt packSize;
			};
		private:
			IO::StreamData *fd;
			Data::SyncArrayListNN<PackInfo> packs;

			Bool pauseParsing;
			Sync::Thread thread;

			static Text::CString ShapeTypeGetName(UInt32 shapeType);
			static void __stdcall ParseThread(NotNullPtr<Sync::Thread> thread);
		public:
			SHPFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~SHPFileAnalyse();

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
