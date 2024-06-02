#ifndef _SM_MEDIA_ALSARENDERER
#define _SM_MEDIA_ALSARENDERER
#include "Media/IAudioRenderer.h"
#include "Sync/Thread.h"

namespace Media
{
	class ALSARenderer : public IAudioRenderer
	{
	private:
		Media::IAudioSource *audsrc;
		Media::IAudioSource *resampler;
		UInt32 resampleFreq;
		UnsafeArrayOpt<const UTF8Char> devName;
		void *hand;
		Media::RefClock *clk;
		EndNotifier endHdlr;
		AnyType endHdlrObj;
		Bool dataConv;
		UInt16 dataBits;
		UInt16 dataNChannel;
		Bool nonBlock;
		Sync::Thread thread;

		UInt32 buffTime;

		static void __stdcall WaveEvents(void *hwo, UInt32 uMsg, UInt32 *dwInstance, UInt32 *dwParam1, UInt32 *dwParam2);
		static void __stdcall PlayThread(NN<Sync::Thread> thread);
		static Data::Duration GetCurrTime(void *hand);

		Bool SetHWParams(Media::IAudioSource *audsrc, void *h);
	public:
		static UOSInt GetDeviceCount();
		static UnsafeArrayOpt<UTF8Char> GetDeviceName(UnsafeArray<UTF8Char> buff, UOSInt devNo);
		
		void OnEvent();

		ALSARenderer(UnsafeArrayOpt<const UTF8Char> devName);
		virtual ~ALSARenderer();

		virtual Bool IsError();
		virtual Bool BindAudio(Media::IAudioSource *audsrc);
		virtual void AudioInit(Media::RefClock *clk);
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
