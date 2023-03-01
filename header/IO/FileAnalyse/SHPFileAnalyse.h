#ifndef _SM_IO_FILEANALYSE_SHPFILEANALYSE
#define _SM_IO_FILEANALYSE_SHPFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class SHPFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			struct PackInfo
			{
				UInt64 fileOfst;
				UOSInt packSize;
			};
		private:
			IO::StreamData *fd;
			Data::SyncArrayList<PackInfo*> packs;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			static Text::CString ShapeTypeGetName(UInt32 shapeType);
			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			SHPFileAnalyse(IO::StreamData *fd);
			virtual ~SHPFileAnalyse();

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
