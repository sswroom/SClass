#ifndef _SM_MEDIA_IMEDIAPLAYER
#define _SM_MEDIA_IMEDIAPLAYER
#include "Media/IPBControl.h"
#include "Media/MediaFile.h"

namespace Media
{
	class IMediaPlayer : public IPBControl
	{
	public:
		typedef void (__stdcall *PBEndHandler)(void *userObj);
	public:
		virtual ~IMediaPlayer(){};

		virtual void SetEndHandler(PBEndHandler hdlr, void *userObj) = 0;
		virtual Bool LoadMedia(Media::MediaFile *file) = 0;
	};
}
#endif
