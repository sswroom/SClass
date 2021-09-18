#ifndef _SM_IO_FILEANALYSE_RIFFFILEANALYSE
#define _SM_IO_FILEANALYSE_RIFFFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace FileAnalyse
	{
		class RIFFFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef struct
			{
				UInt64 fileOfst;
				UOSInt packSize;
				Int32 packType;
				Int32 subPackType;
			} PackInfo;
		private:
			IO::IStreamData *fd;
			Data::SyncArrayList<PackInfo*> *packs;
			Int32 mpgVer;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			void ParseRange(UInt64 ofst, UInt64 size);
			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			RIFFFileAnalyse(IO::IStreamData *fd);
			virtual ~RIFFFileAnalyse();

			virtual const UTF8Char *GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF *sb);
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(const UTF8Char *outputFile);
		};
	}
}
#endif
