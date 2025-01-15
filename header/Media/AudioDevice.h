#ifndef _SM_MEDIA_AUDIODEVICE
#define _SM_MEDIA_AUDIODEVICE
#include "Data/ArrayListNN.h"
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
		Data::ArrayListNN<Media::IAudioRenderer> rendererList;
		Optional<Media::IAudioRenderer> currRenderer;

	public:
		static UOSInt GetDeviceCount();
		static UnsafeArrayOpt<UTF8Char> GetDeviceName(UnsafeArray<UTF8Char> buff, UOSInt devIndex);
		static Optional<Media::IAudioRenderer> CreateRenderer(Text::CStringNN devName);

		AudioDevice();
		~AudioDevice();

		Bool AddDevice(Text::CStringNN devName);
		void ClearDevices();

		Optional<Media::IAudioRenderer> BindAudio(Optional<Media::IAudioSource> audsrc);
	};
}
#endif
