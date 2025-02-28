#ifndef _SM_IO_FILEANALYSE_SPKFILEANALYSE
#define _SM_IO_FILEANALYSE_SPKFILEANALYSE
#include "Data/SyncArrayListNN.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class SPKFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef enum
			{
				PT_HEADER,
				PT_V1DIRECTORY,
				PT_V2DIRECTORY,
				PT_FILE
			} PackType;
			typedef struct
			{
				UInt64 fileOfst;
				UOSInt packSize;
				PackType packType;
				Text::String *fileName;
			} PackInfo;
		private:
			IO::StreamData *fd;
			Data::SyncArrayListNN<PackInfo> packs;

			Bool pauseParsing;
			Sync::Thread thread;

			void ParseV1Directory(UInt64 dirOfst, UInt64 dirSize);
			void ParseV2Directory(UInt64 dirOfst, UInt64 dirSize);
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
			static void __stdcall FreePackInfo(NN<PackInfo> pack);
		public:
			SPKFileAnalyse(NN<IO::StreamData> fd);
			virtual ~SPKFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			void GetDetailDirs(UnsafeArray<const UInt8> dirBuff, UOSInt dirSize, UOSInt frameOfst, NN<IO::FileAnalyse::FrameDetail> frame);
		};
	}
}
#endif
