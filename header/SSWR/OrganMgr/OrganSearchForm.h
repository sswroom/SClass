#ifndef _SM_SSWR_ORGANMGR_ORGANSEARCHFORM
#define _SM_SSWR_ORGANMGR_ORGANSEARCHFORM

#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganSearchForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lbl;
			NN<UI::GUITextBox> txt;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;
			NN<OrganEnv> env;
			Text::String *foundStr;
			OrganGroup *foundGroup;
			Int32 parentId;
			
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			OrganSearchForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<OrganEnv> env);
			virtual ~OrganSearchForm();

			virtual void OnMonitorChanged();

			NN<Text::String> GetFoundStr();
			NN<OrganGroup> GetFoundGroup();
			Int32 GetParentId();
		};
	}
}
#endif
