#ifndef _SM_UI_GUITRACKBAR
#define _SM_UI_GUITRACKBAR
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITrackBar : public GUIControl
	{
	public:
		typedef void (__stdcall *ScrollEvent)(void *userObj, Int32 scrollPos);

	private:
		Data::ArrayList<ScrollEvent> *scrollHandlers;
		Data::ArrayList<void *> *scrollHandlersObj;

	public:
		void EventScrolled();
	public:
		GUITrackBar(GUICore *ui, UI::GUIClientControl *parent, Int32 minVal, Int32 maxVal, Int32 currVal);
		virtual ~GUITrackBar();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		void SetPos(Int32 pos);
		void SetRange(Int32 minVal, Int32 maxVal);
		Int32 GetPos();

		void HandleScrolled(ScrollEvent hdlr, void *userObj);
	};
};
#endif
