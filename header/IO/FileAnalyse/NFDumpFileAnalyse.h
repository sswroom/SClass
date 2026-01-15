#ifndef _SM_IO_FILEANALYSE_NFDUMPFILEANALYSE
#define _SM_IO_FILEANALYSE_NFDUMPFILEANALYSE
#include "Data/FastMapObj.hpp"
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class NFDumpFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef struct
			{
				UInt64 fileOfst;
				UIntOS packSize;
				Int32 packType;
			} PackInfo;
		private:
			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<PackInfo> packs;
			Data::FastMapObj<Int32, UnsafeArrayOpt<UInt8>> extMap;
			Bool hasLZODecomp;

			Bool pauseParsing;
			Sync::Thread thread;
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
			UIntOS LZODecompBlock(UnsafeArray<UInt8> srcBlock, UIntOS srcSize, UnsafeArray<UInt8> outBlock, UIntOS maxOutSize);
		public:
			NFDumpFileAnalyse(NN<IO::StreamData> fd);
			virtual ~NFDumpFileAnalyse();

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
