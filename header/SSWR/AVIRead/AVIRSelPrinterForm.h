#ifndef _SM_SSWR_AVIREAD_AVIRSELPRINTERFORM
#define _SM_SSWR_AVIREAD_AVIRSELPRINTERFORM
#include "Media/Printer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSelPrinterForm : public UI::GUIForm
		{
		public:
			Media::Printer *printer;
		private:
			Media::Printer *currPrinter;
			SSWR::AVIRead::AVIRCore *core;
			UI::GUILabel *lblPrinter;
			UI::GUIComboBox *cboPrinter;
			UI::GUIButton *btnSetting;

			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnSettingClick(void *userObj);
			static void __stdcall OnPrinterChg(void *userObj);
			static void __stdcall OnOKClick(void *userObj);
			static void __stdcall OnCancelClick(void *userObj);
		public:
			AVIRSelPrinterForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSelPrinterForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
