#ifndef _SM_IO_FILEANALYSE_TSFILEANALYSE
#define _SM_IO_FILEANALYSE_TSFILEANALYSE
#include "Data/ArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class TSFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			IO::IStreamData *fd;
			UInt32 packSize;
			UInt64 fileSize;
			Bool hasTime;

		public:
			TSFileAnalyse(IO::IStreamData *fd);
			virtual ~TSFileAnalyse();

			virtual Text::CString GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CString outputFile);
		};
	}
}
#endif
