#ifndef _SM_IO_FILEANALYSE_JPGFILEANALYSE
#define _SM_IO_FILEANALYSE_JPGFILEANALYSE
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/CString.h"
#include "Text/StringBuilder.hpp"

namespace IO
{
	namespace FileAnalyse
	{
		class JPGFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef struct
			{
				UInt8 tagType;
				UInt64 ofst;
				UIntOS size;
			} JPGTag;
		private:
			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<JPGTag> tags;

			Bool pauseParsing;
			Sync::Thread thread;

			static Text::CString GetTagName(UInt8 tagType);
			static void __stdcall ParseThread(NN<Sync::Thread> userObj);
		public:
			JPGFileAnalyse(NN<IO::StreamData> fd);
			virtual ~JPGFileAnalyse();

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
