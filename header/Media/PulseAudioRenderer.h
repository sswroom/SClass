#ifndef _SM_MEDIA_PULSEAUDIORENDERER
#define _SM_MEDIA_PULSEAUDIORENDERER
#include "Media/AudioRenderer.h"
#include "Sync/Thread.h"

namespace Media
{
	class PulseAudioRenderer : public AudioRenderer
	{
	public:
		struct DeviceInfo
		{
			UOSInt count;
			UInt32 state;

			UnsafeArrayOpt<UTF8Char> sbuff;
			UOSInt devNo;
		};
	private:
		Media::AudioSource *audsrc;
		Media::AudioSource *resampler;
		UInt32 resampleFreq;
		Optional<Text::String> devName;
		void *hand;
		Media::RefClock *clk;
		EndNotifier endHdlr;
		void *endHdlrObj;
		Bool dataConv;
		UInt16 dataBits;
		UInt16 dataNChannel;
		Bool nonBlock;
		Sync::Thread thread;

		UInt32 buffTime;

		static void __stdcall WaveEvents(void *hwo, UInt32 uMsg, UInt32 *dwInstance, UInt32 *dwParam1, UInt32 *dwParam2);
		static void __stdcall PlayThread(NN<Sync::Thread> thread);
		static UInt32 GetCurrTime(void *hand);
		static Bool GetDeviceInfo(NN<DeviceInfo> devInfo);
	public:
		static UOSInt GetDeviceCount();
		static UnsafeArrayOpt<UTF8Char> GetDeviceName(UnsafeArray<UTF8Char> buff, UOSInt devNo);
		
		void OnEvent();

		PulseAudioRenderer(const UTF8Char *devName);
		virtual ~PulseAudioRenderer();

		virtual Bool IsError();
		virtual Bool BindAudio(Media::AudioSource *audsrc);
		virtual void AudioInit(Media::RefClock *clk);
		virtual void Start();
		virtual void Stop();
		virtual Bool IsPlaying();
		virtual void SetEndNotify(EndNotifier endHdlr, void *endHdlrObj);
		virtual void SetBufferTime(UInt32 ms);

		virtual Int32 GetDeviceVolume();
		virtual void SetDeviceVolume(Int32 volume);
	};
}
#endif
