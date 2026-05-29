#ifndef _SM_IO_FILEANALYSE_ETLFILEANALYSE
#define _SM_IO_FILEANALYSE_ETLFILEANALYSE
#include "Data/ByteBuffer.h"
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class ETLFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef struct
			{
				UInt64 ofst;
				UInt32 blockSize;
				UInt16 recordId;
				UInt16 level;
				UInt32 processorId;
				UInt64 timestamp;
			} RecordInfo;
		private:
			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<RecordInfo> recordList;
			Sync::Mutex dataMut;
			Data::ByteBuffer packetBuff;

			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			ETLFileAnalyse(NN<IO::StreamData> fd);
			virtual ~ETLFileAnalyse();

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
