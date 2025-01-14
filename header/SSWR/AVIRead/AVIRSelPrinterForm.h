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
			Optional<Media::Printer> printer;
		private:
			Optional<Media::Printer> currPrinter;
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUILabel> lblPrinter;
			NN<UI::GUIComboBox> cboPrinter;
			NN<UI::GUIButton> btnSetting;

			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnSettingClick(AnyType userObj);
			static void __stdcall OnPrinterChg(AnyType userObj);
			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
		public:
			AVIRSelPrinterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSelPrinterForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
