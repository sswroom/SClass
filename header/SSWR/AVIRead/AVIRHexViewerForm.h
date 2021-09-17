#ifndef _SM_SSWR_AVIREAD_AVIRHEXVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRHEXVIEWERFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHexFileView.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHexViewerForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIHexFileView *hexView;
			UI::GUIPanel *pnlValues;
			UI::GUILabel *lblEndian;
			UI::GUIRadioButton *radEndianLittle;
			UI::GUIRadioButton *radEndianBig;
			UI::GUILabel *lblInt8;
			UI::GUITextBox *txtInt8;
			UI::GUILabel *lblInt16;
			UI::GUITextBox *txtInt16;
			UI::GUILabel *lblInt32;
			UI::GUITextBox *txtInt32;
			UI::GUILabel *lblInt64;
			UI::GUITextBox *txtInt64;
			UI::GUILabel *lblUInt8;
			UI::GUITextBox *txtUInt8;
			UI::GUILabel *lblUInt16;
			UI::GUITextBox *txtUInt16;
			UI::GUILabel *lblUInt32;
			UI::GUITextBox *txtUInt32;
			UI::GUILabel *lblUInt64;
			UI::GUITextBox *txtUInt64;
			UI::GUILabel *lblFloat32;
			UI::GUITextBox *txtFloat32;
			UI::GUILabel *lblFloat64;
			UI::GUITextBox *txtFloat64;
			UI::GUIButton *btnFont;

			static void __stdcall OnFilesDrop(void *userObj, const UTF8Char **files, UOSInt nFiles);
			static void __stdcall OnEndianChg(void *userObj, Bool newState);
			static void __stdcall OnOffsetChg(void *userObj, UInt64 ofst);
			static void __stdcall OnFontClicked(void *userObj);
		public:
			AVIRHexViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRHexViewerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
