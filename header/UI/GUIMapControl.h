#ifndef _SM_UI_GUIMAPCONTROL
#define _SM_UI_GUIMAPCONTROL
#include "Map/DrawMapRenderer.h"
#include "Map/MapView.h"
#include "Math/Vector2D.h"
#include "Media/ColorManager.h"
#include "Media/DrawEngine.h"
#include "Sync/Mutex.h"
#include "UI/GUICustomDraw.h"

namespace UI
{
	class GUIMapControl : public GUICustomDraw, public Media::IColorHandler
	{
	public:
		typedef void (__stdcall *MapUpdatedHandler)(void *userObj, Math::Coord2DDbl center, Double drawTime);
		typedef void (__stdcall *ScaleChangedHandler)(void *userObj, Double newScale);
		typedef void (__stdcall *MouseMoveHandler)(void *userObj, Math::Coord2D<OSInt> scnPos);
		typedef void (__stdcall *DrawHandler)(void *userObj, Media::DrawImage *dimg, OSInt xOfst, OSInt yOfst);
	private:
		Media::DrawImage *bgImg;
		Media::ColorManagerSess *colorSess;
		Map::MapView *view;
		Map::MapEnv *mapEnv;
		Map::DrawMapRenderer *renderer;
		Bool releaseRenderer;
		Math::Size2D<UOSInt> currSize;

		Bool bgUpdated;
		UInt32 bgColor;
		UInt32 bgDispColor;
		Int64 mouseLDownTime;
		Int64 mouseRDownTime;
		Bool mouseDown;
		OSInt mouseDownX;
		OSInt mouseDownY;
		OSInt mouseCurrX;
		OSInt mouseCurrY;
		Bool gZoom;
		OSInt gZoomX;
		OSInt gZoomY;
		UInt64 gZoomDist;
		UInt64 gZoomCurrDist;
		OSInt gZoomCurrX;
		OSInt gZoomCurrY;
		Bool pauseUpdate;

		Double markerX;
		Double markerY;
		Double markerDir;
		Bool markerHasDir;
		Bool showMarker;
		Math::Vector2D *selVec;

		Data::ArrayList<ScaleChangedHandler> scaleChgHdlrs;
		Data::ArrayList<void *> scaleChgObjs;
		Data::ArrayList<MouseMoveHandler> mouseMoveHdlrs;
		Data::ArrayList<void *> mouseMoveObjs;
		Data::ArrayList<MapUpdatedHandler> mapUpdHdlrs;
		Data::ArrayList<void *> mapUpdObjs;
		MouseEventHandler mouseDownHdlr;
		void *mouseDownObj;
		MouseEventHandler mouseUpHdlr;
		void *mouseUpObj;
		DrawHandler drawHdlr;
		void *drawHdlrObj;
		Sync::Mutex drawMut;
		Int64 imgTimeoutTick;

	private:
		static void __stdcall ImageUpdated(void *userObj);

		virtual Bool OnMouseDown(OSInt scnX, OSInt scnY, MouseButton btn);
		virtual Bool OnMouseUp(OSInt scnX, OSInt scnY, MouseButton btn);
		virtual void OnMouseMove(OSInt scnX, OSInt scnY) ;
		virtual Bool OnMouseWheel(OSInt scnX, OSInt scnY, Int32 delta);
		virtual void OnGestureBegin(OSInt scnX, OSInt scnY, UInt64 dist);
		virtual void OnGestureStep(OSInt scnX, OSInt scnY, UInt64 dist);
		virtual void OnGestureEnd(OSInt scnX, OSInt scnY, UInt64 dist);
		virtual void OnJSButtonDown(OSInt buttonId);
		virtual void OnJSButtonUp(OSInt buttonId);
		virtual void OnJSAxis(OSInt axis1, OSInt axis2, OSInt axis3, OSInt axis4);

		virtual void OnTimerTick();
		virtual void OnDraw(Media::DrawImage *img);
		void DrawScnObjects(Media::DrawImage *img, Math::Coord2DDbl ofst);
	public:
		/*
		view - will release
		*/
		GUIMapControl(GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *eng, UInt32 bgColor, Map::DrawMapRenderer *renderer, Map::MapView *view, Media::ColorManagerSess *colorSess);
		GUIMapControl(GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *eng, Map::MapEnv *mapEnv, Media::ColorManagerSess *colorSess);
		virtual ~GUIMapControl();

		virtual void OnSizeChanged(Bool updateScn);

		virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
		virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);

		virtual void SetDPI(Double hdpi, Double ddpi);

		void EventScaleChanged(Double newScale);

		void SetBGColor(UInt32 bgColor);
		void SetRenderer(Map::DrawMapRenderer *renderer);
		void UpdateMap();
		Math::Coord2DDbl ScnXY2MapXY(Math::Coord2D<OSInt> scnPos);
		Math::Coord2DDbl ScnXYD2MapXY(Math::Coord2DDbl scnPos);
		Math::Coord2D<OSInt> MapXY2ScnXY(Math::Coord2DDbl mapPos);
		void SetMapScale(Double newScale);
		Double GetMapScale();
		Double GetViewScale();
		void PanToMapXY(Math::Coord2DDbl mapPos);
		void ZoomToRect(Math::RectAreaDbl mapRect);
		Bool InMapMapXY(Math::Coord2DDbl mapPos);
		void ShowMarkerMapXY(Double mapX, Double mapY);
		void ShowMarkerMapXYDir(Double mapX, Double mapY, Double dir, Math::Unit::Angle::AngleUnit unit);
		void HideMarker();
		void SetSelectedVector(Math::Vector2D *vec);
		void SetVAngle(Double angleRad);

		void HandleScaleChanged(ScaleChangedHandler hdlr, void *userObj);
		void HandleMapUpdated(MapUpdatedHandler hdlr, void *userObj);
		void HandleMouseMove(MouseMoveHandler hdlr, void *userObj);
		void HandleMouseUp(MouseEventHandler hdlr, void *userObj);
		void HandleMouseDown(MouseEventHandler hdlr, void *userObj);
		void HandleCustomDraw(DrawHandler hdlr, void *userObj);
		void SetMapUpdated();
		void UpdateMapView(Map::MapView *view);
		Map::MapView *CloneMapView();
		void PauseUpdate(Bool pauseUpdate);
	};
};
#endif
