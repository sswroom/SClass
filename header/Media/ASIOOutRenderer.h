#ifndef _SM_MEDIA_ASIOOUTRENDERER
#define _SM_MEDIA_ASIOOUTRENDERER
#include "Sync/Event.h"
#include "Media/IAudioRenderer.h"

namespace Media
{
	class ASIOOutRenderer : public IAudioRenderer
	{
	private:
		void *asiodrv;
		const WChar *drvName;
		void *bufferInfos;
		Int32 *sampleTypes;
		UInt32 bufferSize;
		Bool bufferCreated;
		Bool playing;
		Bool toStop;
		Media::RefClock *clk;
		Int32 debug;
		Bool threadInit;

		Media::IAudioSource *audSrc;
		Int32 bufferIndex;
		Int64 bufferOfst;
		Bool bufferFilled;
		Sync::Event *bufferEvt;

		EndNotifier endHdlr;
		AnyType endHdlrObj;

		UInt32 buffTime;

		static Int32 GetDeviceIndex(const UTF8Char *buff);
		static UInt32 __stdcall PlayThread(void *obj);
		void InitDevice(UInt32 devId);

	public:
		static UOSInt GetDeviceCount();
		static UTF8Char *GetDeviceName(UTF8Char *buff, UOSInt devNo);

		Int64 SwitchBuffer(Int32 index);

		ASIOOutRenderer(const UTF8Char *devName);
		ASIOOutRenderer(UInt32 devId);
		virtual ~ASIOOutRenderer();

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

		void Test();
	};
}
#endif
