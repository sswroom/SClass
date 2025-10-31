#include "Stdafx.h"
#include "IO/BuildTime.h"
#include "IO/StmData/FileData.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRGISServerForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/ThreadUtil.h"

void __stdcall SSWR::AVIRead::AVIRGISServerForm::OnSSLCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISServerForm>();
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(0, me->ui, me->core, me->ssl, me->sslCert, me->sslKey, me->caCerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		NN<Crypto::Cert::X509Cert> nnCert;
		NN<Crypto::Cert::X509File> nnKey;
		me->sslCert.Delete();
		me->sslKey.Delete();
		me->ClearCACerts();
		me->sslCert = frm.GetCert();
		me->sslKey = frm.GetKey();
		frm.GetCACerts(me->caCerts);
		Text::StringBuilderUTF8 sb;
		if (me->sslCert.SetTo(nnCert)) nnCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		if (me->sslKey.SetTo(nnKey)) nnKey->ToShortString(sb);
		me->lblSSLCert->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRGISServerForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISServerForm>();
	NN<Net::WebServer::WebListener> listener;
	if (me->listener.SetTo(listener))
	{
		listener.Delete();
		me->listener = 0;
		me->txtPort->SetReadOnly(false);
		me->chkSSL->SetEnabled(true);
		me->btnSSLCert->SetEnabled(true);
		me->txtWorkerCnt->SetReadOnly(false);
		me->btnStart->SetText(CSTR("Start"));
		return;
	}
	UInt16 port = 0;
	Bool valid = true;
	UOSInt workerCnt;
	Text::StringBuilderUTF8 sb;
	me->txtPort->GetText(sb);
	Text::StrToUInt16S(sb.ToString(), port, 0);
	sb.ClearStr();
	me->txtWorkerCnt->GetText(sb);
	if (!sb.ToUOSInt(workerCnt))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid worker count"), CSTR("GIS Server"), me);
		return;
	}
	Optional<Net::SSLEngine> ssl = 0;

	if (me->chkSSL->IsChecked())
	{
		NN<Crypto::Cert::X509Cert> sslCert;
		NN<Crypto::Cert::X509File> sslKey;
		if (!me->sslCert.SetTo(sslCert) || !me->sslKey.SetTo(sslKey))
		{
			me->ui->ShowMsgOK(CSTR("Please select SSL Cert/Key First"), CSTR("GIS Server"), me);
			return;
		}
		ssl = me->ssl;
		NN<Net::SSLEngine> nnssl;
		if (!ssl.SetTo(nnssl) || !nnssl->ServerSetCertsASN1(sslCert, sslKey, me->caCerts))
		{
			me->ui->ShowMsgOK(CSTR("Error in initializing Cert/Key"), CSTR("GIS Server"), me);
			return;
		}
	}
	if (port < 65535)
	{
		UTF8Char sbuff[128];
		UnsafeArray<UTF8Char> sptr;
		Data::DateTime dt;
		IO::BuildTime::GetBuildTime(dt);
		dt.ToUTCTime();
		sptr = dt.ToString(Text::StrConcatC(sbuff, UTF8STRC("AVIRead_GIS/")), "yyyyMMddHHmmss");
		NEW_CLASSNN(listener, Net::WebServer::WebListener(me->core->GetTCPClientFactory(), ssl, me->hdlr, port, 120, 2, Sync::ThreadUtil::GetThreadCnt(), CSTRP(sbuff, sptr), false, Net::WebServer::KeepAlive::Default, false));
		if (listener->IsError())
		{
			valid = false;
			listener.Delete();
			me->ui->ShowMsgOK(CSTR("Error in listening to port"), CSTR("GIS Server"), me);
		}
		else
		{
			me->listener = listener;
			if (!listener->Start())
			{
				valid = false;
				me->ui->ShowMsgOK(CSTR("Error in starting GIS Server"), CSTR("GIS Server"), me);
			}
			else
			{
				if (port == 0)
				{
					sb.ClearStr();
					sb.AppendU16(listener->GetListenPort());
					me->txtPort->SetText(sb.ToCString());
				}
			}
		}
	}
	else
	{
		valid = false;
		me->ui->ShowMsgOK(CSTR("Port number out of range"), CSTR("GIS Server"), me);
	}

	if (valid)
	{
		me->txtPort->SetReadOnly(true);
		me->txtWorkerCnt->SetReadOnly(true);
		me->chkSSL->SetEnabled(false);
		me->btnSSLCert->SetEnabled(false);
		me->btnStart->SetText(CSTR("Stop"));
	}
	else
	{
		me->listener.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRGISServerForm::OnAssetSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISServerForm>();
	NN<Map::MapDrawLayer> layer;
	if (me->lbAsset->GetSelectedItem().GetOpt<Map::MapDrawLayer>().SetTo(layer))
	{
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		me->txtAssetPath->SetText(layer->GetSourceNameObj()->ToCString());
		sptr = Text::StrUInt32(sbuff, layer->GetCoordinateSystem()->GetSRID());
		me->txtAssetSRID->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		me->txtAssetPath->SetText(CSTR(""));
		me->txtAssetSRID->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRGISServerForm::OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRGISServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISServerForm>();
	NN<Parser::ParserList> parsers = me->core->GetParserList();
	NN<Map::MapDrawLayer> layer;
	UOSInt i = 0;
	UOSInt j = files.GetCount();
	while (i < j)
	{
		IO::StmData::FileData fd(files.GetItem(i), false);
		if (Optional<Map::MapDrawLayer>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::MapLayer)).SetTo(layer))
		{
			me->hdlr.AddAsset(layer);
			me->lbAsset->AddItem(fd.GetShortName().Or(CSTR("Untitled")), layer);
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRGISServerForm::ClearCACerts()
{
	this->caCerts.DeleteAll();
}

SSWR::AVIRead::AVIRGISServerForm::AVIRGISServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("GIS Server"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->sslCert = 0;
	this->sslKey = 0;
	this->listener = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;

	this->pnlConn = ui->NewPanel(*this);
	this->pnlConn->SetRect(0, 0, 100, 103, false);
	this->pnlConn->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPort = ui->NewLabel(this->pnlConn, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlConn, CSTR("12345"));
	this->txtPort->SetRect(104, 4, 50, 23, false);
	this->lblSSL = ui->NewLabel(this->pnlConn, CSTR("SSL"));
	this->lblSSL->SetRect(4, 28, 100, 23, false);
	this->chkSSL = ui->NewCheckBox(this->pnlConn, CSTR("Enable"), false);
	this->chkSSL->SetRect(104, 28, 100, 23, false);
	this->btnSSLCert = ui->NewButton(this->pnlConn, CSTR("Cert/Key"));
	this->btnSSLCert->SetRect(204, 28, 75, 23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	this->lblSSLCert = ui->NewLabel(this->pnlConn, CSTR(""));
	this->lblSSLCert->SetRect(284, 28, 200, 23, false);
	this->lblWorkerCnt = ui->NewLabel(this->pnlConn, CSTR("Worker Count"));
	this->lblWorkerCnt->SetRect(4, 52, 100, 23, false);
	sptr = Text::StrUOSInt(sbuff, Sync::ThreadUtil::GetThreadCnt());
	this->txtWorkerCnt = ui->NewTextBox(this->pnlConn, CSTRP(sbuff, sptr));
	this->txtWorkerCnt->SetRect(104, 52, 100, 23, false);
	this->btnStart = ui->NewButton(this->pnlConn, CSTR("Start"));
	this->btnStart->SetRect(104, 76, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpAsset = this->tcMain->AddTabPage(CSTR("Asset"));
	this->lbAsset = ui->NewListBox(this->tpAsset, false);
	this->lbAsset->SetRect(0, 0, 150, 23, false);
	this->lbAsset->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbAsset->HandleSelectionChange(OnAssetSelChg, this);
	this->hspAsset = ui->NewHSplitter(this->tpAsset, 3, false);
	this->pnlAsset = ui->NewPanel(this->tpAsset);
	this->pnlAsset->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblAssetPath = ui->NewLabel(this->pnlAsset, CSTR("Path"));
	this->lblAssetPath->SetRect(4, 4, 100, 23, false);
	this->txtAssetPath = ui->NewTextBox(this->pnlAsset, CSTR(""));
	this->txtAssetPath->SetRect(104, 4, 400, 23, false);
	this->txtAssetPath->SetReadOnly(true);
	this->lblAssetSRID = ui->NewLabel(this->pnlAsset, CSTR("SRID"));
	this->lblAssetSRID->SetRect(4, 28, 100, 23, false);
	this->txtAssetSRID = ui->NewTextBox(this->pnlAsset, CSTR(""));
	this->txtAssetSRID->SetRect(104, 28, 100, 23, false);
	this->txtAssetSRID->SetReadOnly(true);

	this->HandleDropFiles(OnFiles, this);
}

SSWR::AVIRead::AVIRGISServerForm::~AVIRGISServerForm()
{
	this->ssl.Delete();
	this->sslCert.Delete();
	this->sslKey.Delete();
	this->ClearCACerts();
}

void SSWR::AVIRead::AVIRGISServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

