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
			UI::GUIPanel *pnlCtrl;
			UI::GUIButton *btnAdd;
			UI::GUIButton *btnModify;
			UI::GUIListView *lvUser;
			Data::ArrayList<OrganWebUser*> *userList;

			static void __stdcall OnAddClicked(void *userObj);
			static void __stdcall OnModifyClicked(void *userObj);
			void UpdateUserList();
		public:
			OrganUserForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env);
			virtual ~OrganUserForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
