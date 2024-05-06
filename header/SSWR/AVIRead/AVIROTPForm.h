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
				NN<Text::String> name;
				Crypto::OTP *otp;
				UInt64 lastCounter;
			} EntryInfo;
			
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListNN<EntryInfo> entryList;

			NN<UI::GUIGroupBox> grpNew;
			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUILabel> lblKey;
			NN<UI::GUITextBox> txtKey;
			NN<UI::GUIButton> btnKeyRand80;
			NN<UI::GUIButton> btnKeyRand160;
			NN<UI::GUILabel> lblType;
			NN<UI::GUIComboBox> cboType;
			NN<UI::GUIButton> btnNew;
			NN<UI::GUIListView> lvEntry;

			void RandBytes(UOSInt len);
			static void __stdcall OnKeyRand80Clicked(AnyType userObj);
			static void __stdcall OnKeyRand160Clicked(AnyType userObj);
			static void __stdcall OnNewClicked(AnyType userObj);
			static void __stdcall OnEntryDblClicked(AnyType userObj, UOSInt index);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIROTPForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROTPForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
