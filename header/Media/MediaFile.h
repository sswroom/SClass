#ifndef _SM_MEDIA_MEDIAFILE
#define _SM_MEDIA_MEDIAFILE
#include "Data/ArrayListNN.hpp"
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
		Optional<Media::ChapterInfo> chapters;

	public:
		MediaFile(NN<Text::String> name);
		MediaFile(Text::CStringNN name);
		virtual ~MediaFile();

		virtual IO::ParserType GetParserType() const;

		virtual UIntOS AddSource(NN<Media::MediaSource> src, Int32 syncTime); //-1 = fail
		virtual Optional<Media::MediaSource> GetStream(UIntOS index, OptOut<Int32> syncTime);
		virtual void KeepStream(UIntOS index, Bool toKeep);

		Optional<Media::ChapterInfo> GetChapterInfo() const;
		void SetChapterInfo(Optional<Media::ChapterInfo> chapters, Bool releaseChapter);

		Bool TrimFile(UInt32 trimTimeStart, Int32 trimTimeEnd);
	};
}
#endif
