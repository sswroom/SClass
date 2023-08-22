#ifndef _SM_IO_FILEANALYSE_SPKFILEANALYSE
#define _SM_IO_FILEANALYSE_SPKFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class SPKFileAnalyse : public IO::FileAnalyse::IFileAnalyse
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
			Data::SyncArrayList<PackInfo*> packs;

			Bool pauseParsing;
			Sync::Thread thread;

			void ParseV1Directory(UInt64 dirOfst, UInt64 dirSize);
			void ParseV2Directory(UInt64 dirOfst, UInt64 dirSize);
			static void __stdcall ParseThread(NotNullPtr<Sync::Thread> thread);
			static void FreePackInfo(PackInfo *pack);
		public:
			SPKFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~SPKFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			void GetDetailDirs(const UInt8 *dirBuff, UOSInt dirSize, UOSInt frameOfst, IO::FileAnalyse::FrameDetail *frame);
		};
	}
}
#endif
