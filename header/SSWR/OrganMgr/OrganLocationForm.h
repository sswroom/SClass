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
			NN<UI::GUIListBox> lbLocation;
			NN<UI::GUIListBox> lbSublocations;
			NN<UI::GUIPanel> pnlLocation;
			NN<UI::GUILabel> lblID;
			NN<UI::GUILabel> lblEName;
			NN<UI::GUILabel> lblCName;
			NN<UI::GUITextBox> txtID;
			NN<UI::GUITextBox> txtEName;
			NN<UI::GUITextBox> txtCName;
			NN<UI::GUIButton> btnAdd;
			NN<UI::GUIButton> btnOk;
			NN<UI::GUIButton> btnCancel;

			NN<OrganEnv> env;
			SelectMode selMode;
			Int32 initId;
			Bool sublocUpdating;
			Location *currLoc;
			UIntOS currLocInd;
			Location *selVal;
			
			void DispId(Int32 id);
			void UpdateSubloc();
			Bool ToSave();
			Location *GetParentLoc();

			static void __stdcall OnLocSelChg(AnyType userObj);
			static void __stdcall OnSubLocSelChg(AnyType userObj);
			static void __stdcall OnSubLocDblClk(AnyType userObj);
			static void __stdcall OnAddClicked(AnyType userObj);
			static void __stdcall OnOkClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			OrganLocationForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<OrganEnv> env, SelectMode selMode, Int32 initId);
			virtual ~OrganLocationForm();

			virtual void OnMonitorChanged();

			Location *GetSelVal();
		};
	}
}
#endif