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
		private:
			IO::Stream *stm;
			IO::StreamType stmType;
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::LogTool log;
			IO::SiLabDriver *siLabDriver;
			Net::SSLEngine *ssl;
			IO::DeviceManager devMgr;
			Data::ArrayList<IO::DeviceInfo*> devList;

			NotNullPtr<UI::GUIPanel> pnlStreamType;
			UI::GUILabel *lblStreamType;
			UI::GUIComboBox *cboStreamType;
			UI::GUITabControl *tcConfig;

			NotNullPtr<UI::GUITabPage> tpSerialPort;
			UI::GUILabel *lblSerialPort;
			UI::GUIComboBox *cboSerialPort;
			UI::GUILabel *lblBaudRate;
			UI::GUITextBox *txtBaudRate;
			UI::GUILabel *lblParity;
			UI::GUIComboBox *cboParity;

			NotNullPtr<UI::GUITabPage> tpSiLabPort;
			NotNullPtr<UI::GUIPanel> pnlSLInfo;
			UI::GUILabel *lblDLLVer;
			UI::GUITextBox *txtDLLVer;
			UI::GUILabel *lblDriverVer;
			UI::GUITextBox *txtDriverVer;
			NotNullPtr<UI::GUIPanel> pnlSLControl;
			UI::GUILabel *lblSLBaudRate;
			UI::GUITextBox *txtSLBaudRate;
			UI::GUIListView *lvSLPort;

			NotNullPtr<UI::GUITabPage> tpTCPSvr;
			UI::GUILabel *lblTCPSvrPort;
			UI::GUITextBox *txtTCPSvrPort;
			UI::GUICheckBox *chkBoardcast;

			NotNullPtr<UI::GUITabPage> tpTCPCli;
			UI::GUILabel *lblTCPCliHost;
			UI::GUITextBox *txtTCPCliHost;
			UI::GUILabel *lblTCPCliPort;
			UI::GUITextBox *txtTCPCliPort;

			NotNullPtr<UI::GUITabPage> tpSSLCli;
			UI::GUILabel *lblSSLCliHost;
			UI::GUITextBox *txtSSLCliHost;
			UI::GUILabel *lblSSLCliPort;
			UI::GUITextBox *txtSSLCliPort;

			NotNullPtr<UI::GUITabPage> tpUDPSvr;
			UI::GUILabel *lblUDPSvrPort;
			UI::GUITextBox *txtUDPSvrPort;

			NotNullPtr<UI::GUITabPage> tpUDPCli;
			UI::GUILabel *lblUDPCliHost;
			UI::GUITextBox *txtUDPCliHost;
			UI::GUILabel *lblUDPCliPort;
			UI::GUITextBox *txtUDPCliPort;

			NotNullPtr<UI::GUITabPage> tpFile;
			UI::GUILabel *lblFileName;
			UI::GUITextBox *txtFileName;
			UI::GUIButton *btnFileBrowse;

			NotNullPtr<UI::GUITabPage> tpHID;
			UI::GUIListBox *lbHIDDevice;

			NotNullPtr<UI::GUIPanel> pnlButtons;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnOKClick(void *userObj);
			static void __stdcall OnCancelClick(void *userObj);
			static void __stdcall OnFileBrowseClick(void *userObj);
			static void __stdcall OnStmTypeChg(void *userObj);
		public:
			AVIRSelStreamForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Bool allowReadOnly, Net::SSLEngine *ssl);
			virtual ~AVIRSelStreamForm();

			virtual void OnMonitorChanged();

			void SetInitStreamType(IO::StreamType stype);
			void SetInitSerialPort(UOSInt port);
			void SetInitBaudRate(Int32 baudRate);

			NotNullPtr<IO::Stream> GetStream() const;
			IO::StreamType GetStreamType() const;
		};
	}
}
#endif
