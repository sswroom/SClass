#ifndef _SM_IO_FILEANALYSE_TSFILEANALYSE
#define _SM_IO_FILEANALYSE_TSFILEANALYSE
#include "Data/ArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace FileAnalyse
	{
		class TSFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			IO::IStreamData *fd;
			Int32 packSize;
			Int64 fileSize;
			Bool hasTime;

		public:
			TSFileAnalyse(IO::IStreamData *fd);
			virtual ~TSFileAnalyse();

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
