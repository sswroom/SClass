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
			Optional<IO::Stream> stm;
			IO::StreamType stmType;
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::LogTool> log;
			Optional<IO::SiLabDriver> siLabDriver;
			Optional<Net::SSLEngine> ssl;
			IO::DeviceManager devMgr;
			Data::ArrayListNN<IO::DeviceInfo> devList;

			NN<UI::GUIPanel> pnlStreamType;
			NN<UI::GUILabel> lblStreamType;
			NN<UI::GUIComboBox> cboStreamType;
			NN<UI::GUITabControl> tcConfig;

			NN<UI::GUITabPage> tpSerialPort;
			NN<UI::GUILabel> lblSerialPort;
			NN<UI::GUIComboBox> cboSerialPort;
			NN<UI::GUILabel> lblBaudRate;
			NN<UI::GUITextBox> txtBaudRate;
			NN<UI::GUILabel> lblParity;
			NN<UI::GUIComboBox> cboParity;

			NN<UI::GUITabPage> tpSiLabPort;
			NN<UI::GUIPanel> pnlSLInfo;
			NN<UI::GUILabel> lblDLLVer;
			NN<UI::GUITextBox> txtDLLVer;
			NN<UI::GUILabel> lblDriverVer;
			NN<UI::GUITextBox> txtDriverVer;
			NN<UI::GUIPanel> pnlSLControl;
			NN<UI::GUILabel> lblSLBaudRate;
			NN<UI::GUITextBox> txtSLBaudRate;
			NN<UI::GUIListView> lvSLPort;

			NN<UI::GUITabPage> tpTCPSvr;
			NN<UI::GUILabel> lblTCPSvrPort;
			NN<UI::GUITextBox> txtTCPSvrPort;
			NN<UI::GUICheckBox> chkBoardcast;

			NN<UI::GUITabPage> tpTCPCli;
			NN<UI::GUILabel> lblTCPCliHost;
			NN<UI::GUITextBox> txtTCPCliHost;
			NN<UI::GUILabel> lblTCPCliPort;
			NN<UI::GUITextBox> txtTCPCliPort;

			NN<UI::GUITabPage> tpSSLCli;
			NN<UI::GUILabel> lblSSLCliHost;
			NN<UI::GUITextBox> txtSSLCliHost;
			NN<UI::GUILabel> lblSSLCliPort;
			NN<UI::GUITextBox> txtSSLCliPort;

			NN<UI::GUITabPage> tpUDPSvr;
			NN<UI::GUILabel> lblUDPSvrPort;
			NN<UI::GUITextBox> txtUDPSvrPort;

			NN<UI::GUITabPage> tpUDPCli;
			NN<UI::GUILabel> lblUDPCliHost;
			NN<UI::GUITextBox> txtUDPCliHost;
			NN<UI::GUILabel> lblUDPCliPort;
			NN<UI::GUITextBox> txtUDPCliPort;

			NN<UI::GUITabPage> tpFile;
			NN<UI::GUILabel> lblFileName;
			NN<UI::GUITextBox> txtFileName;
			NN<UI::GUIButton> btnFileBrowse;

			NN<UI::GUITabPage> tpHID;
			NN<UI::GUIListBox> lbHIDDevice;

			NN<UI::GUIPanel> pnlButtons;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
			static void __stdcall OnFileBrowseClick(AnyType userObj);
			static void __stdcall OnStmTypeChg(AnyType userObj);
		public:
			AVIRSelStreamForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Bool allowReadOnly, Optional<Net::SSLEngine> ssl, NN<IO::LogTool> log);
			virtual ~AVIRSelStreamForm();

			virtual void OnMonitorChanged();

			void SetInitStreamType(IO::StreamType stype);
			void SetInitSerialPort(UIntOS port);
			void SetInitBaudRate(Int32 baudRate);

			NN<IO::Stream> GetStream() const;
			IO::StreamType GetStreamType() const;
		};
	}
}
#endif
