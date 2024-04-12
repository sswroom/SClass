#ifndef _SM_SSWR_ORGANMGR_ORGANSELCATEGORYFORM
#define _SM_SSWR_ORGANMGR_ORGANSELCATEGORYFORM

#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganSelCategoryForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUILabel> lbl;
			NotNullPtr<UI::GUIListBox> lbCategory;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;
			NotNullPtr<SSWR::OrganMgr::OrganEnv> env;
			
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			OrganSelCategoryForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::OrganMgr::OrganEnv> env);
			~OrganSelCategoryForm();

			virtual void OnMonitorChanged();

		private:
			void InitCategory();
		};
	}
}
#endif