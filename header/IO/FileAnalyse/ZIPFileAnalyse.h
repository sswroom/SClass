#ifndef _SM_IO_FILEANALYSE_ZIPFILEANALYSE
#define _SM_IO_FILEANALYSE_ZIPFILEANALYSE
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Thread.h"
#include "Text/CString.h"
#include "Text/StringBuilder.hpp"

namespace IO
{
	namespace FileAnalyse
	{
		class ZIPFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			struct ZIPRecord
			{
				UInt32 tagType;
				UInt64 ofst;
				UInt64 size;
				Optional<Text::String> fileName;
			};
		private:
			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<ZIPRecord> tags;

			Bool pauseParsing;
			Sync::Thread thread;

			static Text::CString GetTagName(UInt32 tagType);
			static Text::CString GetCompName(UInt16 comp);
			UIntOS ParseCentDir(NN<IO::StreamData> fd, UnsafeArray<const UInt8> buff, UIntOS buffSize, UInt64 ofst);
			UIntOS AddCentDir(UnsafeArray<const UInt8> buff, UIntOS buffSize, UInt64 ofst);
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
			static void ParseExtraTag(NN<IO::FileAnalyse::FrameDetail> frame, Data::ByteArrayR tagData, UIntOS extraStart, UIntOS extraLen, UIntOS tagSize, UInt32 compSize, UInt32 uncompSize, UInt32 ofst);
		public:
			ZIPFileAnalyse(NN<IO::StreamData> fd);
			virtual ~ZIPFileAnalyse();

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
