#ifndef _SM_IO_FILEANALYSE_MDBFILEANALYSE
#define _SM_IO_FILEANALYSE_MDBFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Text/CString.h"
#include "Text/StringBuilder.h"

namespace IO
{
	namespace FileAnalyse
	{
		class MDBFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			typedef struct
			{
				UInt64 fileOfst;
				UOSInt packSize;
				UInt16 packType;
			} PackInfo;
		private:
			IO::StreamData *fd;
			Data::SyncArrayList<PackInfo*> packs;
			UInt32 fileFormat;
			UInt32 fileVer;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;
			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			MDBFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~MDBFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			static Text::CString ColumnTypeGetName(UInt8 colType);
		};
	}
}
#endif
