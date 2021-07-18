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
				const UTF8Char *ssid;
				Int32 phyType;
				Double freq;
				const UTF8Char *manuf;
				const UTF8Char *model;
				const UTF8Char *serialNum;
				const UTF8Char *country;
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

			Data::ArrayList<LogFileEntry*> *logList;
			Net::MACInfoList *macList;

			static void __stdcall OnFileClicked(void *userObj);
			static void __stdcall OnStoreClicked(void *userObj);
			static void __stdcall OnContentDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnContentSelChg(void *userObj);
			static void __stdcall OnInputClicked(void *userObj);
			void LogFileLoad(const UTF8Char *fileName);
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
