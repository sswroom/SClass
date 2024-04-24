#ifndef _SM_IO_FILEANALYSE_SSLFILEANALYSE
#define _SM_IO_FILEANALYSE_SSLFILEANALYSE
#include "Data/SyncArrayListNN.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class SSLFileAnalyse : public IO::FileAnalyse::IFileAnalyse
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

			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NotNullPtr<Sync::Thread> thread);
			static void FreePackInfo(NN<PackInfo> pack);

			static UOSInt AppendExtension(NotNullPtr<IO::FileAnalyse::FrameDetail> frame, Data::ByteArrayR buff, UOSInt ofst, UOSInt totalLeng);
		public:
			SSLFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~SSLFileAnalyse();

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
