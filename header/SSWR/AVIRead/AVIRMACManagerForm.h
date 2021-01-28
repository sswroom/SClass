#ifndef _SM_SSWR_AVIREAD_AVIRMACMANAGERFORM
#define _SM_SSWR_AVIREAD_AVIRMACMANAGERFORM
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
				Int64 macInt;
				const UTF8Char *ssid;
				Int32 phyType;
				Double freq;
				const UTF8Char *manuf;
				const UTF8Char *model;
				const UTF8Char *serialNum;
				const UTF8Char *country;
				UInt8 ouis[3][3];
				Int64 neighbour[20];
				Int32 ieLen;
				UInt8 *ieBuff;
			} LogFileEntry;

			typedef struct
			{
				Int64 rangeFrom;
				Int64 rangeTo;
				const UTF8Char *name;
			} DataEntry;
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

			Data::ArrayList<DataEntry*> *dataList;
			Data::ArrayList<LogFileEntry*> *logList;
			Bool modified;

			static void __stdcall OnFileClicked(void *userObj);
			static void __stdcall OnStoreClicked(void *userObj);
			static void __stdcall OnContentDblClicked(void *userObj, OSInt index);
			static void __stdcall OnContentSelChg(void *userObj);
			static void __stdcall OnInputClicked(void *userObj);
			void LogFileLoad(const UTF8Char *fileName);
			void LogFileClear();

			OSInt DataGetIndex(Int64 macInt);
			void DataLoad();
			Bool DataStore();
			void UpdateStatus();
		public:
			AVIRMACManagerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRMACManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
