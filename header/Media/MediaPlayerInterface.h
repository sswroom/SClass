#ifndef _SM_MEDIA_MEDIAPLAYERINTERFACE
#define _SM_MEDIA_MEDIAPLAYERINTERFACE
#include "Media/MediaFile.h"
#include "Media/MediaPlayer.h"
#include "Parser/ParserList.h"

namespace Media
{
	class MediaPlayerInterface
	{
	private:
		NN<Parser::ParserList> parsers;
		Data::Duration storeTime;
		Optional<Media::MediaFile> currFile;
	protected:
		Optional<Media::MediaPlayer> player;
		Optional<Media::PBControl> currPBC;

	protected:
		virtual void OnMediaOpened();
		virtual void OnMediaClosed();

		void SetPlayer(Optional<Media::MediaPlayer> player);
	public:
		MediaPlayerInterface(NN<Parser::ParserList> parsers);
		virtual ~MediaPlayerInterface();

		Bool OpenFile(Text::CStringNN fileName, IO::ParserType targetType);
		Bool OpenVideo(NN<Media::MediaFile> mf);
		void CloseFile();

		Optional<Media::MediaFile> GetOpenedFile();
		Optional<Media::VideoRenderer> GetVideoRenderer();

		void PBStart();
		void PBStop();
		void PBPause();
		void PBPrevChapter();
		void PBNextChapter();
		void PBDecAVOfst();
		void PBIncAVOfst();
		void PBJumpOfst(Int32 ofst);
		void PBJumpToTime(UInt32 time);
	};
}
#endif
