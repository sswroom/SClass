#ifndef _SM_MEDIA_ALSARENDERER
#define _SM_MEDIA_ALSARENDERER
#include "Media/AudioRenderer.h"
#include "Sync/Thread.h"

namespace Media
{
	class ALSARenderer : public AudioRenderer
	{
	private:
		Optional<Media::AudioSource> audsrc;
		Optional<Media::AudioSource> resampler;
		UInt32 resampleFreq;
		UnsafeArrayOpt<const UTF8Char> devName;
		void *hand;
		Optional<Media::RefClock> clk;
		EndNotifier endHdlr;
		AnyType endHdlrObj;
		Bool dataConv;
		UInt16 dataBits;
		UInt16 dataNChannel;
		Bool nonBlock;
		Sync::Thread thread;

		UInt32 buffTime;

		static void __stdcall PlayThread(NN<Sync::Thread> thread);
		static Data::Duration GetCurrTime(void *hand);

		Bool SetHWParams(NN<Media::AudioSource> audsrc, void *h);
	public:
		static UOSInt GetDeviceCount();
		static UnsafeArrayOpt<UTF8Char> GetDeviceName(UnsafeArray<UTF8Char> buff, UOSInt devNo);
		
		void OnEvent();

		ALSARenderer(UnsafeArrayOpt<const UTF8Char> devName);
		virtual ~ALSARenderer();

		virtual Bool IsError();
		virtual Bool BindAudio(Optional<Media::AudioSource> audsrc);
		virtual void AudioInit(Optional<Media::RefClock> clk);
		virtual void Start();
		virtual void Stop();
		virtual Bool IsPlaying();
		virtual void SetEndNotify(EndNotifier endHdlr, AnyType endHdlrObj);
		virtual void SetBufferTime(UInt32 ms);

		virtual Int32 GetDeviceVolume();
		virtual void SetDeviceVolume(Int32 volume);
	};
}
#endif
