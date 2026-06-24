#ifndef _SM_MEDIA_OPENSLESRENDERER
#define _SM_MEDIA_OPENSLESRENDERER

#include "Media/AudioRenderer.h"

namespace Media
{
	class OpenSLESRenderer : public AudioRenderer
	{
	private:
		Optional<Media::AudioSource> audsrc;
		UnsafeArray<const UTF8Char> devName;
		Bool playing;
		Bool threadInit;
		Bool stopPlay;
		void *hand;
		Sync::Event *playEvt;
		Optional<Media::RefClock> clk;
		EndNotifier endHdlr;
		AnyType endHdlrObj;

		UInt32 buffTime;

		static void __stdcall WaveEvents(void *hwo, UInt32 uMsg, UInt32 *dwInstance, UInt32 *dwParam1, UInt32 *dwParam2);
		static UInt32 __stdcall PlayThread(AnyType obj);
		static Int32 GetCurrTime(void *hand);
	public:
		static Int32 GetDeviceCount();
		static UnsafeArrayOpt<UTF8Char> GetDeviceName(UnsafeArray<UTF8Char> buff, Int32 devNo);
		
		void OnEvent();

		OpenSLESRenderer(UnsafeArrayOpt<const UTF8Char> devName);
		virtual ~OpenSLESRenderer();

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
