//require setupapi.lib
#ifndef _SM_MEDIA_KSRENDERER
#define _SM_MEDIA_KSRENDERER
#include "Sync/Event.h"
#include "Media/AudioRenderer.h"

namespace Media
{
	class KSRenderer : public AudioRenderer
	{
	private:
		void *pFilter;
		void *pEnumerator;
		void *pPin;

		Bool playing;
		Bool stopPlay;
		Optional<Media::RefClock> clk;
		Bool threadInit;
		Optional<Sync::Event> playEvt;
		Optional<Media::AudioSource> audsrc;
		EndNotifier endHdlr;
		AnyType endHdlrObj;

		UInt32 buffTime;

		static void FillFormat(void *formatEx, Media::AudioFormat *fmt);
		static UInt32 __stdcall PlayThread(AnyType obj);
	public:
		static UOSInt GetDeviceCount();
		static UnsafeArray<UTF8Char> GetDeviceName(UnsafeArray<UTF8Char> buff, UOSInt devIndex);
		static OSInt GetDeviceId(UnsafeArray<const UTF8Char> devName);

		KSRenderer(OSInt devId);
		virtual ~KSRenderer();

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
