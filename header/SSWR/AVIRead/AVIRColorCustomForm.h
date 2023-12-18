#ifndef _SM_SSWR_AVIREAD_AVIRCOLORCUSTOMFORM
#define _SM_SSWR_AVIREAD_AVIRCOLORCUSTOMFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHScrollBar.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRColorCustomForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::MonitorColorManager> monColor;

			NotNullPtr<UI::GUILabel> lblRX;
			UI::GUITextBox *txtRX;
			NotNullPtr<UI::GUIButton> btnRXDown;
			NotNullPtr<UI::GUIButton> btnRXUp;
			NotNullPtr<UI::GUILabel> lblRY;
			UI::GUITextBox *txtRY;
			NotNullPtr<UI::GUIButton> btnRYDown;
			NotNullPtr<UI::GUIButton> btnRYUp;
			NotNullPtr<UI::GUILabel> lblGX;
			UI::GUITextBox *txtGX;
			NotNullPtr<UI::GUIButton> btnGXDown;
			NotNullPtr<UI::GUIButton> btnGXUp;
			NotNullPtr<UI::GUILabel> lblGY;
			UI::GUITextBox *txtGY;
			NotNullPtr<UI::GUIButton> btnGYDown;
			NotNullPtr<UI::GUIButton> btnGYUp;
			NotNullPtr<UI::GUILabel> lblBX;
			UI::GUITextBox *txtBX;
			NotNullPtr<UI::GUIButton> btnBXDown;
			NotNullPtr<UI::GUIButton> btnBXUp;
			NotNullPtr<UI::GUILabel> lblBY;
			UI::GUITextBox *txtBY;
			NotNullPtr<UI::GUIButton> btnBYDown;
			NotNullPtr<UI::GUIButton> btnBYUp;
			NotNullPtr<UI::GUILabel> lblWX;
			UI::GUITextBox *txtWX;
			NotNullPtr<UI::GUIButton> btnWXDown;
			NotNullPtr<UI::GUIButton> btnWXUp;
			NotNullPtr<UI::GUILabel> lblWY;
			UI::GUITextBox *txtWY;
			NotNullPtr<UI::GUIButton> btnWYDown;
			NotNullPtr<UI::GUIButton> btnWYUp;
			NotNullPtr<UI::GUIButton> btnClose;

			static void __stdcall OnRXDownClicked(void *userObj);
			static void __stdcall OnRXUpClicked(void *userObj);
			static void __stdcall OnRYDownClicked(void *userObj);
			static void __stdcall OnRYUpClicked(void *userObj);
			static void __stdcall OnGXDownClicked(void *userObj);
			static void __stdcall OnGXUpClicked(void *userObj);
			static void __stdcall OnGYDownClicked(void *userObj);
			static void __stdcall OnGYUpClicked(void *userObj);
			static void __stdcall OnBXDownClicked(void *userObj);
			static void __stdcall OnBXUpClicked(void *userObj);
			static void __stdcall OnBYDownClicked(void *userObj);
			static void __stdcall OnBYUpClicked(void *userObj);
			static void __stdcall OnWXDownClicked(void *userObj);
			static void __stdcall OnWXUpClicked(void *userObj);
			static void __stdcall OnWYDownClicked(void *userObj);
			static void __stdcall OnWYUpClicked(void *userObj);
			static void __stdcall OnCloseClicked(void *userObj);
		public:
			AVIRColorCustomForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Media::MonitorColorManager> monColor);
			virtual ~AVIRColorCustomForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
