#ifndef _SM_SSWR_AVIREAD_AVIRWIFILOGMANAGERFORM
#define _SM_SSWR_AVIREAD_AVIRWIFILOGMANAGERFORM
#include "Net/MACInfoList.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWiFiLogManagerForm : public UI::GUIForm
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
			UI::GUIButton *btnFile;
			UI::GUIButton *btnStore;
			UI::GUILabel *lblInfo;
			UI::GUITextBox *txtFileIE;
			UI::GUIVSplitter *vspFile;
			UI::GUIListView *lvContent;

			Data::ArrayList<LogFileEntry*> *logList;
			Net::MACInfoList *macList;

			static void __stdcall OnFileClicked(void *userObj);
			static void __stdcall OnStoreClicked(void *userObj);
			static void __stdcall OnContentDblClicked(void *userObj, OSInt index);
			static void __stdcall OnContentSelChg(void *userObj);
			void LogFileLoad(const UTF8Char *fileName);
			Bool LogFileStore();
			void LogClear();
			LogFileEntry *LogGet(UInt64 iMAC);
			OSInt LogInsert(LogFileEntry *log);
			void LogUIUpdate();

			void UpdateStatus();
		public:
			AVIRWiFiLogManagerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRWiFiLogManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
