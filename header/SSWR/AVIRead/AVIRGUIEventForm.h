#ifndef _SM_SSWR_AVIREAD_AVIRGUIEVENTFORM
#define _SM_SSWR_AVIREAD_AVIRGUIEVENTFORM
#include "IO/LogTool.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGUIEventForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUIButton> btnDisplayOff;
			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::LogTool> log;
			NN<UI::ListBoxLogger> logger;

			static void __stdcall OnLogSelChg(AnyType userObj);
			static void __stdcall OnDisplayOffClicked(AnyType userObj);
			static void __stdcall OnKeyDown(AnyType userObj, UIntOS keyCode, Bool extendedKey);
		public:
			AVIRGUIEventForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGUIEventForm();

			virtual Bool OnPaint();
			virtual void OnMonitorChanged();
			virtual void OnFocus();
			virtual void OnFocusLost();
			virtual void OnDisplaySizeChange(UIntOS dispWidth, UIntOS dispHeight);
		};
	}
}
#endif
