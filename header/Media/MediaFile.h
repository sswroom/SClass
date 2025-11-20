#ifndef _SM_MEDIA_MEDIAFILE
#define _SM_MEDIA_MEDIAFILE
#include "Data/ArrayList.hpp"
#include "Data/ArrayListInt32.h"
#include "IO/ParsedObject.h"
#include "Media/MediaSource.h"
#include "Media/ChapterInfo.h"

namespace Media
{
	class MediaFile : public IO::ParsedObject
	{
	private:
		Data::ArrayListNN<Media::MediaSource> sources;
		Data::ArrayListInt32 keepSources;
		Data::ArrayListInt32 syncTime;
		Bool releaseChapter;
		Media::ChapterInfo *chapters;

	public:
		MediaFile(NN<Text::String> name);
		MediaFile(Text::CStringNN name);
		virtual ~MediaFile();

		virtual IO::ParserType GetParserType() const;

		virtual UOSInt AddSource(NN<Media::MediaSource> src, Int32 syncTime); //-1 = fail
		virtual Optional<Media::MediaSource> GetStream(UOSInt index, OptOut<Int32> syncTime);
		virtual void KeepStream(UOSInt index, Bool toKeep);

		Media::ChapterInfo *GetChapterInfo() const;
		void SetChapterInfo(Media::ChapterInfo *chapters, Bool releaseChapter);

		Bool TrimFile(UInt32 trimTimeStart, Int32 trimTimeEnd);
	};
}
#endif
