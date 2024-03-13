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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIMainMenu> mnuMain;

			UI::GUIHexFileView *hexView;
			NotNullPtr<UI::GUITabControl> tcMain;
			
			NotNullPtr<UI::GUITabPage> tpValues;
			NotNullPtr<UI::GUILabel> lblEndian;
			NotNullPtr<UI::GUIRadioButton> radEndianLittle;
			NotNullPtr<UI::GUIRadioButton> radEndianBig;
			NotNullPtr<UI::GUILabel> lblInt8;
			NotNullPtr<UI::GUITextBox> txtInt8;
			NotNullPtr<UI::GUILabel> lblInt16;
			NotNullPtr<UI::GUITextBox> txtInt16;
			NotNullPtr<UI::GUILabel> lblInt32;
			NotNullPtr<UI::GUITextBox> txtInt32;
			NotNullPtr<UI::GUILabel> lblInt64;
			NotNullPtr<UI::GUITextBox> txtInt64;
			NotNullPtr<UI::GUILabel> lblUInt8;
			NotNullPtr<UI::GUITextBox> txtUInt8;
			NotNullPtr<UI::GUILabel> lblUInt16;
			NotNullPtr<UI::GUITextBox> txtUInt16;
			NotNullPtr<UI::GUILabel> lblUInt32;
			NotNullPtr<UI::GUITextBox> txtUInt32;
			NotNullPtr<UI::GUILabel> lblUInt64;
			NotNullPtr<UI::GUITextBox> txtUInt64;
			NotNullPtr<UI::GUILabel> lblFloat32;
			NotNullPtr<UI::GUITextBox> txtFloat32;
			NotNullPtr<UI::GUILabel> lblFloat64;
			NotNullPtr<UI::GUITextBox> txtFloat64;
			NotNullPtr<UI::GUILabel> lblUTF8CharCode;
			NotNullPtr<UI::GUITextBox> txtUTF8CharCode;
			NotNullPtr<UI::GUIButton> btnFont;
			NotNullPtr<UI::GUIButton> btnNextUnk;
			NotNullPtr<UI::GUICheckBox> chkDynamicSize;
			NotNullPtr<UI::GUIButton> btnOpenFile;

			NotNullPtr<UI::GUITabPage> tpAnalyse;
			NotNullPtr<UI::GUILabel> lblFileFormat;
			NotNullPtr<UI::GUITextBox> txtFileFormat;
			NotNullPtr<UI::GUILabel> lblFrameName;
			NotNullPtr<UI::GUITextBox> txtFrameName;
			NotNullPtr<UI::GUILabel> lblFieldDetail;
			NotNullPtr<UI::GUITextBox> txtFieldDetail;

			NotNullPtr<UI::GUITabPage> tpExtract;
			NotNullPtr<UI::GUILabel> lblExtractBegin;
			NotNullPtr<UI::GUITextBox> txtExtractBegin;
			NotNullPtr<UI::GUIButton> btnExtractBegin;
			NotNullPtr<UI::GUILabel> lblExtractEnd;
			NotNullPtr<UI::GUITextBox> txtExtractEnd;
			NotNullPtr<UI::GUIButton> btnExtractEnd;
			NotNullPtr<UI::GUIButton> btnExtract;

			static void __stdcall OnFilesDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnEndianChg(void *userObj, Bool newState);
			static void __stdcall OnOffsetChg(void *userObj, UInt64 ofst);
			static void __stdcall OnFontClicked(void *userObj);
			static void __stdcall OnNextUnkClicked(void *userObj);
			static void __stdcall OnOpenFileClicked(void *userObj);
			static void __stdcall OnExtractBeginClicked(void *userObj);
			static void __stdcall OnExtractEndClicked(void *userObj);
			static void __stdcall OnExtractClicked(void *userObj);
			Bool LoadFile(Text::CStringNN fileName, Bool dynamicSize);
		public:
			AVIRHexViewerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHexViewerForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			void SetData(NotNullPtr<IO::StreamData> fd, Optional<IO::FileAnalyse::IFileAnalyse> fileAnalyse);
		};
	}
}
#endif
