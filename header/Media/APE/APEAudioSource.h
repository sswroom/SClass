#ifndef _SM_MEDIA_APE_APEAUDIOSOURCE
#define _SM_MEDIA_APE_APEAUDIOSOURCE
#include "Media/IAudioSource.h"
#include "Media/APE/APEIO.h"
#define NO_DEFINE_ENVIRONMENT_VARIABLES
#include "Media/APE/All.h"
#include "Media/APE/MACLib.h"

namespace Media
{
	namespace APE
	{
		class APEAudioSource : public IAudioSource
		{
		private:
			IAPEDecompress *ape;
			Media::APE::APEIO *io;
			Media::AudioFormat *fmt;
			Int32 currBlock;
			Sync::Event *evt;

		public:
			APEAudioSource(IAPEDecompress *ape, Media::APE::APEIO *io);
			virtual ~APEAudioSource();

			virtual WChar *GetName(WChar *buff);
			virtual Bool CanSeek();
			virtual Data::Duration GetStreamTime();
			virtual void GetFormat(AudioFormat *format);

			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual Bool Start(Sync::Event *evt, Int32 blkSize);
			virtual void Stop();
			virtual OSInt ReadBlock(UInt8 *buff, OSInt blkSize);
			virtual OSInt GetMinBlockSize();
			virtual Data::Duration GetCurrTime();
		};
	}
};
#endif
