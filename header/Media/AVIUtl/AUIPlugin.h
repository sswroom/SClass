#ifndef _SM_MEDIA_AVIUTL_AUIPLUGIN
#define _SM_MEDIA_AVIUTL_AUIPLUGIN
#include "Data/ArrayListNN.hpp"
#include "IO/FileSelector.h"
#include "Media/AudioFormat.h"
#include "Media/FrameInfo.h"
#include "Media/MediaSource.h"

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
			UIntOS LoadFile(const Char *fileName, NN<Data::ArrayListNN<Media::MediaSource>> outArr);

			Bool CloseInput(void *hand);
			Bool GetInputVideoInfo(void *hand, NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UInt32> frameCnt);
			Bool GetInputAudioInfo(void *hand, NN<Media::AudioFormat> af, OutParam<UInt32> sampleCnt);
			UIntOS GetVideoFrame(void *hand, UIntOS frameNum, UnsafeArray<UInt8> buff);
			UIntOS GetAudioData(void *hand, UIntOS startSample, UIntOS sampleLength, UInt8 *buff);
			Bool IsVideoKeyFrame(void *hand, UInt32 frameNum);
			Bool ConfigInput(void *hInst, void *hWnd);
			void PrepareSelector(NN<IO::FileSelector> selector);

			static Optional<AUIPlugin> LoadPlugin(const WChar *fileName);
		};
	}
}
#endif
