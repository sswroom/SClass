#ifndef _SM_UI_MSWINDOWDOBJAREA
#define _SM_UI_MSWINDOWDOBJAREA
#include "Media/ColorManager.h"
#include "Media/DrawEngine.h"
#include "Sync/Mutex.h"
#include "UI/GUIDDrawControl.h"
#include "UI/DObj/DObjHandler.h"

namespace UI
{
	class GUIDObjArea : public GUIDDrawControl
	{
	private:
		Media::DrawEngine *deng;
		Media::DrawImage *currDrawImg;
		Media::ColorManagerSess *colorSess;
		Bool drawUpdated;
		Sync::Mutex *dobjMut;
		UI::DObj::DObjHandler *dobjHdlr;
		Bool displayToStop;
		Bool displayRunning;
		Sync::Event *displayEvt;
		Bool processToStop;
		Bool processRunning;
		Sync::Event *processEvt;
		Sync::Event *mainEvt;

		static UInt32 __stdcall DisplayThread(void *userObj);
		static UInt32 __stdcall ProcessThread(void *userObj);
	public:
		GUIDObjArea(GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *deng, Media::ColorManagerSess *colorSess);
		virtual ~GUIDObjArea();

		void SetHandler(UI::DObj::DObjHandler *dobjHdlr);

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

	protected:
		virtual void OnSurfaceCreated();
		virtual void OnMouseWheel(OSInt x, OSInt y, Int32 amount);
		virtual void OnMouseMove(OSInt x, OSInt y);
		virtual void OnMouseDown(OSInt x, OSInt y, MouseButton button);
		virtual void OnMouseUp(OSInt x, OSInt y, MouseButton button);
		virtual void OnMouseDblClick(OSInt x, OSInt y, MouseButton button);
	};
};
#endif
