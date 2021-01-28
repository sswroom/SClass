#ifndef _SM_MEDIA_FREQUENCYGRAPH
#define _SM_MEDIA_FREQUENCYGRAPH
#include "Math/FFTCalc.h"
#include "Media/DrawEngine.h"
#include "Media/IAudioSource.h"

namespace Media
{
	class FrequencyGraph
	{
	public:
		static Media::DrawImage *CreateGraph(Media::DrawEngine *eng, Media::IAudioSource *audio, OSInt fftSize, OSInt timeRes, Math::FFTCalc::WindowType wtype, Double fHeight);
	};
};
#endif