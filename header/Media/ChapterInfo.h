#ifndef _SM_MEDIA_CHAPTERINFO
#define _SM_MEDIA_CHAPTERINFO
#include "Data/ArrayListUInt32.h"
#include "Text/String.h"

namespace Media
{
	class ChapterInfo
	{
	private:
		Data::ArrayListUInt32 *chapterTimes;
		Data::ArrayList<Text::String *> *chapterNames;
		Data::ArrayList<Text::String *> *chapterArtists;

	public:
		ChapterInfo();
		~ChapterInfo();

		void AddChapter(UInt32 chapterTime, const UTF8Char *chapterName, const UTF8Char *chapterArtist);
		UOSInt GetChapterIndex(UInt32 currTime);
		UInt32 GetChapterTime(UOSInt index);
		UOSInt GetChapterCnt();
		Text::String *GetChapterName(UOSInt index);
		Text::String *GetChapterArtist(UOSInt index);
	};
}
#endif
