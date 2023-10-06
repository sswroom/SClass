#ifndef _SM_IO_FILEANALYSE_ZIPFILEANALYSE
#define _SM_IO_FILEANALYSE_ZIPFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/CString.h"
#include "Text/StringBuilder.h"

namespace IO
{
	namespace FileAnalyse
	{
		class ZIPFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			struct ZIPRecord
			{
				UInt32 tagType;
				UInt64 ofst;
				UInt64 size;
			};
		private:
			IO::StreamData *fd;
			Data::SyncArrayList<ZIPRecord*> tags;

			Bool pauseParsing;
			Sync::Thread thread;

			static Text::CString GetTagName(UInt32 tagType);
			static Text::CString GetCompName(UInt16 comp);
			UOSInt ParseCentDir(const UInt8 *buff, UOSInt buffSize, UInt64 ofst);
			UOSInt AddCentDir(const UInt8 *buff, UOSInt buffSize, UInt64 ofst);
			static void __stdcall ParseThread(NotNullPtr<Sync::Thread> thread);
			static void ParseExtraTag(IO::FileAnalyse::FrameDetail *frame, Data::ByteArrayR tagData, UOSInt extraStart, UOSInt extraLen, UOSInt tagSize, UInt32 compSize, UInt32 uncompSize, UInt32 ofst);
		public:
			ZIPFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~ZIPFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
