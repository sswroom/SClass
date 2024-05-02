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
				NN<Text::String> ssid;
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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnStore;
			NN<UI::GUILabel> lblInfo;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpFile;
			NN<UI::GUIPanel> pnlFile;
			NN<UI::GUILabel> lblFile;
			NN<UI::GUITextBox> txtFile;
			NN<UI::GUIButton> btnFile;
			NN<UI::GUITextBox> txtFileIE;
			NN<UI::GUIVSplitter> vspFile;
			NN<UI::GUIListView> lvContent;

			NN<UI::GUITabPage> tpInput;
			NN<UI::GUILabel> lblInput;
			NN<UI::GUITextBox> txtInput;
			NN<UI::GUIButton> btnInput;

			NN<UI::GUITabPage> tpWireshark;
			NN<UI::GUIButton> btnWireshark;

			Data::ArrayListNN<LogFileEntry> logList;
			Net::MACInfoList macList;

			static void __stdcall OnFileClicked(AnyType userObj);
			static void __stdcall OnStoreClicked(AnyType userObj);
			static void __stdcall OnContentDblClicked(AnyType userObj, UOSInt index);
			static void __stdcall OnContentSelChg(AnyType userObj);
			static void __stdcall OnInputClicked(AnyType userObj);
			static void __stdcall OnWiresharkClicked(AnyType userObj);
			void LogFileLoad(Text::CStringNN fileName);
			void LogFileClear();

			void UpdateStatus();
		public:
			AVIRMACManagerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMACManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
