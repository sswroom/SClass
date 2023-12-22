#ifndef _SM_SSWR_ORGANMGR_ORGANUSERFORM
#define _SM_SSWR_ORGANMGR_ORGANUSERFORM

#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganUserForm : public UI::GUIForm
		{
		private:
			OrganEnv *env;
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIButton> btnAdd;
			NotNullPtr<UI::GUIButton> btnModify;
			NotNullPtr<UI::GUIListView> lvUser;
			Data::ArrayList<OrganWebUser*> userList;

			static void __stdcall OnAddClicked(void *userObj);
			static void __stdcall OnModifyClicked(void *userObj);
			void UpdateUserList();
		public:
			OrganUserForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env);
			virtual ~OrganUserForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
