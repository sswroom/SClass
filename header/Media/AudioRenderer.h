#ifndef _SM_MEDIA_AUDIORENDERER
#define _SM_MEDIA_AUDIORENDERER
#include "AnyType.h"
#include "Media/AudioSource.h"
#include "Media/RefClock.h"

namespace Media
{
	class AudioRenderer
	{
	public:
		typedef void (CALLBACKFUNC EndNotifier)(AnyType userObj);

	public:
		virtual ~AudioRenderer() {};

		virtual Bool IsError() = 0;
		virtual Bool BindAudio(Optional<Media::AudioSource> audsrc) = 0;
		virtual void AudioInit(Optional<Media::RefClock> clk) = 0;
		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual Bool IsPlaying() = 0;
		virtual void SetEndNotify(EndNotifier hdlr, AnyType userObj) = 0;
		virtual void SetBufferTime(UInt32 ms) = 0;

		virtual Int32 GetDeviceVolume() = 0;
		virtual void SetDeviceVolume(Int32 volume) = 0;
	};
}
#endif
