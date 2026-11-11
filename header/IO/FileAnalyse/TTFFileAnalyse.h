#ifndef _SM_IO_FILEANALYSE_TTFFILEANALYSE
#define _SM_IO_FILEANALYSE_TTFFILEANALYSE
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class TTFFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			enum class PackType
			{
				Directory,
				Table
			};
			typedef struct
			{
				UInt64 fileOfst;
				UIntOS packSize;
				UIntOS packSizePad;
				PackType packType;
				UInt8 tableTag[4];
			} PackInfo;
		private:
			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<PackInfo> packs;
			Sync::Thread thread;

			static void __stdcall ParseThread(NN<Sync::Thread> thread);
			static void __stdcall FreePackInfo(NN<PackInfo> pack);
		public:
			TTFFileAnalyse(NN<IO::StreamData> fd);
			virtual ~TTFFileAnalyse();

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
