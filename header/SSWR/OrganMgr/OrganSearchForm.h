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
			UI::GUILabel *lbl;
			UI::GUITextBox *txt;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;
			OrganEnv *env;
			const UTF8Char *foundStr;
			OrganGroup *foundGroup;
			Int32 parentId;
			
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			OrganSearchForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env);
			virtual ~OrganSearchForm();

			virtual void OnMonitorChanged();

			const UTF8Char *GetFoundStr();
			OrganGroup *GetFoundGroup();
			Int32 GetParentId();
		};
	}
}
#endif
