//need Winmm.lib
#ifndef _SM_MEDIA_WAVEOUTRENDERER
#define _SM_MEDIA_WAVEOUTRENDERER

#include "Media/IAudioRenderer.h"

namespace Media
{
	class WaveOutRenderer : public IAudioRenderer
	{
	private:
		Media::IAudioSource *audsrc;
		OSInt devId;
		Bool playing;
		Bool threadInit;
		Bool stopPlay;
		Bool buffEmpty[4];
		void *hwo;
		Sync::Event *playEvt;
		Media::RefClock *clk;
		EndNotifier endHdlr;
		void *endHdlrObj;

		UInt32 buffTime;

		static void __stdcall WaveEvents(void *hwo, UInt32 uMsg, UInt32 *dwInstance, UInt32 *dwParam1, UInt32 *dwParam2);
		static UInt32 __stdcall PlayThread(void *obj);
		static Int32 GetMSFromTime(void *mmTime, AudioFormat *fmt);
	public:
		static UOSInt GetDeviceCount();
		static UTF8Char *GetDeviceName(UTF8Char *buff, UOSInt devNo);

		WaveOutRenderer(const UTF8Char *devName);
		WaveOutRenderer(Int32 devId);
		virtual ~WaveOutRenderer();

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
	};
}
#endif
