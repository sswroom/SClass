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
			NN<OrganEnv> env;
			const UTF8Char *descript;

			NotNullPtr<UI::GUILabel> lblId;
			NotNullPtr<UI::GUITextBox> txtId;
			NotNullPtr<UI::GUILabel> lblFileName;
			NotNullPtr<UI::GUITextBox> txtFileName;
			NotNullPtr<UI::GUILabel> lblDescript;
			NotNullPtr<UI::GUITextBox> txtDescript;
			NotNullPtr<UI::GUILabel> lblCamera;
			NotNullPtr<UI::GUITextBox> txtCamera;
			NotNullPtr<UI::GUILabel> lblFileTime;
			NotNullPtr<UI::GUITextBox> txtFileTime;
			NotNullPtr<UI::GUILabel> lblLat;
			NotNullPtr<UI::GUITextBox> txtLat;
			NotNullPtr<UI::GUILabel> lblLon;
			NotNullPtr<UI::GUITextBox> txtLon;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			OrganImageDetailForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<OrganEnv> env, NN<UserFileInfo> userFile);
			virtual ~OrganImageDetailForm();

			virtual void OnMonitorChanged();

			const UTF8Char *GetDescript();
		};
	}
}
#endif
