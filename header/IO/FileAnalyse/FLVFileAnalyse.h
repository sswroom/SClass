#ifndef _SM_IO_FILEANALYSE_FLVFILEANALYSE
#define _SM_IO_FILEANALYSE_FLVFILEANALYSE
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
		class FLVFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef struct
			{
				UInt8 tagType;
				UInt64 ofst;
				UIntOS size;
			} FLVTag;
		private:
			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<FLVTag> tags;
			UIntOS hdrSize;

			Bool pauseParsing;
			Sync::Thread thread;

			UIntOS ParseScriptDataVal(UnsafeArray<UInt8> data, UIntOS ofst, UIntOS endOfst, NN<Text::StringBuilderUTF8> sb);
			void ParseScriptData(UnsafeArray<UInt8> data, UIntOS ofst, UIntOS endOfst, UIntOS frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			FLVFileAnalyse(NN<IO::StreamData> fd);
			virtual ~FLVFileAnalyse();

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
