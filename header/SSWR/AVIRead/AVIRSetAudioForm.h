#ifndef _SM_SSWR_AVIREAD_AVIRSETAUDIOFORM
#define _SM_SSWR_AVIREAD_AVIRSETAUDIOFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSetAudioForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblDevice;
			NN<UI::GUIListBox> lbDevice;

			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
		public:
			AVIRSetAudioForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSetAudioForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
