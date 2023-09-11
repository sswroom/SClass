#ifndef _SM_UI_GUIPICTUREBOXDD
#define _SM_UI_GUIPICTUREBOXDD
#include "Math/Coord2D.h"
#include "Math/RectArea.h"
#include "Media/ColorManager.h"
#include "Media/IImgResizer.h"
#include "Media/StaticImage.h"
#include "Media/CS/CSConverter.h"
#include "Parser/ParserList.h"
#include "UI/GUIDDrawControl.h"

namespace UI
{
	class GUIPictureBoxDD : public GUIDDrawControl, public Media::IColorHandler
	{
	public:
		typedef void (__stdcall *DrawHandler32)(void *userObj, UInt8 *imgPtr, UOSInt w, UOSInt h, UOSInt bpl);
	private:
		Data::ArrayList<MouseEventHandler> mouseDownHdlrs;
		Data::ArrayList<void *> mouseDownObjs;
		Data::ArrayList<MouseEventHandler> mouseUpHdlrs;
		Data::ArrayList<void *> mouseUpObjs;
		Data::ArrayList<MouseEventHandler> mouseMoveHdlrs;
		Data::ArrayList<void *> mouseMoveObjs;
		Data::ArrayList<DrawHandler32> drawHdlrs;
		Data::ArrayList<void *> drawObjs;
		Data::ArrayList<UI::UIEvent> moveToNextHdlrs;
		Data::ArrayList<void *> moveToNextObjs;
		Data::ArrayList<UI::UIEvent> moveToPrevHdlrs;
		Data::ArrayList<void *> moveToPrevObjs;

		UInt8 *bgBuff;
		Math::Size2D<UOSInt> bgBuffSize;

		Media::CS::CSConverter *csconv;
		Media::ColorManagerSess *colorSess;
		Media::Image *currImage;
		Math::Size2D<UOSInt> currImageSize;
		Media::IImgResizer *resizer;
		UInt8 *imgBuff;
		Bool allowEnlarge;
		Double zoomScale;
		Math::Coord2DDbl zoomCenter;
		Bool mouseDowned;
		Math::Coord2D<OSInt> mouseCurrPos;
		Math::Coord2D<OSInt> mouseDownPos;
		Double zoomMinX;
		Double zoomMinY;
		Double zoomMaxX;
		Double zoomMaxY;
		Double zoomMinScale;
		Bool gzoomDown;
		Math::Coord2D<OSInt> gzoomDownPos;
		UInt64 gzoomDownDist;
		Math::Coord2D<OSInt> gzoomCurrPos;
		UInt64 gzoomCurrDist;
		Bool curr10Bit;
		Bool enableLRGBLimit;

	private:
		void UpdateSubSurface();
		void CalDispRect(NotNullPtr<Math::RectAreaDbl> srcRect, NotNullPtr<Math::RectArea<OSInt>> destRect);
		void UpdateZoomRange();
		void UpdateMinScale();
		void CreateResizer();
		static void __stdcall OnSizeChg(void *userObj);
		void DrawFromBG();
		virtual void OnPaint();
	public:
		GUIPictureBoxDD(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, Media::ColorManagerSess *colorSess, Bool allowEnlarge, Bool directMode);
		virtual ~GUIPictureBoxDD();

		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void ChangeMonitor(MonitorHandle *hMon);
		virtual void SetDPI(Double hdpi, Double ddpi);

		void EnableLRGBLimit(Bool enable);
		void SetImage(Media::Image *currImage, Bool sameImg);

		virtual void YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam);
		virtual void RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam);
		void SetAllowEnlarge(Bool allowEnlarge);

		virtual void OnSurfaceCreated();
		virtual void OnMouseWheel(Math::Coord2D<OSInt> pos, Int32 amount);
		virtual void OnMouseMove(Math::Coord2D<OSInt> pos);
		virtual void OnMouseDown(Math::Coord2D<OSInt> pos, MouseButton button);
		virtual void OnMouseUp(Math::Coord2D<OSInt> pos, MouseButton button);
		virtual void OnGZoomBegin(Math::Coord2D<OSInt> pos, UInt64 dist);
		virtual void OnGZoomStep(Math::Coord2D<OSInt> pos, UInt64 dist);
		virtual void OnGZoomEnd(Math::Coord2D<OSInt> pos, UInt64 dist);
		virtual void OnJSButtonDown(OSInt buttonId);
		virtual void OnJSButtonUp(OSInt buttonId);
		virtual void OnJSAxis(OSInt axis1, OSInt axis2, OSInt axis3, OSInt axis4);
		void EventMoveToNext();
		void EventMoveToPrev();

		Bool GetImageViewSize(Math::Size2D<UOSInt> *viewSize, Math::Size2D<UOSInt> imageSize);
		NotNullPtr<Media::StaticImage> CreatePreviewImage(NotNullPtr<const Media::StaticImage> image);

		void HandleMouseDown(MouseEventHandler hdlr, void *userObj);
		void HandleMouseMove(MouseEventHandler hdlr, void *userObj);
		void HandleMouseUp(MouseEventHandler hdlr, void *userObj);
		void HandleDraw(DrawHandler32 hdlr, void *userObj);
		void HandleMoveToNext(UI::UIEvent hdlr, void *userObj);
		void HandleMoveToPrev(UI::UIEvent hdlr, void *userObj);
		Math::Coord2DDbl Scn2ImagePos(Math::Coord2D<OSInt> scnPos);
		Math::Coord2DDbl Image2ScnPos(Math::Coord2DDbl imgPos);
		void ZoomToFit();
		void UpdateBufferImage();
	};
}
#endif
