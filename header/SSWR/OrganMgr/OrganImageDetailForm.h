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

			NN<UI::GUILabel> lblId;
			NN<UI::GUITextBox> txtId;
			NN<UI::GUILabel> lblFileName;
			NN<UI::GUITextBox> txtFileName;
			NN<UI::GUILabel> lblDescript;
			NN<UI::GUITextBox> txtDescript;
			NN<UI::GUILabel> lblCamera;
			NN<UI::GUITextBox> txtCamera;
			NN<UI::GUILabel> lblFileTime;
			NN<UI::GUITextBox> txtFileTime;
			NN<UI::GUILabel> lblLat;
			NN<UI::GUITextBox> txtLat;
			NN<UI::GUILabel> lblLon;
			NN<UI::GUITextBox> txtLon;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

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
