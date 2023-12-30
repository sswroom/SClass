#ifndef _SM_IO_FILEANALYSE_JPGFILEANALYSE
#define _SM_IO_FILEANALYSE_JPGFILEANALYSE
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
		class JPGFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef struct
			{
				UInt8 tagType;
				UInt64 ofst;
				UOSInt size;
			} JPGTag;
		private:
			IO::StreamData *fd;
			Data::SyncArrayList<JPGTag*> tags;

			Bool pauseParsing;
			Sync::Thread thread;

			static Text::CString GetTagName(UInt8 tagType);
			static void __stdcall ParseThread(NotNullPtr<Sync::Thread> userObj);
		public:
			JPGFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~JPGFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual Bool GetFrameDetail(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
