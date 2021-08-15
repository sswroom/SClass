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
		Parser::ParserList *parsers;
		UInt32 storeTime;
		Media::MediaFile *currFile;
	protected:
		Media::MediaPlayer *player;
		Media::IPBControl *currPBC;

	protected:
		virtual void OnMediaOpened();
		virtual void OnMediaClosed();

		void SetPlayer(Media::MediaPlayer *player);
	public:
		MediaPlayerInterface(Parser::ParserList *parsers);
		virtual ~MediaPlayerInterface();

		Bool OpenFile(const UTF8Char *fileName);
		Bool OpenVideo(Media::MediaFile *mf);
		void CloseFile();

		Media::MediaFile *GetOpenedFile();

		void PBStart();
		void PBStop();
		void PBPause();
	};
}
#endif
