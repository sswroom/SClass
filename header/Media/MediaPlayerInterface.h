#ifndef _SM_MEDIA_MEDIAPLAYERINTERFACE
#define _SM_MEDIA_MEDIAPLAYERINTERFACE
#include "Media/MediaFile.h"
#include "Parser/ParserList.h"

namespace Media
{
	class MediaPlayerInterface
	{
	private:
		Parser::ParserList *parsers;

	protected:
		virtual void OnMediaOpened();
		virtual void OnMediaClosed();
	public:
		MediaPlayerInterface(Parser::ParserList *parsers);
		virtual ~MediaPlayerInterface();

		Bool OpenFile(const UTF8Char *fileName);
		Bool OpenVideo(Media::MediaFile *mf);
	};
}
#endif
