#ifndef _SM_IO_FILEANALYSE_SHPFILEANALYSE
#define _SM_IO_FILEANALYSE_SHPFILEANALYSE
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class SHPFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			struct PackInfo
			{
				UInt64 fileOfst;
				UIntOS packSize;
			};
		private:
			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<PackInfo> packs;

			Bool pauseParsing;
			Sync::Thread thread;

			static Text::CString ShapeTypeGetName(UInt32 shapeType);
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			SHPFileAnalyse(NN<IO::StreamData> fd);
			virtual ~SHPFileAnalyse();

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
