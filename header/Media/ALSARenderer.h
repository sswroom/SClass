#ifndef _SM_MEDIA_ALSARENDERER
#define _SM_MEDIA_ALSARENDERER

#include "Media/IAudioRenderer.h"

namespace Media
{
	class ALSARenderer : public IAudioRenderer
	{
	private:
		Media::IAudioSource *audsrc;
		const UTF8Char *devName;
		Bool playing;
		Bool threadInit;
		Bool stopPlay;
		void *hand;
		Sync::Event *playEvt;
		Media::RefClock *clk;
		EndNotifier endHdlr;
		void *endHdlrObj;
		Bool dataConv;
		UInt16 dataBits;
		Bool nonBlock;

		UInt32 buffTime;

		static void __stdcall WaveEvents(void *hwo, UInt32 uMsg, UInt32 *dwInstance, UInt32 *dwParam1, UInt32 *dwParam2);
		static UInt32 __stdcall PlayThread(void *obj);
		static UInt32 GetCurrTime(void *hand);

		Bool SetHWParams(Media::IAudioSource *audsrc, void *h);
	public:
		static UOSInt GetDeviceCount();
		static UTF8Char *GetDeviceName(UTF8Char *buff, UOSInt devNo);
		
		void OnEvent();

		ALSARenderer(const UTF8Char *devName);
		virtual ~ALSARenderer();

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
