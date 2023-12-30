#ifndef _SM_IO_FILEANALYSE_LNKFILEANALYSE
#define _SM_IO_FILEANALYSE_LNKFILEANALYSE
#include "Data/SyncArrayList.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class LNKFileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			enum class TagType
			{
				ShellLinkHeader,
				LinkTargetIDList,
				LinkInfo,
				NameString,
				RelativePath,
				WorkingDir,
				CommandLineArguments,
				IconLocation,
				ExtraData
			};

			struct TagInfo
			{
				TagType tagType;
				UInt64 ofst;
				UOSInt size;
			};
		private:
			IO::StreamData *fd;
			Data::SyncArrayList<TagInfo*> tags;
			
			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NotNullPtr<Sync::Thread> thread);
		public:
			LNKFileAnalyse(NotNullPtr<IO::StreamData> fd);
			virtual ~LNKFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			static Text::CString TagTypeGetName(TagType tagType);
			static Text::CString ShowWindowGetName(UInt32 showWindow);
			static Text::CString DriveTypeGetName(UInt32 driveType);
			static Text::CString FontFamilyGetName(UInt32 fontFamily);
		};
	}
}
#endif
