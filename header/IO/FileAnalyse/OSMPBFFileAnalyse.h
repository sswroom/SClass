#ifndef _SM_IO_FILEANALYSE_OSMPBFFILEANALYSE
#define _SM_IO_FILEANALYSE_OSMPBFFILEANALYSE
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class OSMPBFFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			enum class PackType
			{
				Length,
				BlobHeader,
				Blob
			};
			typedef struct
			{
				UInt64 fileOfst;
				UIntOS packSize;
				PackType packType;
				Optional<Text::String> dataType;
			} PackInfo;
		private:
			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<PackInfo> packs;

			Bool pauseParsing;
			Sync::Thread thread;
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
			static void __stdcall FreePackInfo(NN<PackInfo> pack);
		public:
			OSMPBFFileAnalyse(NN<IO::StreamData> fd);
			virtual ~OSMPBFFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UIntOS GetFrameCount();
			virtual Bool GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb);
			virtual UIntOS GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UIntOS index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			static Text::CStringNN PackTypeGetName(PackType packType);
		};
	}
}
#endif
