#ifndef _SM_IO_FILEANALYSE_QTFILEANALYSE
#define _SM_IO_FILEANALYSE_QTFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class QTFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef struct
			{
				UOSInt lev;
				UInt64 fileOfst;
				UInt64 packSize;
				Int32 packType;
			} PackInfo;
		private:
			IO::IStreamData *fd;
			Data::SyncArrayList<PackInfo*> *packs;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;
			UOSInt maxLev;

			void ParseRange(UOSInt lev, UInt64 ofst, UInt64 size);
			static UInt32 __stdcall ParseThread(void *userObj);
			UOSInt GetFrameIndex(UOSInt lev, UInt64 ofst);
		public:
			QTFileAnalyse(IO::IStreamData *fd);
			virtual ~QTFileAnalyse();

			virtual const UTF8Char *GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(const UTF8Char *outputFile);
		};
	}
}
#endif
