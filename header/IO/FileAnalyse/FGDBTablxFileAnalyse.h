#ifndef _SM_IO_FILEANALYSE_FGDBTABLXFILEANALYSE
#define _SM_IO_FILEANALYSE_FGDBTABLXFILEANALYSE
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Map/ESRI/FileGDBUtil.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class FGDBTablxFileAnalyse : public IO::FileAnalyse::FileAnalyser
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
			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<TagInfo> tags;
			Optional<Map::ESRI::FileGDBTableInfo> tableInfo;
			Math::ArcGISPRJParser prjParser;
			UInt32 maxRowSize;

			Bool pauseParsing;
			Sync::Thread thread;

			static void __stdcall ParseThread(NN<Sync::Thread> thread);
		public:
			FGDBTablxFileAnalyse(NN<IO::StreamData> fd);
			virtual ~FGDBTablxFileAnalyse();

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
