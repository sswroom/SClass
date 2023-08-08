#ifndef _SM_IO_FILEANALYSE_DWGFILEANALYSE
#define _SM_IO_FILEANALYSE_DWGFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class DWGFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		public:
			enum class PackType
			{
				Unknown,
				FileHeaderV1,
				PreviewImage,
				HeaderVariables,
				ClassSection,
				ObjectMap,
				UnknownTable,
				Measurement
			};

			typedef struct
			{
				UInt64 fileOfst;
				UInt64 packSize;
				PackType packType;
			} PackInfo;
		private:
			IO::StreamData *fd;
			Data::SyncArrayList<PackInfo*> packs;

			UInt32 fileVer;

			Bool pauseParsing;
			Bool threadRunning;
			Bool threadToStop;
			Bool threadStarted;

			static UInt32 __stdcall ParseThread(void *userObj);
		public:
			DWGFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~DWGFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			static Text::CString PackTypeGetName(PackType packType);
		};
	}
}
#endif
