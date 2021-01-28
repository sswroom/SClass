#ifndef _SM_IO_FILEANALYSE_QTFILEANALYSE
#define _SM_IO_FILEANALYSE_QTFILEANALYSE
#include "Data/ArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace FileAnalyse
	{
		class QTFileAnalyse : public IO::FileAnalyse::IFileAnalyse
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
			Data::ArrayList<PackInfo*> *packs;
			Sync::Mutex *packMut;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			void ParseRange(Int64 ofst, Int64 size);
			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			QTFileAnalyse(IO::IStreamData *fd);
			virtual ~QTFileAnalyse();

			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF *sb);
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(const UTF8Char *outputFile);
		};
	}
}
#endif
