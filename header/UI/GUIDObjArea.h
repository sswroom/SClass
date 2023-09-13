#ifndef _SM_UI_MSWINDOWDOBJAREA
#define _SM_UI_MSWINDOWDOBJAREA
#include "Media/ColorManager.h"
#include "Media/DrawEngine.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "UI/GUIDDrawControl.h"
#include "UI/DObj/DObjHandler.h"

namespace UI
{
	class GUIDObjArea : public GUIDDrawControl
	{
	private:
		NotNullPtr<Media::DrawEngine> deng;
		Media::DrawImage *currDrawImg;
		NotNullPtr<Media::ColorManagerSess> colorSess;
		Bool drawUpdated;
		Sync::Mutex dobjMut;
		UI::DObj::DObjHandler *dobjHdlr;
		Sync::Thread displayThread;
		Sync::Thread processThread;
		Sync::Event mainEvt;

		static void __stdcall DisplayThread(NotNullPtr<Sync::Thread> thread);
		static void __stdcall ProcessThread(NotNullPtr<Sync::Thread> thread);
		static void __stdcall OnUpdateSize(void *userObj);
	public:
		GUIDObjArea(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, NotNullPtr<Media::DrawEngine> deng, NotNullPtr<Media::ColorManagerSess> colorSess);
		virtual ~GUIDObjArea();

		void SetHandler(UI::DObj::DObjHandler *dobjHdlr);

		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);

	protected:
		virtual void OnSurfaceCreated();
		virtual void OnMouseWheel(Math::Coord2D<OSInt> pos, Int32 amount);
		virtual void OnMouseMove(Math::Coord2D<OSInt> pos);
		virtual void OnMouseDown(Math::Coord2D<OSInt> pos, MouseButton button);
		virtual void OnMouseUp(Math::Coord2D<OSInt> pos, MouseButton button);
		virtual void OnMouseDblClick(Math::Coord2D<OSInt> pos, MouseButton button);
	};
};
#endif
