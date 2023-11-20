#ifndef _SM_UI_GUIVIDEOBOXDDLQ
#define _SM_UI_GUIVIDEOBOXDDLQ
#include "UI/GUIVideoBoxDD.h"

namespace UI
{
	class GUIVideoBoxDDLQ : public GUIVideoBoxDD
	{
	protected:
		virtual void ProcessVideo(ThreadStat *tstat, VideoBuff *vbuff, VideoBuff *vbuff2);
		virtual Media::IImgResizer *CreateResizer(Media::ColorManagerSess *colorSess, UInt32 bitDepth, Double srcRefLuminance);
		virtual void CreateCSConv(ThreadStat *tstat, Media::FrameInfo *info);
		virtual void CreateThreadResizer(ThreadStat *tstat);
	public:
		GUIVideoBoxDDLQ(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::ColorManagerSess> colorSess, UOSInt buffCnt, UOSInt threadCnt);
		virtual ~GUIVideoBoxDDLQ();
	};
}
#endif
