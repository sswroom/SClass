#ifndef _SM_SSWR_AVIREAD_AVIRHEXVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRHEXVIEWERFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHexFileView.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
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
			UI::GUITabControl *tcMain;
			
			UI::GUITabPage *tpValues;
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
			UI::GUILabel *lblUTF8CharCode;
			UI::GUITextBox *txtUTF8CharCode;
			UI::GUIButton *btnFont;
			UI::GUIButton *btnNextUnk;
			UI::GUICheckBox *chkDynamicSize;
			UI::GUIButton *btnOpenFile;

			UI::GUITabPage *tpAnalyse;
			UI::GUILabel *lblFileFormat;
			UI::GUITextBox *txtFileFormat;
			UI::GUILabel *lblFrameName;
			UI::GUITextBox *txtFrameName;
			UI::GUILabel *lblFieldDetail;
			UI::GUITextBox *txtFieldDetail;

			static void __stdcall OnFilesDrop(void *userObj, Text::String **files, UOSInt nFiles);
			static void __stdcall OnEndianChg(void *userObj, Bool newState);
			static void __stdcall OnOffsetChg(void *userObj, UInt64 ofst);
			static void __stdcall OnFontClicked(void *userObj);
			static void __stdcall OnNextUnkClicked(void *userObj);
			static void __stdcall OnOpenFileClicked(void *userObj);
			Bool LoadFile(Text::CString fileName, Bool dynamicSize);
		public:
			AVIRHexViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRHexViewerForm();

			virtual void OnMonitorChanged();

			void SetData(IO::IStreamData *fd);
		};
	}
}
#endif
