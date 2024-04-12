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
			NotNullPtr<OrganEnv> env;
			Optional<OrganWebUser> user;
			NotNullPtr<UI::GUILabel> lblUserName;
			NotNullPtr<UI::GUITextBox> txtUserName;
			NotNullPtr<UI::GUILabel> lblPassword;
			NotNullPtr<UI::GUITextBox> txtPassword;
			NotNullPtr<UI::GUILabel> lblRetype;
			NotNullPtr<UI::GUITextBox> txtRetype;
			NotNullPtr<UI::GUILabel> lblWatermark;
			NotNullPtr<UI::GUITextBox> txtWatermark;

			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			OrganUserEditForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<OrganEnv> env, Optional<OrganWebUser> user);
			virtual ~OrganUserEditForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
