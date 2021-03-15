//require setupapi.lib
#ifndef _SM_MEDIA_KSRENDERER
#define _SM_MEDIA_KSRENDERER
#include "Sync/Event.h"
#include "Media/IAudioRenderer.h"

namespace Media
{
	class KSRenderer : public IAudioRenderer
	{
	private:
		void *pFilter;
		void *pEnumerator;
		void *pPin;

		Bool playing;
		Bool stopPlay;
		Media::RefClock *clk;
		Bool threadInit;
		Sync::Event *playEvt;
		Media::IAudioSource *audsrc;
		EndNotifier endHdlr;
		void *endHdlrObj;

		UInt32 buffTime;

		static void FillFormat(void *formatEx, Media::AudioFormat *fmt);
		static UInt32 __stdcall PlayThread(void *obj);
	public:
		static UOSInt GetDeviceCount();
		static UTF8Char *GetDeviceName(UTF8Char *buff, UOSInt devIndex);
		static OSInt GetDeviceId(const UTF8Char *devName);

		KSRenderer(OSInt devId);
		virtual ~KSRenderer();

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
