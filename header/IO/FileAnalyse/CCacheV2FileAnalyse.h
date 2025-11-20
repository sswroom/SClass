#ifndef _SM_IO_FILEANALYSE_CCACHEV2FILEANALYSE
#define _SM_IO_FILEANALYSE_CCACHEV2FILEANALYSE
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class CCacheV2FileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			enum class TagType
			{
				Header,
				UserHeader,
				Tile
			};

			struct TagInfo
			{
				TagType tagType;
				UInt64 ofst;
				UOSInt size;
				UOSInt row;
				UOSInt col;
			};
		private:
			IO::StreamData *fd;
			Data::SyncArrayListNN<TagInfo> tags;
			UInt32 maxRowSize;

			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			CCacheV2FileAnalyse(NN<IO::StreamData> fd);
			virtual ~CCacheV2FileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);

			static Text::CStringNN TagTypeGetName(TagType tagType);
		};
	}
}
#endif
