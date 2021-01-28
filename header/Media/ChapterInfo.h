#ifndef _SM_MEDIA_CHAPTERINFO
#define _SM_MEDIA_CHAPTERINFO
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"

namespace Media
{
	class ChapterInfo
	{
	private:
		Data::ArrayListInt32 *chapterTimes;
		Data::ArrayList<const UTF8Char *> *chapterNames;
		Data::ArrayList<const UTF8Char *> *chapterArtists;

	public:
		ChapterInfo();
		~ChapterInfo();

		void AddChapter(Int32 chapterTime, const UTF8Char *chapterName, const UTF8Char *chapterArtist);
		OSInt GetChapterIndex(Int32 currTime);
		Int32 GetChapterTime(OSInt index);
		OSInt GetChapterCnt();
		const UTF8Char *GetChapterName(OSInt index);
		const UTF8Char *GetChapterArtist(OSInt index);
	};
};
#endif
