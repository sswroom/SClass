#ifndef _SM_IO_FILEANALYSE_TIFFFILEANALYSE
#define _SM_IO_FILEANALYSE_TIFFFILEANALYSE
#include "Data/ByteOrder.h"
#include "Data/SyncArrayListNN.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class TIFFFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef enum
			{
				PT_HEADER,
				PT_RESERVED,
				PT_IFD,
				PT_IFD8
			} PackType;
			typedef struct
			{
				UInt64 fileOfst;
				UOSInt packSize;
				PackType packType;
			} PackInfo;
		private:
			IO::StreamData *fd;
			Data::SyncArrayListNN<PackInfo> packs;
			Data::ByteOrder *bo;

			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NN<Sync::Thread> thread);
			static void FreePackInfo(NN<PackInfo> pack);
		public:
			TIFFFileAnalyse(NN<IO::StreamData> fd);
			virtual ~TIFFFileAnalyse();

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
