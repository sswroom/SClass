#ifndef _SM_IO_FILEANALYSE_EXEFILEANALYSE
#define _SM_IO_FILEANALYSE_EXEFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace FileAnalyse
	{
		class EXEFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef struct
			{
				Int64 fileOfst;
				Int64 packSize;
				Int32 packType;
			} PackInfo;
		private:
			IO::IStreamData *fd;
			Data::SyncArrayList<PackInfo*> *packs;
			UInt8 *imageBuff;
			OSInt imageSize;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			EXEFileAnalyse(IO::IStreamData *fd);
			virtual ~EXEFileAnalyse();

			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF *sb);
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(const UTF8Char *outputFile);

			static const UTF8Char *PackTypeGetName(Int32 packType);
		};
	}
}
#endif
