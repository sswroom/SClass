#ifndef _SM_IO_FILEANALYSE_FGDBFILEANALYSE
#define _SM_IO_FILEANALYSE_FGDBFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Map/ESRI/FileGDBUtil.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class FGDBFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			enum class TagType
			{
				Header,
				Field,
				Row,
				FreeSpace
			};

			struct TagInfo
			{
				TagType tagType;
				UInt64 ofst;
				UOSInt size;
			};
		private:
			IO::IStreamData *fd;
			Data::SyncArrayList<TagInfo*> tags;
			Map::ESRI::FileGDBTableInfo *tableInfo;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			FGDBFileAnalyse(IO::IStreamData *fd);
			virtual ~FGDBFileAnalyse();

			virtual Text::CString GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CString outputFile);

			static Text::CString TagTypeGetName(TagType tagType);
		};
	}
}
#endif
