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
			OrganEnv *env;
			OrganWebUser *user;
			UI::GUILabel *lblUserName;
			UI::GUITextBox *txtUserName;
			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUILabel *lblRetype;
			UI::GUITextBox *txtRetype;
			UI::GUILabel *lblWatermark;
			UI::GUITextBox *txtWatermark;

			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			OrganUserEditForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env, OrganWebUser *user);
			virtual ~OrganUserEditForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
