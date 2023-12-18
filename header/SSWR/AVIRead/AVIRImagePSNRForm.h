#ifndef _SM_SSWR_AVIREAD_AVIRIMAGEPSNRFORM
#define _SM_SSWR_AVIREAD_AVIRIMAGEPSNRFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRImagePSNRForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			UI::GUILabel *lblOriFile;
			UI::GUITextBox *txtOriFile;
			NotNullPtr<UI::GUIButton> btnOriFile;
			UI::GUILabel *lblLossyFile;
			UI::GUITextBox *txtLossyFile;
			NotNullPtr<UI::GUIButton> btnLossyFile;
			UI::GUILabel *lblMode;
			UI::GUIRadioButton *radMode16Bit;
			UI::GUIRadioButton *radMode8Bit;
			NotNullPtr<UI::GUIButton> btnCompare;
			UI::GUILabel *lblPSNR;
			UI::GUITextBox *txtPSNR;
			UI::GUILabel *lblPSNRDB;

			static void __stdcall OnOriFileClicked(void *userObj);
			static void __stdcall OnLossyFileClicked(void *userObj);
			static void __stdcall OnCompareClicked(void *userObj);
		public:
			AVIRImagePSNRForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRImagePSNRForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
