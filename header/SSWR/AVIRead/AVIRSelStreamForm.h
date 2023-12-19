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
			Optional<Net::SSLEngine> ssl;
			IO::DeviceManager devMgr;
			Data::ArrayList<IO::DeviceInfo*> devList;

			NotNullPtr<UI::GUIPanel> pnlStreamType;
			NotNullPtr<UI::GUILabel> lblStreamType;
			NotNullPtr<UI::GUIComboBox> cboStreamType;
			NotNullPtr<UI::GUITabControl> tcConfig;

			NotNullPtr<UI::GUITabPage> tpSerialPort;
			NotNullPtr<UI::GUILabel> lblSerialPort;
			NotNullPtr<UI::GUIComboBox> cboSerialPort;
			NotNullPtr<UI::GUILabel> lblBaudRate;
			NotNullPtr<UI::GUITextBox> txtBaudRate;
			NotNullPtr<UI::GUILabel> lblParity;
			NotNullPtr<UI::GUIComboBox> cboParity;

			NotNullPtr<UI::GUITabPage> tpSiLabPort;
			NotNullPtr<UI::GUIPanel> pnlSLInfo;
			NotNullPtr<UI::GUILabel> lblDLLVer;
			NotNullPtr<UI::GUITextBox> txtDLLVer;
			NotNullPtr<UI::GUILabel> lblDriverVer;
			NotNullPtr<UI::GUITextBox> txtDriverVer;
			NotNullPtr<UI::GUIPanel> pnlSLControl;
			NotNullPtr<UI::GUILabel> lblSLBaudRate;
			NotNullPtr<UI::GUITextBox> txtSLBaudRate;
			UI::GUIListView *lvSLPort;

			NotNullPtr<UI::GUITabPage> tpTCPSvr;
			NotNullPtr<UI::GUILabel> lblTCPSvrPort;
			NotNullPtr<UI::GUITextBox> txtTCPSvrPort;
			NotNullPtr<UI::GUICheckBox> chkBoardcast;

			NotNullPtr<UI::GUITabPage> tpTCPCli;
			NotNullPtr<UI::GUILabel> lblTCPCliHost;
			NotNullPtr<UI::GUITextBox> txtTCPCliHost;
			NotNullPtr<UI::GUILabel> lblTCPCliPort;
			NotNullPtr<UI::GUITextBox> txtTCPCliPort;

			NotNullPtr<UI::GUITabPage> tpSSLCli;
			NotNullPtr<UI::GUILabel> lblSSLCliHost;
			NotNullPtr<UI::GUITextBox> txtSSLCliHost;
			NotNullPtr<UI::GUILabel> lblSSLCliPort;
			NotNullPtr<UI::GUITextBox> txtSSLCliPort;

			NotNullPtr<UI::GUITabPage> tpUDPSvr;
			NotNullPtr<UI::GUILabel> lblUDPSvrPort;
			NotNullPtr<UI::GUITextBox> txtUDPSvrPort;

			NotNullPtr<UI::GUITabPage> tpUDPCli;
			NotNullPtr<UI::GUILabel> lblUDPCliHost;
			NotNullPtr<UI::GUITextBox> txtUDPCliHost;
			NotNullPtr<UI::GUILabel> lblUDPCliPort;
			NotNullPtr<UI::GUITextBox> txtUDPCliPort;

			NotNullPtr<UI::GUITabPage> tpFile;
			NotNullPtr<UI::GUILabel> lblFileName;
			NotNullPtr<UI::GUITextBox> txtFileName;
			NotNullPtr<UI::GUIButton> btnFileBrowse;

			NotNullPtr<UI::GUITabPage> tpHID;
			UI::GUIListBox *lbHIDDevice;

			NotNullPtr<UI::GUIPanel> pnlButtons;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(void *userObj);
			static void __stdcall OnCancelClick(void *userObj);
			static void __stdcall OnFileBrowseClick(void *userObj);
			static void __stdcall OnStmTypeChg(void *userObj);
		public:
			AVIRSelStreamForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Bool allowReadOnly, Optional<Net::SSLEngine> ssl);
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
