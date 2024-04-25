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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUILabel> lblOriFile;
			NN<UI::GUITextBox> txtOriFile;
			NN<UI::GUIButton> btnOriFile;
			NN<UI::GUILabel> lblLossyFile;
			NN<UI::GUITextBox> txtLossyFile;
			NN<UI::GUIButton> btnLossyFile;
			NN<UI::GUILabel> lblMode;
			NN<UI::GUIRadioButton> radMode16Bit;
			NN<UI::GUIRadioButton> radMode8Bit;
			NN<UI::GUIButton> btnCompare;
			NN<UI::GUILabel> lblPSNR;
			NN<UI::GUITextBox> txtPSNR;
			NN<UI::GUILabel> lblPSNRDB;

			static void __stdcall OnOriFileClicked(AnyType userObj);
			static void __stdcall OnLossyFileClicked(AnyType userObj);
			static void __stdcall OnCompareClicked(AnyType userObj);
		public:
			AVIRImagePSNRForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRImagePSNRForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
