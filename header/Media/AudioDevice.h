#ifndef _SM_MEDIA_AUDIODEVICE
#define _SM_MEDIA_AUDIODEVICE

#include "Data/ArrayList.h"
#include "Media/IAudioRenderer.h"

namespace Media
{
	class AudioDevice
	{
	public:
		typedef enum
		{
			AT_WAVEOUT = 1,
			AT_ASIO = 2,
			AT_KS = 4,
			AT_ALSA = 8
		} APIType;
	private:
		Data::ArrayList<Media::IAudioRenderer*> rendererList;
		Media::IAudioRenderer *currRenderer;

	public:
		static UOSInt GetDeviceCount();
		static UTF8Char *GetDeviceName(UTF8Char *buff, UOSInt devIndex);
		static Media::IAudioRenderer *CreateRenderer(Text::CString devName);

		AudioDevice();
		~AudioDevice();

		Bool AddDevice(Text::CString devName);
		void ClearDevices();

		Media::IAudioRenderer *BindAudio(Media::IAudioSource *audsrc);
	};
}
#endif
