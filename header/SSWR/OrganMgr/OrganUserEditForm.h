#ifndef _SM_SSWR_ORGANMGR_ORGANUSEREDITFORM
#define _SM_SSWR_ORGANMGR_ORGANUSEREDITFORM

#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganUserEditForm : public UI::GUIForm
		{
		private:
			NN<OrganEnv> env;
			Optional<OrganWebUser> user;
			NN<UI::GUILabel> lblUserName;
			NN<UI::GUITextBox> txtUserName;
			NN<UI::GUILabel> lblPassword;
			NN<UI::GUITextBox> txtPassword;
			NN<UI::GUILabel> lblRetype;
			NN<UI::GUITextBox> txtRetype;
			NN<UI::GUILabel> lblWatermark;
			NN<UI::GUITextBox> txtWatermark;

			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			OrganUserEditForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<OrganEnv> env, Optional<OrganWebUser> user);
			virtual ~OrganUserEditForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
