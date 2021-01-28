#ifndef _SM_IO_FILEANALYSE_JPGFILEANALYSE
#define _SM_IO_FILEANALYSE_JPGFILEANALYSE
#include "Data/ArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
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
				Int64 ofst;
				OSInt size;
			} JPGTag;
		private:
			IO::IStreamData *fd;
			Data::ArrayList<JPGTag*> *tags;
			Sync::Mutex *tagsMut;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			static const UTF8Char *GetTagName(UInt8 tagType);
			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			JPGFileAnalyse(IO::IStreamData *fd);
			virtual ~JPGFileAnalyse();

			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF *sb);
			virtual Bool GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(const UTF8Char *outputFile);
		};
	}
}
#endif
