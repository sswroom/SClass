#ifndef _SM_SSWR_ORGANMGR_ORGANDATAFILEFORM
#define _SM_SSWR_ORGANMGR_ORGANDATAFILEFORM

#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganDataFileForm : public UI::GUIForm
		{
		private:
			NotNullPtr<OrganEnv> env;
			NotNullPtr<UI::GUIListView> lvFiles;
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIButton> btnDelete;
			NotNullPtr<UI::GUIButton> btnStartTime;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
			static void __stdcall OnFilesDblClk(AnyType userObj, UOSInt itemIndex);
			static void __stdcall OnDeleteClicked(AnyType userObj);
			static void __stdcall OnStartTimeClicked(AnyType userObj);
			void UpdateFileList();
		public:
			OrganDataFileForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<OrganEnv> env);
			virtual ~OrganDataFileForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif