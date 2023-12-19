#ifndef _SM_SSWR_ORGANMGR_ORGANSEARCHFORM
#define _SM_SSWR_ORGANMGR_ORGANSEARCHFORM

#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganSearchForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUILabel> lbl;
			NotNullPtr<UI::GUITextBox> txt;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;
			OrganEnv *env;
			Text::String *foundStr;
			OrganGroup *foundGroup;
			Int32 parentId;
			
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			OrganSearchForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env);
			virtual ~OrganSearchForm();

			virtual void OnMonitorChanged();

			Text::String *GetFoundStr();
			OrganGroup *GetFoundGroup();
			Int32 GetParentId();
		};
	}
}
#endif
