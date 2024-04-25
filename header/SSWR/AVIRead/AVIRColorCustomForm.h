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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::MonitorColorManager> monColor;

			NN<UI::GUILabel> lblRX;
			NN<UI::GUITextBox> txtRX;
			NN<UI::GUIButton> btnRXDown;
			NN<UI::GUIButton> btnRXUp;
			NN<UI::GUILabel> lblRY;
			NN<UI::GUITextBox> txtRY;
			NN<UI::GUIButton> btnRYDown;
			NN<UI::GUIButton> btnRYUp;
			NN<UI::GUILabel> lblGX;
			NN<UI::GUITextBox> txtGX;
			NN<UI::GUIButton> btnGXDown;
			NN<UI::GUIButton> btnGXUp;
			NN<UI::GUILabel> lblGY;
			NN<UI::GUITextBox> txtGY;
			NN<UI::GUIButton> btnGYDown;
			NN<UI::GUIButton> btnGYUp;
			NN<UI::GUILabel> lblBX;
			NN<UI::GUITextBox> txtBX;
			NN<UI::GUIButton> btnBXDown;
			NN<UI::GUIButton> btnBXUp;
			NN<UI::GUILabel> lblBY;
			NN<UI::GUITextBox> txtBY;
			NN<UI::GUIButton> btnBYDown;
			NN<UI::GUIButton> btnBYUp;
			NN<UI::GUILabel> lblWX;
			NN<UI::GUITextBox> txtWX;
			NN<UI::GUIButton> btnWXDown;
			NN<UI::GUIButton> btnWXUp;
			NN<UI::GUILabel> lblWY;
			NN<UI::GUITextBox> txtWY;
			NN<UI::GUIButton> btnWYDown;
			NN<UI::GUIButton> btnWYUp;
			NN<UI::GUIButton> btnClose;

			static void __stdcall OnRXDownClicked(AnyType userObj);
			static void __stdcall OnRXUpClicked(AnyType userObj);
			static void __stdcall OnRYDownClicked(AnyType userObj);
			static void __stdcall OnRYUpClicked(AnyType userObj);
			static void __stdcall OnGXDownClicked(AnyType userObj);
			static void __stdcall OnGXUpClicked(AnyType userObj);
			static void __stdcall OnGYDownClicked(AnyType userObj);
			static void __stdcall OnGYUpClicked(AnyType userObj);
			static void __stdcall OnBXDownClicked(AnyType userObj);
			static void __stdcall OnBXUpClicked(AnyType userObj);
			static void __stdcall OnBYDownClicked(AnyType userObj);
			static void __stdcall OnBYUpClicked(AnyType userObj);
			static void __stdcall OnWXDownClicked(AnyType userObj);
			static void __stdcall OnWXUpClicked(AnyType userObj);
			static void __stdcall OnWYDownClicked(AnyType userObj);
			static void __stdcall OnWYUpClicked(AnyType userObj);
			static void __stdcall OnCloseClicked(AnyType userObj);
		public:
			AVIRColorCustomForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::MonitorColorManager> monColor);
			virtual ~AVIRColorCustomForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
