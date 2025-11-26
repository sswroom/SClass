#ifndef _SM_IO_FILEANALYSE_PCAPNGFILEANALYSE
#define _SM_IO_FILEANALYSE_PCAPNGFILEANALYSE
#include "Data/ByteBuffer.h"
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class PCapngFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef struct
			{
				UInt64 ofst;
				UInt32 blockType;
				UInt32 blockLength;
				UInt16 linkType;
				Int8 timeResol;
			} BlockInfo;
		private:
			Optional<IO::StreamData> fd;
			Bool isBE;
			Data::SyncArrayListNN<BlockInfo> blockList;
			Data::ByteBuffer packetBuff;

			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			PCapngFileAnalyse(NN<IO::StreamData> fd);
			virtual ~PCapngFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual Bool GetFrameDetail(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			static void SetTime(NN<Data::DateTime> dt, Int64 ts, Int8 timeResol);
		};
	}
}
#endif
