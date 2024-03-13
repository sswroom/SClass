#ifndef _SM_SSWR_ORGANMGR_ORGANLOCATIONFORM
#define _SM_SSWR_ORGANMGR_ORGANLOCATIONFORM

#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganLocationForm : public UI::GUIForm
		{
		public:
			typedef enum
			{
				SM_NONE,
				SM_CHILD,
				SM_ANY
			} SelectMode;
		private:
			NotNullPtr<UI::GUIListBox> lbLocation;
			NotNullPtr<UI::GUIListBox> lbSublocations;
			NotNullPtr<UI::GUIPanel> pnlLocation;
			NotNullPtr<UI::GUILabel> lblID;
			NotNullPtr<UI::GUILabel> lblEName;
			NotNullPtr<UI::GUILabel> lblCName;
			NotNullPtr<UI::GUITextBox> txtID;
			NotNullPtr<UI::GUITextBox> txtEName;
			NotNullPtr<UI::GUITextBox> txtCName;
			NotNullPtr<UI::GUIButton> btnAdd;
			NotNullPtr<UI::GUIButton> btnOk;
			NotNullPtr<UI::GUIButton> btnCancel;

			OrganEnv *env;
			SelectMode selMode;
			Int32 initId;
			Bool sublocUpdating;
			Location *currLoc;
			UOSInt currLocInd;
			Location *selVal;
			
			void DispId(Int32 id);
			void UpdateSubloc();
			Bool ToSave();
			Location *GetParentLoc();

			static void __stdcall OnLocSelChg(void *userObj);
			static void __stdcall OnSubLocSelChg(void *userObj);
			static void __stdcall OnSubLocDblClk(void *userObj);
			static void __stdcall OnAddClicked(void *userObj);
			static void __stdcall OnOkClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			OrganLocationForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env, SelectMode selMode, Int32 initId);
			virtual ~OrganLocationForm();

			virtual void OnMonitorChanged();

			Location *GetSelVal();
		};
	}
}
#endif