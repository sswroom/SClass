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
			UI::GUIPanel *pnlMain;
			UI::GUIButton *btnDisplayOff;
			NotNullPtr<UI::GUIListBox> lbLog;
			UI::GUITextBox *txtLog;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::LogTool *log;
			NotNullPtr<UI::ListBoxLogger> logger;

			static void __stdcall OnLogSelChg(void *userObj);
			static void __stdcall OnDisplayOffClicked(void *userObj);
			static void __stdcall OnKeyDown(void *userObj, UOSInt keyCode, Bool extendedKey);
		public:
			AVIRGUIEventForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGUIEventForm();

			virtual Bool OnPaint();
			virtual void OnMonitorChanged();
			virtual void OnFocus();
			virtual void OnFocusLost();
			virtual void OnDisplaySizeChange(UOSInt dispWidth, UOSInt dispHeight);
		};
	};
};
#endif
