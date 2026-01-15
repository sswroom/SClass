#ifndef _SM_IO_FILEANALYSE_FILEANALYSER
#define _SM_IO_FILEANALYSE_FILEANALYSER
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
		class FileAnalyser
		{
		public:
			virtual ~FileAnalyser();

			virtual Text::CStringNN GetFormatName() = 0;
			virtual UIntOS GetFrameCount() = 0;
			virtual Bool GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb) = 0;
			virtual Bool GetFrameDetail(UIntOS index, NN<Text::StringBuilderUTF8> sb);
			virtual UIntOS GetFrameIndex(UInt64 ofst) = 0;
			virtual Optional<FrameDetail> GetFrameDetail(UIntOS index) = 0;

			virtual Bool IsError() = 0;
			virtual Bool IsParsing() = 0;
			virtual Bool TrimPadding(Text::CStringNN outputFile) = 0;

			static Optional<FileAnalyser> AnalyseFile(NN<IO::StreamData> fd);
			static void AddFilters(NN<IO::FileSelector> selector);
		};
	}
}
#endif
