#ifndef _SM_MEDIA_IAUDIORENDERER
#define _SM_MEDIA_IAUDIORENDERER
#include "AnyType.h"
#include "Media/IAudioSource.h"
#include "Media/RefClock.h"

namespace Media
{
	class IAudioRenderer
	{
	public:
		typedef void (__stdcall *EndNotifier)(AnyType userObj);

	public:
		virtual ~IAudioRenderer() {};

		virtual Bool IsError() = 0;
		virtual Bool BindAudio(Media::IAudioSource *audsrc) = 0;
		virtual void AudioInit(Media::RefClock *clk) = 0;
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
