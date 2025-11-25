#ifndef _SM_IO_FILEANALYSE_NFDUMPFILEANALYSE
#define _SM_IO_FILEANALYSE_NFDUMPFILEANALYSE
#include "Data/FastMap.hpp"
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
				UOSInt packSize;
				Int32 packType;
			} PackInfo;
		private:
			IO::StreamData *fd;
			Data::SyncArrayListNN<PackInfo> packs;
			Data::FastMap<Int32, UInt8*> extMap;
			Bool hasLZODecomp;

			Bool pauseParsing;
			Sync::Thread thread;
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
			UOSInt LZODecompBlock(UnsafeArray<UInt8> srcBlock, UOSInt srcSize, UnsafeArray<UInt8> outBlock, UOSInt maxOutSize);
		public:
			NFDumpFileAnalyse(NN<IO::StreamData> fd);
			virtual ~NFDumpFileAnalyse();

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
