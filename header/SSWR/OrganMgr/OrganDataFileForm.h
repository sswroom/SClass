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
			NN<OrganEnv> env;
			NN<UI::GUIListView> lvFiles;
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIButton> btnDelete;
			NN<UI::GUIButton> btnStartTime;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnFilesDblClk(AnyType userObj, UIntOS itemIndex);
			static void __stdcall OnDeleteClicked(AnyType userObj);
			static void __stdcall OnStartTimeClicked(AnyType userObj);
			void UpdateFileList();
		public:
			OrganDataFileForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<OrganEnv> env);
			virtual ~OrganDataFileForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif