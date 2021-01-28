#ifndef _SM_UTILUI_DATASENDERFORM
#define _SM_UTILUI_DATASENDERFORM
#include "Net/SocketFactory.h"
#include "Net/TCPServer.h"
#include "Data/ArrayListStr.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "Rodsum/LSGateResendServer.h"
#include "Rodsum/LSGateRAWReader.h"

namespace UtilUI
{
	class DataSenderForm : public UI::GUIForm, public Rodsum::LSGateRAWReader::ProgressHandler
	{
	private:
		Net::SocketFactory *sockf;
		IO::LogTool *log;
		Rodsum::LSGateResendServer *svr;
		Rodsum::LSGateRAWReader *reader;

		UI::GUITabControl *tcMain;
		UI::GUITabPage *tpCtrl;
		UI::GUITabPage *tpLog;

		UI::GUIPanel *pnlCtrl;
		UI::GUIListBox *lbFiles;
		UI::GUITextBox *txtPort;
		UI::GUIButton *btnStart;
		UI::GUIProgressBar *pgbFile;
		UI::GUITextBox *txtFileDelay;
		UI::GUIButton *btnFileDelay;
		UI::GUITextBox *txtPacketDelay;
		UI::GUIButton *btnPacketDelay;

		UI::GUITextBox *txtLog;
		UI::GUIListBox *lbLog;
		UI::ListBoxLogger *logger;
		Data::ArrayListStr *files;
		Bool closing;
		
	private:
		static void __stdcall FileHandler(void *userObj, const WChar **files, OSInt nFiles);
		static void __stdcall OnStartClicked(void *userObj);
		static void __stdcall OnLogSelectedChange(void *userObj);
		static void __stdcall OnFileDelayClicked(void *userObj);
		static void __stdcall OnPacketDelayClicked(void *userObj);
		void AddFile(const WChar *fileName);
	public:
		DataSenderForm(void *hInst, UI::GUIClientControl *parent, UI::GUIUI *ui, Net::SocketFactory *sockf);
		virtual ~DataSenderForm();

		virtual void ProgressUpdate(const WChar *fileName, Int64 currPos, Int64 fileSize);
		virtual void FileStarted(const WChar *fileName, Int64 fileSize);
		virtual void FileFinished(const WChar *fileName);
	};
};
#endif
