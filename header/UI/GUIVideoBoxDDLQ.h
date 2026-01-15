#ifndef _SM_UI_GUIVIDEOBOXDDLQ
#define _SM_UI_GUIVIDEOBOXDDLQ
#include "UI/GUIVideoBoxDD.h"

namespace UI
{
	class GUIVideoBoxDDLQ : public GUIVideoBoxDD
	{
	protected:
		virtual void ProcessVideo(NN<ThreadStat> tstat, VideoBuff *vbuff, VideoBuff *vbuff2);
		virtual NN<Media::ImageResizer> CreateResizer(NN<Media::ColorManagerSess> colorSess, UInt32 bitDepth, Double srcRefLuminance);
		virtual void CreateCSConv(NN<ThreadStat> tstat, Media::FrameInfo *info);
		virtual void CreateThreadResizer(NN<ThreadStat> tstat);
	public:
		GUIVideoBoxDDLQ(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::ColorManagerSess> colorSess, UIntOS buffCnt, UIntOS threadCnt);
		virtual ~GUIVideoBoxDDLQ();
	};
}
#endif
