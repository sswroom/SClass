#ifndef _SM_MEDIA_CHAPTERINFO
#define _SM_MEDIA_CHAPTERINFO
#include "Data/ArrayListUInt32.h"

namespace Media
{
	class ChapterInfo
	{
	private:
		Data::ArrayListUInt32 *chapterTimes;
		Data::ArrayList<const UTF8Char *> *chapterNames;
		Data::ArrayList<const UTF8Char *> *chapterArtists;

	public:
		ChapterInfo();
		~ChapterInfo();

		void AddChapter(UInt32 chapterTime, const UTF8Char *chapterName, const UTF8Char *chapterArtist);
		UOSInt GetChapterIndex(UInt32 currTime);
		UInt32 GetChapterTime(UOSInt index);
		UOSInt GetChapterCnt();
		const UTF8Char *GetChapterName(UOSInt index);
		const UTF8Char *GetChapterArtist(UOSInt index);
	};
}
#endif
