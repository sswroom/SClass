#ifndef _SM_SSWR_AVIREAD_AVIRFILEEXFORM
#define _SM_SSWR_AVIREAD_AVIRFILEEXFORM

#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFileExForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblSrc;
			UI::GUITextBox *txtSrc;
			UI::GUIButton *btnSrc;
			UI::GUILabel *lblDest;
			UI::GUITextBox *txtDest;
			UI::GUIButton *btnDest;
			UI::GUILabel *lblFileSize;
			UI::GUITextBox *txtFileSize;
			UI::GUILabel *lblStartOfst;
			UI::GUITextBox *txtStartOfst;
			UI::GUILabel *lblEndOfst;
			UI::GUITextBox *txtEndOfst;
			UI::GUIButton *btnStart;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnSrcChanged(void *userObj);
			static void __stdcall OnSrcClicked(void *userObj);
			static void __stdcall OnDestClicked(void *userObj);
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt fileCnt);
		public:
			AVIRFileExForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileExForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
