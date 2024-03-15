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

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUILabel> lblType;
			NotNullPtr<UI::GUIComboBox> cboType;
			NotNullPtr<UI::GUIButton> btnOk;
			NotNullPtr<UI::GUIButton> btnCancel;
			NotNullPtr<UI::GUIPictureBox> pbImage;

			static void __stdcall OnOkClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall OnTypeSelChg(AnyType userObj);

		public:
			AVIRSNBHandlerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::SNBDongle::HandleType handType);
			virtual ~AVIRSNBHandlerForm();

			virtual void OnMonitorChanged();

			IO::SNBDongle::HandleType GetHandleType();
		};
	};
};
#endif
