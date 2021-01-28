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
			UI::GUIPanel *pnlDetail;
			UI::GUILabel *lblFrom;
			UI::GUILabel *lblTo;
			UI::GUILabel *lblLocation;
			UI::GUIDateTimePicker *dtpFrom;
			UI::GUIDateTimePicker *dtpTo;
			UI::GUITextBox *txtLocation;
			UI::GUIButton *btnLocation;
			UI::GUIButton *btnAdd;
			UI::GUIListBox *lbTrips;
			OrganEnv *env;
			Int32 locId;
			Bool updating;
			
			void UpdateList();
			static void __stdcall OnTripSelChg(void *userObj);
			static void __stdcall OnAddClicked(void *userObj);
			static void __stdcall OnLocationClicked(void *userObj);
		public:
			OrganTripForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env);
			~OrganTripForm();

			virtual void OnMonitorChanged();

			void SetTimes(Data::DateTime *fromTime, Data::DateTime *toTime);
		};
	}
}
#endif