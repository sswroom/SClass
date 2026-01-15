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
			Math::Coord2D<IntOS> videoTL;
			Math::Size2D<UIntOS> videoSize;

			NN<UI::GUIForm> ownerFrm;
			NN<UI::GUITimer> tmr;
			NN<Text::String> videoFileName;
			Media::Decoder::VideoDecoderFinder vdecoders;
			Media::VideoSource *decoder;
			Media::ImageResizer *resizer;
			Sync::Mutex frameMut;
			Optional<Media::DrawImage> frameImg;

			static void __stdcall OnVideoFrame(AnyType userObj, UInt8 *frameBuff, Int32 frameTime, UInt32 frameW, UInt32 frameH);
			static void __stdcall OnTimerTick(AnyType userObj);
		protected:
			virtual void DrawBkg(NN<Media::DrawImage> dimg);
			void DrawVideo(NN<Media::DrawImage> dimg);
		public:
			DShowVideoDObjHandler(NN<UI::GUIForm> ownerFrm, NN<Media::DrawEngine> deng, Optional<Media::ColorSess> colorSess, Text::CStringNN imageFileName, Math::Coord2D<IntOS> videoPos, Math::Size2D<UIntOS> videoSize, Text::CStringNN videoFileName);
			virtual ~DShowVideoDObjHandler();
		};
	}
}
#endif
