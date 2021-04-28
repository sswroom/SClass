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
		typedef void (__stdcall *MapUpdatedHandler)(void *userObj, Double centerX, Double centerY, Double drawTime);
		typedef void (__stdcall *ScaleChangedHandler)(void *userObj, Double newScale);
		typedef void (__stdcall *MouseMoveHandler)(void *userObj, OSInt x, OSInt y);
		typedef void (__stdcall *DrawHandler)(void *userObj, Media::DrawImage *dimg, OSInt xOfst, OSInt yOfst);
	private:
		Media::DrawImage *bgImg;
		Media::ColorManagerSess *colorSess;
		Map::MapView *view;
		Map::MapEnv *mapEnv;
		Map::DrawMapRenderer *renderer;
		Bool releaseRenderer;
		UOSInt currWidth;
		UOSInt currHeight;

		Bool bgUpdated;
		UInt32 bgColor;
		UInt32 bgDispColor;
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

		Data::ArrayList<ScaleChangedHandler> *scaleChgHdlrs;
		Data::ArrayList<void *> *scaleChgObjs;
		Data::ArrayList<MouseMoveHandler> *mouseMoveHdlrs;
		Data::ArrayList<void *> *mouseMoveObjs;
		Data::ArrayList<MapUpdatedHandler> *mapUpdHdlrs;
		Data::ArrayList<void *> *mapUpdObjs;
		MouseEventHandler mouseDownHdlr;
		void *mouseDownObj;
		MouseEventHandler mouseUpHdlr;
		void *mouseUpObj;
		DrawHandler drawHdlr;
		void *drawHdlrObj;
		Sync::Mutex *drawMut;
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
		void DrawScnObjects(Media::DrawImage *img, Double xOfst, Double yOfst);
	public:
		/*
		view - will release
		*/
		GUIMapControl(GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *eng, Int32 bgColor, Map::DrawMapRenderer *renderer, Map::MapView *view, Media::ColorManagerSess *colorSess);
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
		void ScnXY2MapXY(OSInt scnX, OSInt scnY, Double *mapX, Double *mapY);
		void ScnXYD2MapXY(Double scnX, Double scnY, Double *mapX, Double *mapY);
		void MapXY2ScnXY(Double mapX, Double mapY, OSInt *scnX, OSInt *scnY);
		void SetMapScale(Double newScale);
		Double GetMapScale();
		Double GetViewScale();
		void PanToMapXY(Double mapX, Double mapY);
		void ZoomToRect(Double mapX1, Double mapY1, Double mapX2, Double mapY2);
		Bool InMapMapXY(Double mapX, Double mapY);
		void ShowMarkerMapXY(Double mapX, Double mapY);
		void ShowMarkerMapXYDir(Double mapX, Double mapY, Double dir, Math::Unit::Angle::AngleUnit unit);
		void HideMarker();
		void SetSelectedVector(Math::Vector2D *vec);

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
