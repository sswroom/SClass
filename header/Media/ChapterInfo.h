#ifndef _SM_MEDIA_CHAPTERINFO
#define _SM_MEDIA_CHAPTERINFO
#include "Data/ArrayListNN.h"
#include "Data/ArrayListUInt32.h"
#include "Data/Duration.h"
#include "Text/String.h"

namespace Media
{
	class ChapterInfo
	{
	private:
		Data::ArrayListUInt32 chapterTimes;
		Data::ArrayListNN<Text::String> chapterNames;
		Data::ArrayList<Text::String *> chapterArtists;

	public:
		ChapterInfo();
		~ChapterInfo();

		void AddChapter(UInt32 chapterTime, Text::CString chapterName, Text::CString chapterArtist);
		UOSInt GetChapterIndex(Data::Duration currTime);
		UInt32 GetChapterTime(UOSInt index);
		UOSInt GetChapterCnt();
		Text::String *GetChapterName(UOSInt index);
		Text::String *GetChapterArtist(UOSInt index);
	};
}
#endif
