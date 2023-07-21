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
		static Media::DrawImage *CreateGraph(NotNullPtr<Media::DrawEngine> eng, Media::IAudioSource *audio, UOSInt fftSize, UOSInt timeRes, Math::FFTCalc::WindowType wtype, Double fontSizePx);
	};
}
#endif