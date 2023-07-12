#ifndef _SM_SSWR_AVIREAD_AVIRMACMANAGERFORM
#define _SM_SSWR_AVIREAD_AVIRMACMANAGERFORM
#include "Net/MACInfoList.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMACManagerForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				UInt8 mac[6];
				UInt64 macInt;
				NotNullPtr<Text::String> ssid;
				Int32 phyType;
				Double freq;
				Text::String *manuf;
				Text::String *model;
				Text::String *serialNum;
				Text::String *country;
				UInt8 ouis[3][3];
				UInt64 neighbour[20];
				UInt32 ieLen;
				UInt8 *ieBuff;
			} LogFileEntry;
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIPanel *pnlControl;
			UI::GUIButton *btnStore;
			UI::GUILabel *lblInfo;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpFile;
			UI::GUIPanel *pnlFile;
			UI::GUILabel *lblFile;
			UI::GUITextBox *txtFile;
			UI::GUIButton *btnFile;
			UI::GUITextBox *txtFileIE;
			UI::GUIVSplitter *vspFile;
			UI::GUIListView *lvContent;

			UI::GUITabPage *tpInput;
			UI::GUILabel *lblInput;
			UI::GUITextBox *txtInput;
			UI::GUIButton *btnInput;

			UI::GUITabPage *tpWireshark;
			UI::GUIButton *btnWireshark;

			Data::ArrayList<LogFileEntry*> logList;
			Net::MACInfoList macList;

			static void __stdcall OnFileClicked(void *userObj);
			static void __stdcall OnStoreClicked(void *userObj);
			static void __stdcall OnContentDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnContentSelChg(void *userObj);
			static void __stdcall OnInputClicked(void *userObj);
			static void __stdcall OnWiresharkClicked(void *userObj);
			void LogFileLoad(Text::CString fileName);
			void LogFileClear();

			void UpdateStatus();
		public:
			AVIRMACManagerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRMACManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
