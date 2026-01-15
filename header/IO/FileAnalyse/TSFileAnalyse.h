#ifndef _SM_IO_FILEANALYSE_TSFILEANALYSE
#define _SM_IO_FILEANALYSE_TSFILEANALYSE
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class TSFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			Optional<IO::StreamData> fd;
			UInt32 packSize;
			UInt64 fileSize;
			Bool hasTime;

		public:
			TSFileAnalyse(NN<IO::StreamData> fd);
			virtual ~TSFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UIntOS GetFrameCount();
			virtual Bool GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb);
			virtual Bool GetFrameDetail(UIntOS index, NN<Text::StringBuilderUTF8> sb);
			virtual UIntOS GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UIntOS index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
