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
				Text::String *fileName;
			};
		private:
			IO::StreamData *fd;
			Data::SyncArrayListNN<ZIPRecord> tags;

			Bool pauseParsing;
			Sync::Thread thread;

			static Text::CString GetTagName(UInt32 tagType);
			static Text::CString GetCompName(UInt16 comp);
			UOSInt ParseCentDir(UnsafeArray<const UInt8> buff, UOSInt buffSize, UInt64 ofst);
			UOSInt AddCentDir(UnsafeArray<const UInt8> buff, UOSInt buffSize, UInt64 ofst);
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
			static void ParseExtraTag(NN<IO::FileAnalyse::FrameDetail> frame, Data::ByteArrayR tagData, UOSInt extraStart, UOSInt extraLen, UOSInt tagSize, UInt32 compSize, UInt32 uncompSize, UInt32 ofst);
		public:
			ZIPFileAnalyse(NN<IO::StreamData> fd);
			virtual ~ZIPFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
