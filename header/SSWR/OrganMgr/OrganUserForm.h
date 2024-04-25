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
			NN<OrganEnv> env;
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIButton> btnAdd;
			NN<UI::GUIButton> btnModify;
			NN<UI::GUIListView> lvUser;
			Data::ArrayListNN<OrganWebUser> userList;

			static void __stdcall OnAddClicked(AnyType userObj);
			static void __stdcall OnModifyClicked(AnyType userObj);
			void UpdateUserList();
		public:
			OrganUserForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<OrganEnv> env);
			virtual ~OrganUserForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
