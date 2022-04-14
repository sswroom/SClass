#ifndef _SM_SSWR_AVIREAD_AVIRSELSTREAMFORM
#define _SM_SSWR_AVIREAD_AVIRSELSTREAMFORM
#include "IO/DeviceManager.h"
#include "IO/SiLabDriver.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSelStreamForm : public UI::GUIForm
		{
		public:
			IO::Stream *stm;
			SSWR::AVIRead::AVIRCore::StreamType stmType;
		private:
			SSWR::AVIRead::AVIRCore *core;
			IO::SiLabDriver *siLabDriver;
			IO::DeviceManager *devMgr;
			Data::ArrayList<IO::DeviceInfo*> *devList;

			UI::GUIPanel *pnlStreamType;
			UI::GUILabel *lblStreamType;
			UI::GUIComboBox *cboStreamType;
			UI::GUITabControl *tcConfig;

			UI::GUITabPage *tpSerialPort;
			UI::GUILabel *lblSerialPort;
			UI::GUIComboBox *cboSerialPort;
			UI::GUILabel *lblBaudRate;
			UI::GUITextBox *txtBaudRate;
			UI::GUILabel *lblParity;
			UI::GUIComboBox *cboParity;

			UI::GUITabPage *tpSiLabPort;
			UI::GUIPanel *pnlSLInfo;
			UI::GUILabel *lblDLLVer;
			UI::GUITextBox *txtDLLVer;
			UI::GUILabel *lblDriverVer;
			UI::GUITextBox *txtDriverVer;
			UI::GUIPanel *pnlSLControl;
			UI::GUILabel *lblSLBaudRate;
			UI::GUITextBox *txtSLBaudRate;
			UI::GUIListView *lvSLPort;

			UI::GUITabPage *tpTCPSvr;
			UI::GUILabel *lblTCPSvrPort;
			UI::GUITextBox *txtTCPSvrPort;
			UI::GUICheckBox *chkBoardcast;

			UI::GUITabPage *tpTCPCli;
			UI::GUILabel *lblTCPCliHost;
			UI::GUITextBox *txtTCPCliHost;
			UI::GUILabel *lblTCPCliPort;
			UI::GUITextBox *txtTCPCliPort;

			UI::GUITabPage *tpUDPSvr;
			UI::GUILabel *lblUDPSvrPort;
			UI::GUITextBox *txtUDPSvrPort;

			UI::GUITabPage *tpUDPCli;
			UI::GUILabel *lblUDPCliHost;
			UI::GUITextBox *txtUDPCliHost;
			UI::GUILabel *lblUDPCliPort;
			UI::GUITextBox *txtUDPCliPort;

			UI::GUITabPage *tpFile;
			UI::GUILabel *lblFileName;
			UI::GUITextBox *txtFileName;
			UI::GUIButton *btnFileBrowse;

			UI::GUITabPage *tpHID;
			UI::GUIListBox *lbHIDDevice;

			UI::GUIPanel *pnlButtons;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnOKClick(void *userObj);
			static void __stdcall OnCancelClick(void *userObj);
			static void __stdcall OnFileBrowseClick(void *userObj);
			static void __stdcall OnStmTypeChg(void *userObj);
		public:
			AVIRSelStreamForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Bool allowReadOnly);
			virtual ~AVIRSelStreamForm();

			virtual void OnMonitorChanged();

			void SetInitStreamType(SSWR::AVIRead::AVIRCore::StreamType stype);
			void SetInitSerialPort(UOSInt port);
			void SetInitBaudRate(Int32 baudRate);
		};
	}
}
#endif
