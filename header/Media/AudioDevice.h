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
		Data::ArrayList<Media::IAudioRenderer*> *rendererList;
		Media::IAudioRenderer *currRenderer;

	public:
		static OSInt GetDeviceCount();
		static UTF8Char *GetDeviceName(UTF8Char *buff, OSInt devIndex);
		static Media::IAudioRenderer *CreateRenderer(const UTF8Char *devName);

		AudioDevice();
		~AudioDevice();

		Bool AddDevice(const UTF8Char *devName);

		Media::IAudioRenderer *BindAudio(Media::IAudioSource *audsrc);
	};
};
#endif
