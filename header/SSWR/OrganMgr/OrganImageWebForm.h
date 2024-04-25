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
			NN<OrganEnv> env;
			Text::String *srcURL;
			Text::String *location;

			NN<UI::GUILabel> lblId;
			NN<UI::GUITextBox> txtId;
			NN<UI::GUILabel> lblFileName;
			NN<UI::GUITextBox> txtFileName;
			NN<UI::GUILabel> lblImageURL;
			NN<UI::GUITextBox> txtImageURL;
			NN<UI::GUILabel> lblSourceURL;
			NN<UI::GUITextBox> txtSourceURL;
			NN<UI::GUILabel> lblLocation;
			NN<UI::GUITextBox> txtLocation;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			OrganImageWebForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<OrganEnv> env, NN<OrganImageItem> userFile, NN<SSWR::OrganMgr::WebFileInfo> wfile);
			virtual ~OrganImageWebForm();

			virtual void OnMonitorChanged();

			Text::String *GetSrcURL();
			Text::String *GetLocation();
		};
	}
}
#endif
