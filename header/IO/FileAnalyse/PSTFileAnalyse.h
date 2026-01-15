#ifndef _SM_IO_FILEANALYSE_PSTFILEANALYSE
#define _SM_IO_FILEANALYSE_PSTFILEANALYSE
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class PSTFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			enum class PackType
			{
				Header
			};
			struct PackItem
			{
				PackType packType;
				UInt64 ofst;
				UIntOS size;
			};
		private:
			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<PackItem> items;
			Bool unicode;

			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			PSTFileAnalyse(NN<IO::StreamData> fd);
			virtual ~PSTFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UIntOS GetFrameCount();
			virtual Bool GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb);
			virtual UIntOS GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UIntOS index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			static Text::CStringNN PackTypeGetName(PackType packType);
			static Text::CString NIDTypeGetName(UInt32 nidType);
		};
	}
}
#endif
