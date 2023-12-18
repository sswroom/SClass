#ifndef _SM_SSWR_AVIREAD_AVIROTPFORM
#define _SM_SSWR_AVIREAD_AVIROTPFORM
#include "Crypto/OTP.h"
#include "Data/ArrayList.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIROTPForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				NotNullPtr<Text::String> name;
				Crypto::OTP *otp;
				UInt64 lastCounter;
			} EntryInfo;
			
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayList<EntryInfo*> *entryList;

			NotNullPtr<UI::GUIGroupBox> grpNew;
			UI::GUILabel *lblName;
			UI::GUITextBox *txtName;
			UI::GUILabel *lblKey;
			UI::GUITextBox *txtKey;
			NotNullPtr<UI::GUIButton> btnKeyRand80;
			NotNullPtr<UI::GUIButton> btnKeyRand160;
			UI::GUILabel *lblType;
			UI::GUIComboBox *cboType;
			NotNullPtr<UI::GUIButton> btnNew;
			UI::GUIListView *lvEntry;

			void RandBytes(UOSInt len);
			static void __stdcall OnKeyRand80Clicked(void *userObj);
			static void __stdcall OnKeyRand160Clicked(void *userObj);
			static void __stdcall OnNewClicked(void *userObj);
			static void __stdcall OnEntryDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIROTPForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROTPForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
