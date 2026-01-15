#ifndef _SM_IO_FILEANALYSE_SMTCFILEANALYSE
#define _SM_IO_FILEANALYSE_SMTCFILEANALYSE
#include "Data/ArrayListNN.hpp"
#include "Data/ByteBuffer.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class SMTCFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			struct DataInfo
			{
				UInt64 ofst;
				UIntOS size;
				UInt8 type;
			};
			
		private:
			Optional<IO::StreamData> fd;
			Data::ArrayListNN<DataInfo> dataList;
			Sync::Mutex dataMut;
			Data::ByteBuffer packetBuff;

			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			SMTCFileAnalyse(NN<IO::StreamData> fd);
			virtual ~SMTCFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UIntOS GetFrameCount();
			virtual Bool GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb);
			virtual UIntOS GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UIntOS index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
