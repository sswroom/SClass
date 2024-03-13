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
			NotNullPtr<UI::GUILabel> lblOriFile;
			NotNullPtr<UI::GUITextBox> txtOriFile;
			NotNullPtr<UI::GUIButton> btnOriFile;
			NotNullPtr<UI::GUILabel> lblLossyFile;
			NotNullPtr<UI::GUITextBox> txtLossyFile;
			NotNullPtr<UI::GUIButton> btnLossyFile;
			NotNullPtr<UI::GUILabel> lblMode;
			NotNullPtr<UI::GUIRadioButton> radMode16Bit;
			NotNullPtr<UI::GUIRadioButton> radMode8Bit;
			NotNullPtr<UI::GUIButton> btnCompare;
			NotNullPtr<UI::GUILabel> lblPSNR;
			NotNullPtr<UI::GUITextBox> txtPSNR;
			NotNullPtr<UI::GUILabel> lblPSNRDB;

			static void __stdcall OnOriFileClicked(void *userObj);
			static void __stdcall OnLossyFileClicked(void *userObj);
			static void __stdcall OnCompareClicked(void *userObj);
		public:
			AVIRImagePSNRForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRImagePSNRForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
