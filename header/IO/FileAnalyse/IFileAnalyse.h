#ifndef _SM_IO_FILEANALYSE_IFILEANALYSE
#define _SM_IO_FILEANALYSE_IFILEANALYSE
#include "Data/ArrayList.h"
#include "IO/IFileSelector.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/FrameDetail.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace FileAnalyse
	{
		class IFileAnalyse
		{
		public:
			virtual ~IFileAnalyse();

			virtual const UTF8Char *GetFormatName() = 0;
			virtual UOSInt GetFrameCount() = 0;
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF *sb) = 0;
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst) = 0;
			virtual FrameDetail *GetFrameDetail(UOSInt index) = 0;

			virtual Bool IsError() = 0;
			virtual Bool IsParsing() = 0;
			virtual Bool TrimPadding(const UTF8Char *outputFile) = 0;

			static IFileAnalyse *AnalyseFile(IO::IStreamData *fd);
			static void AddFilters(IO::IFileSelector *selector);
		};
	}
}
#endif
