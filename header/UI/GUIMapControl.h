#ifndef _SM_UI_GUIMAPCONTROL
#define _SM_UI_GUIMAPCONTROL
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "Map/DrawMapRenderer.h"
#include "Map/MapView.h"
#include "Math/Geometry/Vector2D.h"
#include "Media/ColorManager.h"
#include "Media/DrawEngine.h"
#include "Sync/Mutex.h"
#include "UI/GUICustomDraw.h"

namespace UI
{
	class GUIMapControl : public GUICustomDraw, public Media::IColorHandler
	{
	public:
		typedef void (CALLBACKFUNC MapUpdatedHandler)(AnyType userObj, Math::Coord2DDbl center, Double drawTime);
		typedef void (CALLBACKFUNC ScaleChangedHandler)(AnyType userObj, Double newScale);
		typedef void (CALLBACKFUNC MouseMoveHandler)(AnyType userObj, Math::Coord2D<OSInt> scnPos);
		typedef void (CALLBACKFUNC DrawHandler)(AnyType userObj, NN<Media::DrawImage> dimg, OSInt xOfst, OSInt yOfst);
	private:
		Optional<Media::DrawImage> bgImg;
		NN<Media::ColorManagerSess> colorSess;
		NN<Map::MapView> view;
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
		Math::Coord2D<OSInt> mouseDownPos;
		Math::Coord2D<OSInt> mouseCurrPos;
		Bool gZoom;
		Math::Coord2D<OSInt> gZoomPos;
		UInt64 gZoomDist;
		UInt64 gZoomCurrDist;
		Math::Coord2D<OSInt> gZoomCurrPos;
		Bool pauseUpdate;

		Math::Coord2DDbl markerPos;
		Double markerDir;
		Bool markerHasDir;
		Bool showMarker;
		Data::ArrayListNN<Math::Geometry::Vector2D> selVecList;

		Data::ArrayList<Data::CallbackStorage<ScaleChangedHandler>> scaleChgHdlrs;
		Data::ArrayList<Data::CallbackStorage<MouseMoveHandler>> mouseMoveHdlrs;
		Data::ArrayList<Data::CallbackStorage<MapUpdatedHandler>> mapUpdHdlrs;
		Data::CallbackStorage<MouseEventHandler> mouseDownHdlr;
		Data::CallbackStorage<MouseEventHandler> mouseUpHdlr;
		Data::CallbackStorage<DrawHandler> drawHdlr;
		Sync::Mutex drawMut;
		Int64 imgTimeoutTick;

	private:
		static void __stdcall ImageUpdated(AnyType userObj);

		virtual Bool OnMouseDown(Math::Coord2D<OSInt> scnPos, MouseButton btn);
		virtual Bool OnMouseUp(Math::Coord2D<OSInt> scnPos, MouseButton btn);
		virtual void OnMouseMove(Math::Coord2D<OSInt> scnPos) ;
		virtual Bool OnMouseWheel(Math::Coord2D<OSInt> scnPos, Int32 delta);
		virtual void OnGestureBegin(Math::Coord2D<OSInt> scnPos, UInt64 dist);
		virtual void OnGestureStep(Math::Coord2D<OSInt> scnPos, UInt64 dist);
		virtual void OnGestureEnd(Math::Coord2D<OSInt> scnPos, UInt64 dist);
		virtual void OnJSButtonDown(OSInt buttonId);
		virtual void OnJSButtonUp(OSInt buttonId);
		virtual void OnJSAxis(OSInt axis1, OSInt axis2, OSInt axis3, OSInt axis4);

		virtual void OnTimerTick();
		virtual void OnDraw(NN<Media::DrawImage> img);
		void DrawScnObjects(NN<Media::DrawImage> img, Math::Coord2DDbl ofst);
		void ReleaseSelVecList();
	public:
		/*
		view - will release
		*/
		GUIMapControl(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, UInt32 bgColor, Map::DrawMapRenderer *renderer, NN<Map::MapView> view, NN<Media::ColorManagerSess> colorSess);
		GUIMapControl(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, NN<Map::MapEnv> mapEnv, NN<Media::ColorManagerSess> colorSess);
		virtual ~GUIMapControl();

		virtual void OnSizeChanged(Bool updateScn);

		virtual void YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuvParam);
		virtual void RGBParamChanged(NN<const Media::IColorHandler::RGBPARAM2> rgbParam);

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
		void ShowMarkerMapXY(Math::Coord2DDbl mapPos);
		void ShowMarkerMapXYDir(Math::Coord2DDbl mapPos, Double dir, Math::Unit::Angle::AngleUnit unit);
		void HideMarker();
		void SetSelectedVector(Optional<Math::Geometry::Vector2D> vec);
		void SetSelectedVectors(NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList);
		void SetVAngle(Double angleRad);

		void HandleScaleChanged(ScaleChangedHandler hdlr, AnyType userObj);
		void HandleMapUpdated(MapUpdatedHandler hdlr, AnyType userObj);
		void HandleMouseMove(MouseMoveHandler hdlr, AnyType userObj);
		void HandleMouseUp(MouseEventHandler hdlr, AnyType userObj);
		void HandleMouseDown(MouseEventHandler hdlr, AnyType userObj);
		void HandleCustomDraw(DrawHandler hdlr, AnyType userObj);
		void SetMapUpdated();
		void UpdateMapView(NN<Map::MapView> view);
		NN<Map::MapView> CloneMapView();
		void PauseUpdate(Bool pauseUpdate);
	};
};
#endif
