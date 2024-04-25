#ifndef _SM_SSWR_AVIREAD_AVIRSNBHANDLERFORM
#define _SM_SSWR_AVIREAD_AVIRSNBHANDLERFORM
#include "IO/SNBDongle.h"
#include "Media/ImageList.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSNBHandlerForm : public UI::GUIForm
		{
		private:
			IO::SNBDongle::HandleType handType;
			NN<SSWR::AVIRead::AVIRCore> core;
			Media::ImageList *simg;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblType;
			NN<UI::GUIComboBox> cboType;
			NN<UI::GUIButton> btnOk;
			NN<UI::GUIButton> btnCancel;
			NN<UI::GUIPictureBox> pbImage;

			static void __stdcall OnOkClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall OnTypeSelChg(AnyType userObj);

		public:
			AVIRSNBHandlerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, IO::SNBDongle::HandleType handType);
			virtual ~AVIRSNBHandlerForm();

			virtual void OnMonitorChanged();

			IO::SNBDongle::HandleType GetHandleType();
		};
	};
};
#endif
