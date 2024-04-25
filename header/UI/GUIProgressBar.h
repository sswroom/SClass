#ifndef _SM_UI_GUIPROGRESSBAR
#define _SM_UI_GUIPROGRESSBAR
#include "UI/GUIClientControl.h"
#include "IO/ProgressHandler.h"

namespace UI
{
	class GUIProgressBar : public GUIControl, public IO::ProgressHandler
	{
	public:
		GUIProgressBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUIProgressBar();

		virtual Text::CStringNN GetObjectClass() const;
	};
}
#endif
