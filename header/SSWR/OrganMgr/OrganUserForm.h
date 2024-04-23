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
			NotNullPtr<OrganEnv> env;
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIButton> btnAdd;
			NotNullPtr<UI::GUIButton> btnModify;
			NotNullPtr<UI::GUIListView> lvUser;
			Data::ArrayListNN<OrganWebUser> userList;

			static void __stdcall OnAddClicked(AnyType userObj);
			static void __stdcall OnModifyClicked(AnyType userObj);
			void UpdateUserList();
		public:
			OrganUserForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<OrganEnv> env);
			virtual ~OrganUserForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
