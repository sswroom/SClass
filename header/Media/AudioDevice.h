#ifndef _SM_MEDIA_AUDIODEVICE
#define _SM_MEDIA_AUDIODEVICE
#include "Data/ArrayListNN.hpp"
#include "Media/AudioRenderer.h"

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
		Data::ArrayListNN<Media::AudioRenderer> rendererList;
		Optional<Media::AudioRenderer> currRenderer;

	public:
		static UOSInt GetDeviceCount();
		static UnsafeArrayOpt<UTF8Char> GetDeviceName(UnsafeArray<UTF8Char> buff, UOSInt devIndex);
		static Optional<Media::AudioRenderer> CreateRenderer(Text::CStringNN devName);

		AudioDevice();
		~AudioDevice();

		Bool AddDevice(Text::CStringNN devName);
		void ClearDevices();

		Optional<Media::AudioRenderer> BindAudio(Optional<Media::AudioSource> audsrc);
	};
}
#endif
