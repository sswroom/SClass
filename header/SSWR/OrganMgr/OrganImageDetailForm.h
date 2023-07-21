#ifndef _SM_SSWR_ORGANMGR_ORGANIMAGEDETAILFORM
#define _SM_SSWR_ORGANMGR_ORGANIMAGEDETAILFORM

#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganImageDetailForm : public UI::GUIForm
		{
		private:
			OrganEnv *env;
			const UTF8Char *descript;

			UI::GUILabel *lblId;
			UI::GUITextBox *txtId;
			UI::GUILabel *lblFileName;
			UI::GUITextBox *txtFileName;
			UI::GUILabel *lblDescript;
			UI::GUITextBox *txtDescript;
			UI::GUILabel *lblCamera;
			UI::GUITextBox *txtCamera;
			UI::GUILabel *lblFileTime;
			UI::GUITextBox *txtFileTime;
			UI::GUILabel *lblLat;
			UI::GUITextBox *txtLat;
			UI::GUILabel *lblLon;
			UI::GUITextBox *txtLon;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			OrganImageDetailForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env, UserFileInfo *userFile);
			virtual ~OrganImageDetailForm();

			virtual void OnMonitorChanged();

			const UTF8Char *GetDescript();
		};
	}
}
#endif
