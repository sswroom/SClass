#ifndef _SM_IO_FILEANALYSE_IFILEANALYSE
#define _SM_IO_FILEANALYSE_IFILEANALYSE
#include "Data/ArrayList.h"
#include "IO/FileSelector.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FrameDetail.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class IFileAnalyse
		{
		public:
			virtual ~IFileAnalyse();

			virtual Text::CStringNN GetFormatName() = 0;
			virtual UOSInt GetFrameCount() = 0;
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb) = 0;
			virtual Bool GetFrameDetail(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst) = 0;
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index) = 0;

			virtual Bool IsError() = 0;
			virtual Bool IsParsing() = 0;
			virtual Bool TrimPadding(Text::CStringNN outputFile) = 0;

			static Optional<IFileAnalyse> AnalyseFile(NotNullPtr<IO::StreamData> fd);
			static void AddFilters(NotNullPtr<IO::FileSelector> selector);
		};
	}
}
#endif
