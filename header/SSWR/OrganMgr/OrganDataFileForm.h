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
			OrganEnv *env;
			UI::GUIListView *lvFiles;
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIButton> btnDelete;
			NotNullPtr<UI::GUIButton> btnStartTime;

			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnFilesDblClk(void *userObj, UOSInt itemIndex);
			static void __stdcall OnDeleteClicked(void *userObj);
			static void __stdcall OnStartTimeClicked(void *userObj);
			void UpdateFileList();
		public:
			OrganDataFileForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env);
			virtual ~OrganDataFileForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif