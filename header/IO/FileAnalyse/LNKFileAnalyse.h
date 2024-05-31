#ifndef _SM_IO_FILEANALYSE_LNKFILEANALYSE
#define _SM_IO_FILEANALYSE_LNKFILEANALYSE
#include "Data/SyncArrayListNN.h"
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
			Data::SyncArrayListNN<TagInfo> tags;
			
			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			LNKFileAnalyse(NN<IO::StreamData> fd);
			virtual ~LNKFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			static Text::CStringNN TagTypeGetName(TagType tagType);
			static Text::CStringNN ShowWindowGetName(UInt32 showWindow);
			static Text::CStringNN DriveTypeGetName(UInt32 driveType);
			static Text::CStringNN FontFamilyGetName(UInt32 fontFamily);
		};
	}
}
#endif
