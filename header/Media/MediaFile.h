#ifndef _SM_MEDIA_MEDIAFILE
#define _SM_MEDIA_MEDIAFILE
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "IO/ParsedObject.h"
#include "Media/IMediaSource.h"
#include "Media/ChapterInfo.h"

namespace Media
{
	class MediaFile : public IO::ParsedObject
	{
	private:
		Data::ArrayList<Media::IMediaSource*> *sources;
		Data::ArrayListInt32 *keepSources;
		Data::ArrayListInt32 *syncTime;
		Bool releaseChapter;
		Media::ChapterInfo *chapters;

	public:
		MediaFile(Text::String *name);
		MediaFile(Text::CString name);
		virtual ~MediaFile();

		virtual IO::ParserType GetParserType();

		virtual UOSInt AddSource(Media::IMediaSource *src, Int32 syncTime); //-1 = fail
		virtual Media::IMediaSource *GetStream(UOSInt index, Int32 *syncTime);
		virtual void KeepStream(UOSInt index, Bool toKeep);

		Media::ChapterInfo *GetChapterInfo();
		void SetChapterInfo(Media::ChapterInfo *chapters, Bool releaseChapter);

		Bool TrimFile(UInt32 trimTimeStart, Int32 trimTimeEnd);
	};
}
#endif
