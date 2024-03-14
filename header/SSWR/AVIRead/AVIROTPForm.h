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
			NotNullPtr<UI::GUILabel> lblName;
			NotNullPtr<UI::GUITextBox> txtName;
			NotNullPtr<UI::GUILabel> lblKey;
			NotNullPtr<UI::GUITextBox> txtKey;
			NotNullPtr<UI::GUIButton> btnKeyRand80;
			NotNullPtr<UI::GUIButton> btnKeyRand160;
			NotNullPtr<UI::GUILabel> lblType;
			NotNullPtr<UI::GUIComboBox> cboType;
			NotNullPtr<UI::GUIButton> btnNew;
			NotNullPtr<UI::GUIListView> lvEntry;

			void RandBytes(UOSInt len);
			static void __stdcall OnKeyRand80Clicked(AnyType userObj);
			static void __stdcall OnKeyRand160Clicked(AnyType userObj);
			static void __stdcall OnNewClicked(AnyType userObj);
			static void __stdcall OnEntryDblClicked(AnyType userObj, UOSInt index);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIROTPForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROTPForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
