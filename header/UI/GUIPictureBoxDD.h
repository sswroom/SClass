#ifndef _SM_UI_GUIPICTUREBOXDD
#define _SM_UI_GUIPICTUREBOXDD
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "Math/Coord2D.h"
#include "Math/RectArea.hpp"
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
		typedef void (CALLBACKFUNC DrawHandler32)(AnyType userObj, UnsafeArray<UInt8> imgPtr, UIntOS w, UIntOS h, UIntOS bpl);
	private:
		Data::ArrayListObj<Data::CallbackStorage<MouseEventHandler>> mouseDownHdlrs;
		Data::ArrayListObj<Data::CallbackStorage<MouseEventHandler>> mouseUpHdlrs;
		Data::ArrayListObj<Data::CallbackStorage<MouseEventHandler>> mouseMoveHdlrs;
		Data::ArrayListObj<Data::CallbackStorage<DrawHandler32>> drawHdlrs;
		Data::ArrayListObj<Data::CallbackStorage<UI::UIEvent>> moveToNextHdlrs;
		Data::ArrayListObj<Data::CallbackStorage<UI::UIEvent>> moveToPrevHdlrs;

		UInt8 *bgBuff;
		Math::Size2D<UIntOS> bgBuffSize;

		Optional<Media::CS::CSConverter> csconv;
		NN<Media::ColorManagerSess> colorSess;
		Optional<Media::RasterImage> currImage;
		Math::Size2D<UIntOS> currImageSize;
		Media::ImageResizer *resizer;
		UInt8 *imgBuff;
		Bool allowEnlarge;
		Double zoomScale;
		Math::Coord2DDbl zoomCenter;
		Bool mouseDowned;
		Math::Coord2D<IntOS> mouseCurrPos;
		Math::Coord2D<IntOS> mouseDownPos;
		Double zoomMinX;
		Double zoomMinY;
		Double zoomMaxX;
		Double zoomMaxY;
		Double zoomMinScale;
		Bool gzoomDown;
		Math::Coord2D<IntOS> gzoomDownPos;
		UInt64 gzoomDownDist;
		Math::Coord2D<IntOS> gzoomCurrPos;
		UInt64 gzoomCurrDist;
		Bool curr10Bit;
		Bool enableLRGBLimit;

	private:
		void UpdateSubSurface();
		void CalDispRect(NN<Math::RectAreaDbl> srcRect, NN<Math::RectArea<IntOS>> destRect);
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
		virtual IntOS OnNotify(UInt32 code, void *lParam);
		virtual void ChangeMonitor(Optional<MonitorHandle> hMon);
		virtual void SetDPI(Double hdpi, Double ddpi);

		void EnableLRGBLimit(Bool enable);
		void SetImage(Optional<Media::RasterImage> currImage, Bool sameImg);

		virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
		virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);
		void SetAllowEnlarge(Bool allowEnlarge);

		virtual void OnSurfaceCreated();
		virtual void OnMouseWheel(Math::Coord2D<IntOS> pos, Int32 amount);
		virtual void OnMouseMove(Math::Coord2D<IntOS> pos);
		virtual void OnMouseDown(Math::Coord2D<IntOS> pos, MouseButton button);
		virtual void OnMouseUp(Math::Coord2D<IntOS> pos, MouseButton button);
		virtual void OnGZoomBegin(Math::Coord2D<IntOS> pos, UInt64 dist);
		virtual void OnGZoomStep(Math::Coord2D<IntOS> pos, UInt64 dist);
		virtual void OnGZoomEnd(Math::Coord2D<IntOS> pos, UInt64 dist);
		virtual void OnJSButtonDown(IntOS buttonId);
		virtual void OnJSButtonUp(IntOS buttonId);
		virtual void OnJSAxis(IntOS axis1, IntOS axis2, IntOS axis3, IntOS axis4);
		void EventMoveToNext();
		void EventMoveToPrev();

		Bool GetImageViewSize(Math::Size2D<UIntOS> *viewSize, Math::Size2D<UIntOS> imageSize);
		Optional<Media::StaticImage> CreatePreviewImage(NN<const Media::StaticImage> image);

		void HandleMouseDown(MouseEventHandler hdlr, AnyType userObj);
		void HandleMouseMove(MouseEventHandler hdlr, AnyType userObj);
		void HandleMouseUp(MouseEventHandler hdlr, AnyType userObj);
		void HandleDraw(DrawHandler32 hdlr, AnyType userObj);
		void HandleMoveToNext(UI::UIEvent hdlr, AnyType userObj);
		void HandleMoveToPrev(UI::UIEvent hdlr, AnyType userObj);
		Math::Coord2DDbl Scn2ImagePos(Math::Coord2D<IntOS> scnPos);
		Math::Coord2DDbl Image2ScnPos(Math::Coord2DDbl imgPos);
		void ZoomToFit();
		void UpdateBufferImage();
	};
}
#endif
