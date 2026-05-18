#ifndef _SM_SSWR_AVIREAD_AVIRKEYTESTFORM
#define _SM_SSWR_AVIREAD_AVIRKEYTESTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRKeyTestForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblKeyInput;
			NN<UI::GUITextBox> txtKeyInput;
			NN<UI::GUILabel> lblGUIKey;
			NN<UI::GUITextBox> txtGUIKey;
			NN<UI::GUILabel> lblOSKey;
			NN<UI::GUITextBox> txtOSKey;
			NN<UI::GUILabel> lblOSKeyHex;
			NN<UI::GUITextBox> txtOSKeyHex;
		
			static UI::EventState __stdcall OnKeyInputKeyDown(AnyType userObj, UInt32 osKey);
		public:
			AVIRKeyTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRKeyTestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
