#ifndef _SM_IO_FILEANALYSE_ZIPFILEANALYSE
#define _SM_IO_FILEANALYSE_ZIPFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
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
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			static Text::CString GetTagName(UInt32 tagType);
			static Text::CString GetCompName(UInt16 comp);
			UOSInt ParseCentDir(const UInt8 *buff, UOSInt buffSize, UInt64 ofst);
			UOSInt AddCentDir(const UInt8 *buff, UOSInt buffSize, UInt64 ofst);
			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			ZIPFileAnalyse(IO::StreamData *fd);
			virtual ~ZIPFileAnalyse();

			virtual Text::CString GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CString outputFile);
		};
	}
}
#endif
