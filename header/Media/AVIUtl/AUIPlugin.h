#ifndef _SM_MEDIA_AVIUTL_AUIPLUGIN
#define _SM_MEDIA_AVIUTL_AUIPLUGIN
#include "Data/ArrayListNN.h"
#include "IO/FileSelector.h"
#include "Media/AudioFormat.h"
#include "Media/FrameInfo.h"
#include "Media/IMediaSource.h"

namespace Media
{
	namespace AVIUtl
	{
		class AUIPlugin
		{
		private:
			typedef struct
			{
				void *hMod;
				UInt32 useCnt;
				void *pluginTable;
			} PluginDetail;

		public:
			typedef struct
			{
				void *hand;
				UInt32 useCnt;
			} AUIInput;

		private:
			PluginDetail *plugin;

			AUIPlugin();
			AUIPlugin(const AUIPlugin *plugin);
		public:
			~AUIPlugin();

			NN<AUIPlugin> Clone() const;
			UOSInt LoadFile(const Char *fileName, NN<Data::ArrayListNN<Media::IMediaSource>> outArr);

			Bool CloseInput(void *hand);
			Bool GetInputVideoInfo(void *hand, NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UInt32> frameCnt);
			Bool GetInputAudioInfo(void *hand, NN<Media::AudioFormat> af, OutParam<UInt32> sampleCnt);
			UOSInt GetVideoFrame(void *hand, UOSInt frameNum, UInt8 *buff);
			UOSInt GetAudioData(void *hand, UOSInt startSample, UOSInt sampleLength, UInt8 *buff);
			Bool IsVideoKeyFrame(void *hand, UInt32 frameNum);
			Bool ConfigInput(void *hInst, void *hWnd);
			void PrepareSelector(NN<IO::FileSelector> selector);

			static Optional<AUIPlugin> LoadPlugin(const WChar *fileName);
		};
	}
}
#endif
