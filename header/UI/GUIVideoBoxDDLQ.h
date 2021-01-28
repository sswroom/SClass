#ifndef _SM_UI_GUIVIDEOBOXDDLQ
#define _SM_UI_GUIVIDEOBOXDDLQ
#include "UI/GUIVideoBoxDD.h"

namespace UI
{
	class GUIVideoBoxDDLQ : public GUIVideoBoxDD
	{
	protected:
		virtual void ProcessVideo(ThreadStat *tstat, VideoBuff *vbuff, VideoBuff *vbuff2);
		virtual Media::IImgResizer *CreateResizer(Media::ColorManagerSess *colorSess, Int32 bitDepth);
		virtual void CreateCSConv(ThreadStat *tstat, Media::FrameInfo *info);
		virtual void CreateThreadResizer(ThreadStat *tstat);
	public:
		GUIVideoBoxDDLQ(GUICore *ui, UI::GUIClientControl *parent, Media::ColorManagerSess *colorSess, OSInt buffCnt, OSInt threadCnt);
		virtual ~GUIVideoBoxDDLQ();
	};
};
#endif
