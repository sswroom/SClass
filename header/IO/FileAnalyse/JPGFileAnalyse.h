#ifndef _SM_IO_FILEANALYSE_JPGFILEANALYSE
#define _SM_IO_FILEANALYSE_JPGFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
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
			IO::IStreamData *fd;
			Data::SyncArrayList<JPGTag*> *tags;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			static const UTF8Char *GetTagName(UInt8 tagType);
			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			JPGFileAnalyse(IO::IStreamData *fd);
			virtual ~JPGFileAnalyse();

			virtual const UTF8Char *GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(const UTF8Char *outputFile);
		};
	}
}
#endif
