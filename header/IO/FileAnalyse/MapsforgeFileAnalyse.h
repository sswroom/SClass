#ifndef _SM_IO_FILEANALYSE_MAPSFORGEFILEANALYSE
#define _SM_IO_FILEANALYSE_MAPSFORGEFILEANALYSE
#include "Data/SyncArrayListNN.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/CString.h"
#include "Text/StringBuilder.h"

namespace IO
{
	namespace FileAnalyse
	{
		class MapsforgeFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef struct
			{
				UInt64 fileOfst;
				UOSInt packSize;
				UInt16 packType;
			} PackInfo;
		private:
			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<PackInfo> packs;

			Bool pauseParsing;
			Sync::Thread thread;
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
			static UOSInt ReadVBEU(UnsafeArray<UInt8> buff, UOSInt ofst, OutParam<UInt64> v);
		public:
			MapsforgeFileAnalyse(NN<IO::StreamData> fd);
			virtual ~MapsforgeFileAnalyse();

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
