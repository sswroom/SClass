#ifndef _SM_MEDIA_OPENSLESRENDERER
#define _SM_MEDIA_OPENSLESRENDERER

#include "Media/AudioRenderer.h"

namespace Media
{
	class OpenSLESRenderer : public AudioRenderer
	{
	private:
		Media::AudioSource *audsrc;
		const UTF8Char *devName;
		Bool playing;
		Bool threadInit;
		Bool stopPlay;
		void *hand;
		Sync::Event *playEvt;
		Media::RefClock *clk;
		EndNotifier endHdlr;
		void *endHdlrObj;

		UInt32 buffTime;

		static void __stdcall WaveEvents(void *hwo, UInt32 uMsg, UInt32 *dwInstance, UInt32 *dwParam1, UInt32 *dwParam2);
		static UInt32 __stdcall PlayThread(void *obj);
		static Int32 GetCurrTime(void *hand);
	public:
		static Int32 GetDeviceCount();
		static UTF8Char *GetDeviceName(UTF8Char *buff, Int32 devNo);
		
		void OnEvent();

		OpenSLESRenderer(const UTF8Char *devName);
		virtual ~OpenSLESRenderer();

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
