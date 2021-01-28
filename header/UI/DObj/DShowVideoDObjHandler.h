#ifndef _SM_UI_DOBJ_DSHOWVIDEODOBJHANDLER
#define _SM_UI_DOBJ_DSHOWVIDEODOBJHANDLER
#include "Media/DShow/DShowManager.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "UI/GUIForm.h"
#include "UI/DObj/ImageDObjHandler.h"

namespace UI
{
	namespace DObj
	{
		class DShowVideoDObjHandler : public ImageDObjHandler
		{
		protected:
			Int32 videoX;
			Int32 videoY;
			Int32 videoW;
			Int32 videoH;

			UI::GUIForm *ownerFrm;
			UI::GUITimer *tmr;
			const WChar *videoFileName;
			Media::DShow::DShowManager *dshowMgr;
			Media::DShow::DShowGraph *graph;
			Media::DShow::DShowVideoRecvFilter *renderer;
			Media::IImgResizer *resizer;
			Sync::Mutex *frameMut;
			Media::DrawImage *frameImg;

			static void __stdcall OnVideoFrame(void *userObj, UInt8 *frameBuff, Int32 frameTime, Int32 frameW, Int32 frameH);
			static void __stdcall OnTimerTick(void *userObj);
		protected:
			virtual void DrawBkg(Media::DrawImage *dimg);
			void DrawVideo(Media::DrawImage *dimg);
		public:
			DShowVideoDObjHandler(UI::GUIForm *ownerFrm, Media::DrawEngine *deng, const WChar *imageFileName, Int32 videoX, Int32 videoY, Int32 videoW, Int32 videoH, const WChar *videoFileName);
			virtual ~DShowVideoDObjHandler();
		};
	}
}
#endif
