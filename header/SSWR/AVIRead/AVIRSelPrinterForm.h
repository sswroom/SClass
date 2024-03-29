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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<UI::GUILabel> lblPrinter;
			NotNullPtr<UI::GUIComboBox> cboPrinter;
			NotNullPtr<UI::GUIButton> btnSetting;

			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnSettingClick(AnyType userObj);
			static void __stdcall OnPrinterChg(AnyType userObj);
			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
		public:
			AVIRSelPrinterForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSelPrinterForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
