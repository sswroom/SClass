#ifndef _SM_MEDIA_IMEDIAPLAYER
#define _SM_MEDIA_IMEDIAPLAYER
#include "AnyType.h"
#include "Media/IPBControl.h"
#include "Media/MediaFile.h"

namespace Media
{
	class IMediaPlayer : public IPBControl
	{
	public:
		typedef void (__stdcall *PBEndHandler)(AnyType userObj);
	public:
		virtual ~IMediaPlayer(){};

		virtual void SetEndHandler(PBEndHandler hdlr, AnyType userObj) = 0;
		virtual Bool LoadMedia(Optional<Media::MediaFile> file) = 0;
	};
}
#endif
