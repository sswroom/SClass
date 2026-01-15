#ifndef _SM_MEDIA_FREQUENCYGRAPH
#define _SM_MEDIA_FREQUENCYGRAPH
#include "Math/FFTCalc.h"
#include "Media/DrawEngine.h"
#include "Media/AudioSource.h"

namespace Media
{
	class FrequencyGraph
	{
	public:
		static Optional<Media::DrawImage> CreateGraph(NN<Media::DrawEngine> eng, NN<Media::AudioSource> audio, UIntOS fftSize, UIntOS timeRes, Math::FFTCalc::WindowType wtype, Double fontSizePx);
	};
}
#endif