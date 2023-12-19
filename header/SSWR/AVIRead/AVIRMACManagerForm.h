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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUIButton> btnStore;
			NotNullPtr<UI::GUILabel> lblInfo;
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpFile;
			NotNullPtr<UI::GUIPanel> pnlFile;
			NotNullPtr<UI::GUILabel> lblFile;
			NotNullPtr<UI::GUITextBox> txtFile;
			NotNullPtr<UI::GUIButton> btnFile;
			NotNullPtr<UI::GUITextBox> txtFileIE;
			NotNullPtr<UI::GUIVSplitter> vspFile;
			UI::GUIListView *lvContent;

			NotNullPtr<UI::GUITabPage> tpInput;
			NotNullPtr<UI::GUILabel> lblInput;
			NotNullPtr<UI::GUITextBox> txtInput;
			NotNullPtr<UI::GUIButton> btnInput;

			NotNullPtr<UI::GUITabPage> tpWireshark;
			NotNullPtr<UI::GUIButton> btnWireshark;

			Data::ArrayList<LogFileEntry*> logList;
			Net::MACInfoList macList;

			static void __stdcall OnFileClicked(void *userObj);
			static void __stdcall OnStoreClicked(void *userObj);
			static void __stdcall OnContentDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnContentSelChg(void *userObj);
			static void __stdcall OnInputClicked(void *userObj);
			static void __stdcall OnWiresharkClicked(void *userObj);
			void LogFileLoad(Text::CStringNN fileName);
			void LogFileClear();

			void UpdateStatus();
		public:
			AVIRMACManagerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMACManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
