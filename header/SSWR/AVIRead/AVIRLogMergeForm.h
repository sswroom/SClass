#ifndef _SM_SSWR_AVIREAD_AVIRLOGMERGEFORM
#define _SM_SSWR_AVIREAD_AVIRLOGMERGEFORM
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLogMergeForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			UI::GUILabel *lblFile1;
			UI::GUITextBox *txtFile1;
			UI::GUIButton *btnFile1;
			UI::GUILabel *lblFile2;
			UI::GUITextBox *txtFile2;
			UI::GUIButton *btnFile2;
			UI::GUILabel *lblOFile;
			UI::GUITextBox *txtOFile;
			UI::GUIButton *btnOFile;
			UI::GUIButton *btnConvert;

			static void __stdcall OnFile1Clicked(void *userObj);
			static void __stdcall OnFile2Clicked(void *userObj);
			static void __stdcall OnOFileClicked(void *userObj);
			static void __stdcall OnConvertClicked(void *userObj);
		public:
			AVIRLogMergeForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRLogMergeForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
