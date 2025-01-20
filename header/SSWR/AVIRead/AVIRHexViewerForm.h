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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIMainMenu> mnuMain;

			NN<UI::GUIHexFileView> hexView;
			NN<UI::GUITabControl> tcMain;
			
			NN<UI::GUITabPage> tpValues;
			NN<UI::GUILabel> lblEndian;
			NN<UI::GUIRadioButton> radEndianLittle;
			NN<UI::GUIRadioButton> radEndianBig;
			NN<UI::GUILabel> lblInt8;
			NN<UI::GUITextBox> txtInt8;
			NN<UI::GUILabel> lblInt16;
			NN<UI::GUITextBox> txtInt16;
			NN<UI::GUILabel> lblInt32;
			NN<UI::GUITextBox> txtInt32;
			NN<UI::GUILabel> lblInt64;
			NN<UI::GUITextBox> txtInt64;
			NN<UI::GUILabel> lblUInt8;
			NN<UI::GUITextBox> txtUInt8;
			NN<UI::GUILabel> lblUInt16;
			NN<UI::GUITextBox> txtUInt16;
			NN<UI::GUILabel> lblUInt32;
			NN<UI::GUITextBox> txtUInt32;
			NN<UI::GUILabel> lblUInt64;
			NN<UI::GUITextBox> txtUInt64;
			NN<UI::GUILabel> lblFloat32;
			NN<UI::GUITextBox> txtFloat32;
			NN<UI::GUILabel> lblFloat64;
			NN<UI::GUITextBox> txtFloat64;
			NN<UI::GUILabel> lblUTF8CharCode;
			NN<UI::GUITextBox> txtUTF8CharCode;
			NN<UI::GUIButton> btnFont;
			NN<UI::GUIButton> btnNextUnk;
			NN<UI::GUICheckBox> chkDynamicSize;
			NN<UI::GUIButton> btnOpenFile;

			NN<UI::GUITabPage> tpAnalyse;
			NN<UI::GUILabel> lblFileFormat;
			NN<UI::GUITextBox> txtFileFormat;
			NN<UI::GUILabel> lblFrameName;
			NN<UI::GUITextBox> txtFrameName;
			NN<UI::GUILabel> lblFieldDetail;
			NN<UI::GUITextBox> txtFieldDetail;

			NN<UI::GUITabPage> tpExtract;
			NN<UI::GUILabel> lblExtractBegin;
			NN<UI::GUITextBox> txtExtractBegin;
			NN<UI::GUIButton> btnExtractBegin;
			NN<UI::GUILabel> lblExtractEnd;
			NN<UI::GUITextBox> txtExtractEnd;
			NN<UI::GUIButton> btnExtractEnd;
			NN<UI::GUIButton> btnExtract;

			static void __stdcall OnFilesDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnEndianChg(AnyType userObj, Bool newState);
			static void __stdcall OnOffsetChg(AnyType userObj, UInt64 ofst);
			static void __stdcall OnFontClicked(AnyType userObj);
			static void __stdcall OnNextUnkClicked(AnyType userObj);
			static void __stdcall OnOpenFileClicked(AnyType userObj);
			static void __stdcall OnExtractBeginClicked(AnyType userObj);
			static void __stdcall OnExtractEndClicked(AnyType userObj);
			static void __stdcall OnExtractClicked(AnyType userObj);
			Bool LoadFile(Text::CStringNN fileName, Bool dynamicSize);
		public:
			AVIRHexViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHexViewerForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			void SetData(NN<IO::StreamData> fd, Optional<IO::FileAnalyse::FileAnalyser> fileAnalyse);
		};
	}
}
#endif
