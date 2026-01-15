#ifndef _SM_MEDIA_CHAPTERINFO
#define _SM_MEDIA_CHAPTERINFO
#include "Data/ArrayListObj.hpp"
#include "Data/ArrayListStringNN.h"
#include "Data/ArrayListUInt32.h"
#include "Data/Duration.h"
#include "Text/String.h"

namespace Media
{
	class ChapterInfo
	{
	private:
		Data::ArrayListUInt32 chapterTimes;
		Data::ArrayListStringNN chapterNames;
		Data::ArrayListObj<Optional<Text::String>> chapterArtists;

	public:
		ChapterInfo();
		~ChapterInfo();

		void AddChapter(UInt32 chapterTime, Text::CStringNN chapterName, Text::CString chapterArtist);
		UIntOS GetChapterIndex(Data::Duration currTime);
		UInt32 GetChapterTime(UIntOS index);
		UIntOS GetChapterCnt();
		Optional<Text::String> GetChapterName(UIntOS index);
		Optional<Text::String> GetChapterArtist(UIntOS index);
	};
}
#endif
