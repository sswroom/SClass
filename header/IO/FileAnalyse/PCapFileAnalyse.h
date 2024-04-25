#ifndef _SM_IO_FILEANALYSE_PCAPFILEANALYSE
#define _SM_IO_FILEANALYSE_PCAPFILEANALYSE
#include "Data/ArrayList.h"
#include "Data/ByteBuffer.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class PCapFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			IO::StreamData *fd;
			Bool isBE;
			UInt32 linkType;
			Data::ArrayList<UInt64> ofstList;
			Data::ArrayList<UInt64> sizeList;
			Sync::Mutex dataMut;
			Data::ByteBuffer packetBuff;

			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			PCapFileAnalyse(NN<IO::StreamData> fd);
			virtual ~PCapFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual Bool GetFrameDetail(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
