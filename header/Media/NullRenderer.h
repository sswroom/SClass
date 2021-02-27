#ifndef _SM_MEDIA_NULLRENDERER
#define _SM_MEDIA_NULLRENDERER
#include "Media/IAudioRenderer.h"

namespace Media
{
	class NullRenderer : public IAudioRenderer
	{
	private:
		Media::IAudioSource *audsrc;
		Bool playing;
		Bool threadInit;
		Bool stopPlay;
		Sync::Event *playEvt;
		Media::RefClock *clk;
		EndNotifier endHdlr;
		void *endHdlrObj;
		UInt32 buffTime;
		Int64 sampleCnt;

		static UInt32 __stdcall PlayThread(void *obj);
	public:
		NullRenderer();
		virtual ~NullRenderer();

		virtual Bool IsError();
		virtual Bool BindAudio(Media::IAudioSource *audsrc);
		virtual void AudioInit(Media::RefClock *clk);
		virtual void Start();
		virtual void Stop();
		virtual Bool IsPlaying();
		virtual void SetEndNotify(EndNotifier endHdlr, void *endHdlrObj);
		virtual void SetBufferTime(UInt32 ms);

		virtual Int32 GetDeviceVolume();
		virtual void SetDeviceVolume(Int32 volume);
		Int64 GetSampleCnt();
	};
}
#endif
