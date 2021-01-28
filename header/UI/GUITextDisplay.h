#ifndef _SM_UI_GUITEXTDISPLAY
#define _SM_UI_GUITEXTDISPLAY
#include "UI/GUIDDrawControl.h"

namespace UI
{
	class GUITextDisplay : public UI::GUIDDrawControl
	{
	private:
		const UTF8Char *currText;
		Media::DrawEngine *eng;

	public:
		GUITextDisplay(GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *eng);
		virtual ~GUITextDisplay();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(Int32 code, void *lParam);
		virtual void OnSurfaceCreated();
		void SetDispText(const UTF8Char *dispText);

	protected:
		void UpdateScreen();
	};
}
#endif
