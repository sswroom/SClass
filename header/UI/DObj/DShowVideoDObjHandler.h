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
			Math::Coord2D<OSInt> videoTL;
			Math::Size2D<UOSInt> videoSize;

			NN<UI::GUIForm> ownerFrm;
			NN<UI::GUITimer> tmr;
			NN<Text::String> videoFileName;
			Media::Decoder::VideoDecoderFinder vdecoders;
			Media::IVideoSource *decoder;
			Media::IImgResizer *resizer;
			Sync::Mutex frameMut;
			Media::DrawImage *frameImg;

			static void __stdcall OnVideoFrame(AnyType userObj, UInt8 *frameBuff, Int32 frameTime, UInt32 frameW, UInt32 frameH);
			static void __stdcall OnTimerTick(AnyType userObj);
		protected:
			virtual void DrawBkg(NN<Media::DrawImage> dimg);
			void DrawVideo(NN<Media::DrawImage> dimg);
		public:
			DShowVideoDObjHandler(NN<UI::GUIForm> ownerFrm, NN<Media::DrawEngine> deng, Text::CStringNN imageFileName, Math::Coord2D<OSInt> videoPos, Math::Size2D<UOSInt> videoSize, Text::CString videoFileName);
			virtual ~DShowVideoDObjHandler();
		};
	}
}
#endif
