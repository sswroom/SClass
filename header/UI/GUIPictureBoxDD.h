#ifndef _SM_UI_GUIPICTUREBOXDD
#define _SM_UI_GUIPICTUREBOXDD
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
		typedef void (__stdcall *DrawHandler32)(void *userObj, UInt8 *imgPtr, OSInt w, OSInt h, OSInt bpl);
	private:
		Data::ArrayList<MouseEventHandler> *mouseDownHdlrs;
		Data::ArrayList<void *> *mouseDownObjs;
		Data::ArrayList<MouseEventHandler> *mouseUpHdlrs;
		Data::ArrayList<void *> *mouseUpObjs;
		Data::ArrayList<MouseEventHandler> *mouseMoveHdlrs;
		Data::ArrayList<void *> *mouseMoveObjs;
		Data::ArrayList<DrawHandler32> *drawHdlrs;
		Data::ArrayList<void *> *drawObjs;
		Data::ArrayList<UI::UIEvent> *moveToNextHdlrs;
		Data::ArrayList<void *> *moveToNextObjs;
		Data::ArrayList<UI::UIEvent> *moveToPrevHdlrs;
		Data::ArrayList<void *> *moveToPrevObjs;

		UInt8 *bgBuff;
		UOSInt bgBuffW;
		UOSInt bgBuffH;

		Media::CS::CSConverter *csconv;
		Media::ColorManagerSess *colorSess;
		Media::Image *currImage;
		OSInt currImageW;
		OSInt currImageH;
		Media::IImgResizer *resizer;
		UInt8 *imgBuff;
		Bool allowEnlarge;
		Double zoomScale;
		Double zoomCenterX;
		Double zoomCenterY;
		Bool mouseDowned;
		OSInt mouseCurrX;
		OSInt mouseCurrY;
		OSInt mouseDownX;
		OSInt mouseDownY;
		Double zoomMinX;
		Double zoomMinY;
		Double zoomMaxX;
		Double zoomMaxY;
		Double zoomMinScale;
		Bool gzoomDown;
		OSInt gzoomDownX;
		OSInt gzoomDownY;
		UInt64 gzoomDownDist;
		OSInt gzoomCurrX;
		OSInt gzoomCurrY;
		UInt64 gzoomCurrDist;
		Bool curr10Bit;
		Bool enableLRGBLimit;

	private:
		void UpdateSubSurface();
		void CalDispRect(Double *srcRect, OSInt *destRect);
		void UpdateZoomRange();
		void UpdateMinScale();
		void CreateResizer();
		static void __stdcall OnSizeChg(void *userObj);
		void DrawFromBG();
		virtual void OnPaint();
	public:
		GUIPictureBoxDD(GUICore *ui, UI::GUIClientControl *parent, Media::ColorManagerSess *colorSess, Bool allowEnlarge, Bool directMode);
		virtual ~GUIPictureBoxDD();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void ChangeMonitor(void *hMon);
		virtual void SetDPI(Double hdpi, Double ddpi);

		void EnableLRGBLimit(Bool enable);
		void SetImage(Media::Image *currImage, Bool sameImg);

		virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
		virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);
		void SetAllowEnlarge(Bool allowEnlarge);

		virtual void OnSurfaceCreated();
		virtual void OnMouseWheel(OSInt x, OSInt y, Int32 amount);
		virtual void OnMouseMove(OSInt x, OSInt y);
		virtual void OnMouseDown(OSInt x, OSInt y, MouseButton button);
		virtual void OnMouseUp(OSInt x, OSInt y, MouseButton button);
		virtual void OnGZoomBegin(OSInt x, OSInt y, UInt64 dist);
		virtual void OnGZoomStep(OSInt x, OSInt y, UInt64 dist);
		virtual void OnGZoomEnd(OSInt x, OSInt y, UInt64 dist);
		virtual void OnJSButtonDown(OSInt buttonId);
		virtual void OnJSButtonUp(OSInt buttonId);
		virtual void OnJSAxis(OSInt axis1, OSInt axis2, OSInt axis3, OSInt axis4);
		void EventMoveToNext();
		void EventMoveToPrev();

		Bool GetImageViewSize(UOSInt *viewSize, UOSInt imageWidth, UOSInt imageHeight);
		Media::StaticImage *CreatePreviewImage(Media::StaticImage *image);

		void HandleMouseDown(MouseEventHandler hdlr, void *userObj);
		void HandleMouseMove(MouseEventHandler hdlr, void *userObj);
		void HandleMouseUp(MouseEventHandler hdlr, void *userObj);
		void HandleDraw(DrawHandler32 hdlr, void *userObj);
		void HandleMoveToNext(UI::UIEvent hdlr, void *userObj);
		void HandleMoveToPrev(UI::UIEvent hdlr, void *userObj);
		void Scn2ImagePos(OSInt x, OSInt y, Double *imgX, Double *imgY);
		void Image2ScnPos(Double x, Double y, Double *scnX, Double *scnY);
		void ZoomToFit();
	};
}
#endif
