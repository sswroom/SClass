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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::ImageList *simg;

			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblType;
			UI::GUIComboBox *cboType;
			UI::GUIButton *btnOk;
			UI::GUIButton *btnCancel;
			UI::GUIPictureBox *pbImage;

			static void __stdcall OnOkClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
			static void __stdcall OnTypeSelChg(void *userObj);

		public:
			AVIRSNBHandlerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::SNBDongle::HandleType handType);
			virtual ~AVIRSNBHandlerForm();

			virtual void OnMonitorChanged();

			IO::SNBDongle::HandleType GetHandleType();
		};
	};
};
#endif
