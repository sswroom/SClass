//need Winmm.lib
#ifndef _SM_MEDIA_WAVEOUTRENDERER
#define _SM_MEDIA_WAVEOUTRENDERER

#include "Media/AudioRenderer.h"

namespace Media
{
	class WaveOutRenderer : public AudioRenderer
	{
	private:
		Optional<Media::AudioSource> audsrc;
		OSInt devId;
		Bool playing;
		Bool threadInit;
		Bool stopPlay;
		Bool buffEmpty[4];
		void *hwo;
		Sync::Event *playEvt;
		Optional<Media::RefClock> clk;
		EndNotifier endHdlr;
		AnyType endHdlrObj;

		UInt32 buffTime;

		static void __stdcall WaveEvents(void *hwo, UInt32 uMsg, UInt32 *dwInstance, UInt32 *dwParam1, UInt32 *dwParam2);
		static UInt32 __stdcall PlayThread(AnyType obj);
		static UInt32 GetMSFromTime(void *mmTime, NN<const AudioFormat> fmt);
		static Data::Duration GetDurFromTime(void *mmTime, NN<const AudioFormat> fmt);
	public:
		static UOSInt GetDeviceCount();
		static UnsafeArrayOpt<UTF8Char> GetDeviceName(UnsafeArray<UTF8Char> buff, UOSInt devNo);

		WaveOutRenderer(UnsafeArrayOpt<const UTF8Char> devName);
		WaveOutRenderer(Int32 devId);
		virtual ~WaveOutRenderer();

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
