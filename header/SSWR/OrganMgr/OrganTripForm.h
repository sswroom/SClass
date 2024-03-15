#ifndef _SM_SSWR_ORGANMGR_ORGANTRIPFORM
#define _SM_SSWR_ORGANMGR_ORGANTRIPFORM

#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIDateTimePicker.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganTripForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUIPanel> pnlDetail;
			NotNullPtr<UI::GUILabel> lblFrom;
			NotNullPtr<UI::GUILabel> lblTo;
			NotNullPtr<UI::GUILabel> lblLocation;
			NotNullPtr<UI::GUIDateTimePicker> dtpFrom;
			NotNullPtr<UI::GUIDateTimePicker> dtpTo;
			NotNullPtr<UI::GUIButton> btnDate1Hr;
			NotNullPtr<UI::GUITextBox> txtLocation;
			NotNullPtr<UI::GUIButton> btnLocation;
			NotNullPtr<UI::GUIButton> btnLocationLast;
			NotNullPtr<UI::GUIButton> btnAdd;
			NotNullPtr<UI::GUIListBox> lbTrips;
			OrganEnv *env;
			Int32 locId;
			Bool updating;
			Data::Timestamp refTime;
			
			void UpdateList();
			static void __stdcall OnTripSelChg(AnyType userObj);
			static void __stdcall OnAddClicked(AnyType userObj);
			static void __stdcall OnLocationClicked(AnyType userObj);
			static void __stdcall OnDate1HrClicked(AnyType userObj);
			static void __stdcall OnLocationLastClicked(AnyType userObj);
		public:
			OrganTripForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env);
			~OrganTripForm();

			virtual void OnMonitorChanged();

			void SetTimes(const Data::Timestamp &refTime, const Data::Timestamp &fromTime, const Data::Timestamp &toTime);
		};
	}
}
#endif