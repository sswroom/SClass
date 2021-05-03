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
			UI::GUIPanel *pnlCtrl;
			UI::GUIButton *btnDelete;
			UI::GUIButton *btnStartTime;

			static void __stdcall OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles);
			static void __stdcall OnFilesDblClk(void *userObj, OSInt itemIndex);
			static void __stdcall OnDeleteClicked(void *userObj);
			static void __stdcall OnStartTimeClicked(void *userObj);
			void UpdateFileList();
		public:
			OrganDataFileForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env);
			virtual ~OrganDataFileForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif