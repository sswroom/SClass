#ifndef _SM_UI_GUIPICTUREBOXDD
#define _SM_UI_GUIPICTUREBOXDD
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "Math/Coord2D.h"
#include "Math/RectArea.h"
#include "Media/ColorManager.h"
#include "Media/ImageResizer.h"
#include "Media/StaticImage.h"
#include "Media/CS/CSConverter.h"
#include "Parser/ParserList.h"
#include "UI/GUIDDrawControl.h"

namespace UI
{
	class GUIPictureBoxDD : public GUIDDrawControl, public Media::ColorHandler
	{
	public:
		typedef void (CALLBACKFUNC DrawHandler32)(AnyType userObj, UnsafeArray<UInt8> imgPtr, UOSInt w, UOSInt h, UOSInt bpl);
	private:
		Data::ArrayList<Data::CallbackStorage<MouseEventHandler>> mouseDownHdlrs;
		Data::ArrayList<Data::CallbackStorage<MouseEventHandler>> mouseUpHdlrs;
		Data::ArrayList<Data::CallbackStorage<MouseEventHandler>> mouseMoveHdlrs;
		Data::ArrayList<Data::CallbackStorage<DrawHandler32>> drawHdlrs;
		Data::ArrayList<Data::CallbackStorage<UI::UIEvent>> moveToNextHdlrs;
		Data::ArrayList<Data::CallbackStorage<UI::UIEvent>> moveToPrevHdlrs;

		UInt8 *bgBuff;
		Math::Size2D<UOSInt> bgBuffSize;

		Optional<Media::CS::CSConverter> csconv;
		NN<Media::ColorManagerSess> colorSess;
		Optional<Media::RasterImage> currImage;
		Math::Size2D<UOSInt> currImageSize;
		Media::ImageResizer *resizer;
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
		void CalDispRect(NN<Math::RectAreaDbl> srcRect, NN<Math::RectArea<OSInt>> destRect);
		void UpdateZoomRange();
		void UpdateMinScale();
		void CreateResizer();
		static void __stdcall OnSizeChg(AnyType userObj);
		void DrawFromBG();
		virtual void OnPaint();
	public:
		GUIPictureBoxDD(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::ColorManagerSess> colorSess, Bool allowEnlarge, Bool directMode);
		virtual ~GUIPictureBoxDD();

		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void ChangeMonitor(Optional<MonitorHandle> hMon);
		virtual void SetDPI(Double hdpi, Double ddpi);

		void EnableLRGBLimit(Bool enable);
		void SetImage(Optional<Media::RasterImage> currImage, Bool sameImg);

		virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
		virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);
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
		Optional<Media::StaticImage> CreatePreviewImage(NN<const Media::StaticImage> image);

		void HandleMouseDown(MouseEventHandler hdlr, AnyType userObj);
		void HandleMouseMove(MouseEventHandler hdlr, AnyType userObj);
		void HandleMouseUp(MouseEventHandler hdlr, AnyType userObj);
		void HandleDraw(DrawHandler32 hdlr, AnyType userObj);
		void HandleMoveToNext(UI::UIEvent hdlr, AnyType userObj);
		void HandleMoveToPrev(UI::UIEvent hdlr, AnyType userObj);
		Math::Coord2DDbl Scn2ImagePos(Math::Coord2D<OSInt> scnPos);
		Math::Coord2DDbl Image2ScnPos(Math::Coord2DDbl imgPos);
		void ZoomToFit();
		void UpdateBufferImage();
	};
}
#endif
