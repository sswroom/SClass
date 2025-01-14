#ifndef _SM_SSWR_AVIREAD_AVIRWIADEVFORM
#define _SM_SSWR_AVIREAD_AVIRWIADEVFORM
#include "Media/WIAManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWIADevForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::WIAManager> wiaMgr;

			NN<UI::GUILabel> lblDevice;
			NN<UI::GUIListBox> lbDevice;

			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);

		public:
			AVIRWIADevForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWIADevForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
