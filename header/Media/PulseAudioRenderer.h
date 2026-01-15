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
			UIntOS count;
			UInt32 state;

			UnsafeArrayOpt<UTF8Char> sbuff;
			UIntOS devNo;
		};
	private:
		struct ClassData;
		NN<ClassData> clsData;
		Optional<Media::AudioSource> audsrc;
		Optional<Media::AudioSource> resampler;
		UInt32 resampleFreq;
		Optional<Text::String> devName;

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
		static UInt32 GetCurrTime(void *stream);
		static Bool GetDeviceInfo(NN<DeviceInfo> devInfo);
	public:
		static UIntOS GetDeviceCount();
		static UnsafeArrayOpt<UTF8Char> GetDeviceName(UnsafeArray<UTF8Char> buff, UIntOS devNo);
		
		void OnEvent();

		PulseAudioRenderer(UnsafeArrayOpt<const UTF8Char> devName, Text::CStringNN appName);
		virtual ~PulseAudioRenderer();

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

		void WriteStream(UIntOS length);
	};
}
#endif
