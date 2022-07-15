#ifndef _SM_UI_DOBJ_DSHOWVIDEODOBJHANDLER
#define _SM_UI_DOBJ_DSHOWVIDEODOBJHANDLER
#include "Media/Decoder/VideoDecoderFinder.h"
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
			UInt32 videoW;
			UInt32 videoH;

			UI::GUIForm *ownerFrm;
			UI::GUITimer *tmr;
			Text::String *videoFileName;
			Media::Decoder::VideoDecoderFinder vdecoders;
			Media::IVideoSource *decoder;
			Media::IImgResizer *resizer;
			Sync::Mutex frameMut;
			Media::DrawImage *frameImg;

			static void __stdcall OnVideoFrame(void *userObj, UInt8 *frameBuff, Int32 frameTime, UInt32 frameW, UInt32 frameH);
			static void __stdcall OnTimerTick(void *userObj);
		protected:
			virtual void DrawBkg(Media::DrawImage *dimg);
			void DrawVideo(Media::DrawImage *dimg);
		public:
			DShowVideoDObjHandler(UI::GUIForm *ownerFrm, Media::DrawEngine *deng, Text::CString imageFileName, Int32 videoX, Int32 videoY, UInt32 videoW, UInt32 videoH, Text::CString videoFileName);
			virtual ~DShowVideoDObjHandler();
		};
	}
}
#endif
