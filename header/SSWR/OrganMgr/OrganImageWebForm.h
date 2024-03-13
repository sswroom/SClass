#ifndef _SM_SSWR_ORGANMGR_ORGANIMAGEWEBFORM
#define _SM_SSWR_ORGANMGR_ORGANIMAGEWEBFORM

#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganImageWebForm : public UI::GUIForm
		{
		private:
			OrganEnv *env;
			Text::String *srcURL;
			Text::String *location;

			NotNullPtr<UI::GUILabel> lblId;
			NotNullPtr<UI::GUITextBox> txtId;
			NotNullPtr<UI::GUILabel> lblFileName;
			NotNullPtr<UI::GUITextBox> txtFileName;
			NotNullPtr<UI::GUILabel> lblImageURL;
			NotNullPtr<UI::GUITextBox> txtImageURL;
			NotNullPtr<UI::GUILabel> lblSourceURL;
			NotNullPtr<UI::GUITextBox> txtSourceURL;
			NotNullPtr<UI::GUILabel> lblLocation;
			NotNullPtr<UI::GUITextBox> txtLocation;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			OrganImageWebForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env, OrganImageItem *userFile, SSWR::OrganMgr::WebFileInfo *wfile);
			virtual ~OrganImageWebForm();

			virtual void OnMonitorChanged();

			Text::String *GetSrcURL();
			Text::String *GetLocation();
		};
	}
}
#endif
