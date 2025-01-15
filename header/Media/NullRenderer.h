#ifndef _SM_MEDIA_NULLRENDERER
#define _SM_MEDIA_NULLRENDERER
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "Media/IAudioRenderer.h"

namespace Media
{
	class NullRenderer : public IAudioRenderer
	{
	private:
		Optional<Media::IAudioSource> audsrc;
		Bool playing;
		Bool threadInit;
		Bool stopPlay;
		Sync::Event playEvt;
		Optional<Media::RefClock> clk;
		Data::CallbackStorage<EndNotifier> endHdlr;
		UInt32 buffTime;
		UInt64 sampleCnt;

		static UInt32 __stdcall PlayThread(AnyType obj);
	public:
		NullRenderer();
		virtual ~NullRenderer();

		virtual Bool IsError();
		virtual Bool BindAudio(Optional<Media::IAudioSource> audsrc);
		virtual void AudioInit(Optional<Media::RefClock> clk);
		virtual void Start();
		virtual void Stop();
		virtual Bool IsPlaying();
		virtual void SetEndNotify(EndNotifier endHdlr, AnyType endHdlrObj);
		virtual void SetBufferTime(UInt32 ms);

		virtual Int32 GetDeviceVolume();
		virtual void SetDeviceVolume(Int32 volume);
		UInt64 GetSampleCnt();
	};
}
#endif
