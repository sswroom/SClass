#include "Stdafx.h"
#include "IO/BuildTime.h"
#include "IO/StmData/FileData.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRGISServerForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/ThreadUtil.h"

#define TITLE CSTR("GIS Server")

void __stdcall SSWR::AVIRead::AVIRGISServerForm::OnSSLCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISServerForm>();
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(nullptr, me->ui, me->core, me->ssl, me->sslCert, me->sslKey, me->caCerts);
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
		me->listener = nullptr;
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
		me->ui->ShowMsgOK(CSTR("Please enter valid worker count"), TITLE, me);
		return;
	}
	Optional<Net::SSLEngine> ssl = nullptr;

	if (me->chkSSL->IsChecked())
	{
		NN<Crypto::Cert::X509Cert> sslCert;
		NN<Crypto::Cert::X509File> sslKey;
		if (!me->sslCert.SetTo(sslCert) || !me->sslKey.SetTo(sslKey))
		{
			me->ui->ShowMsgOK(CSTR("Please select SSL Cert/Key First"), TITLE, me);
			return;
		}
		ssl = me->ssl;
		NN<Net::SSLEngine> nnssl;
		if (!ssl.SetTo(nnssl) || !nnssl->ServerSetCertsASN1(sslCert, sslKey, me->caCerts))
		{
			me->ui->ShowMsgOK(CSTR("Error in initializing Cert/Key"), TITLE, me);
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
			me->ui->ShowMsgOK(CSTR("Error in listening to port"), TITLE, me);
		}
		else
		{
			me->listener = listener;
			if (!listener->Start())
			{
				valid = false;
				me->ui->ShowMsgOK(CSTR("Error in starting GIS Server"), TITLE, me);
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
		me->ui->ShowMsgOK(CSTR("Port number out of range"), TITLE, me);
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
		sptr = Text::StrUOSInt(sbuff, layer->GetRecordCnt());
		me->txtAssetCount->SetText(CSTRP(sbuff, sptr));
		Math::RectAreaDbl bbox;
		if (layer->GetBounds(bbox))
		{
			sptr = Text::StrDouble(sbuff, bbox.min.x);
			me->txtAssetMinX->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, bbox.min.y);
			me->txtAssetMinY->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, bbox.max.x);
			me->txtAssetMaxX->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, bbox.max.y);
			me->txtAssetMaxY->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			me->txtAssetMinX->SetText(CSTR(""));
			me->txtAssetMinY->SetText(CSTR(""));
			me->txtAssetMaxX->SetText(CSTR(""));
			me->txtAssetMaxY->SetText(CSTR(""));
		}
	}
	else
	{
		me->txtAssetPath->SetText(CSTR(""));
		me->txtAssetSRID->SetText(CSTR(""));
		me->txtAssetCount->SetText(CSTR(""));
		me->txtAssetMinX->SetText(CSTR(""));
		me->txtAssetMinY->SetText(CSTR(""));
		me->txtAssetMaxX->SetText(CSTR(""));
		me->txtAssetMaxY->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRGISServerForm::OnWSSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISServerForm>();
	NN<Map::GISWebService::GISWorkspace> ws;
	if (me->lbWS->GetSelectedItem().GetOpt<Map::GISWebService::GISWorkspace>().SetTo(ws))
	{
		me->txtWSName->SetText(ws->name->ToCString());
		me->txtWSURI->SetText(ws->uri->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRGISServerForm::OnWSAddClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISServerForm>();
	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbURI;
	me->txtWSName->GetText(sbName);
	me->txtWSURI->GetText(sbURI);
	if (sbName.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter name"), TITLE, me);
		return;
	}
	if (!sbURI.StartsWith(CSTR("http://")) && !sbURI.StartsWith(CSTR("https://")))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid URI"), TITLE, me);
		return;
	}
	NN<Map::GISWebService::GISWorkspace> ws;
	if (me->hdlr.AddWorkspace(sbName.ToCString(), sbURI.ToCString()).SetTo(ws))
	{
		me->lbWS->AddItem(ws->name, ws);
		me->cboFeatureWS->AddItem(ws->name, ws);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISServerForm::OnFeatureLayerSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISServerForm>();
	NN<Map::MapDrawLayer> layer;
	if (me->cboFeatureLayer->GetSelectedItem().GetOpt<Map::MapDrawLayer>().SetTo(layer))
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(layer->GetName());
		me->txtFeatureName->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRGISServerForm::OnFeatureAddClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISServerForm>();
	UOSInt i = me->cboFeatureLayer->GetSelectedIndex();
	if (i == INVALID_INDEX)
	{
		return;
	}
	NN<Map::GISWebService::GISWorkspace> ws;
	if (!me->cboFeatureWS->GetSelectedItem().GetOpt<Map::GISWebService::GISWorkspace>().SetTo(ws))
	{
		return;
	}
	Text::StringBuilderUTF8 sb;
	me->txtFeatureName->GetText(sb);
	if (me->hdlr.AddFeature(sb.ToCString(), ws, i))
	{
		me->lbFeature->AddItem(sb.ToCString(), 0);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISServerForm::OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRGISServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISServerForm>();
	NN<Parser::ParserList> parsers = me->core->GetParserList();
	NN<Map::MapDrawLayer> layer;
	UOSInt i = 0;
	UOSInt j = files.GetCount();
	UOSInt k;
	while (i < j)
	{
		IO::StmData::FileData fd(files.GetItem(i), false);
		if (Optional<Map::MapDrawLayer>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::MapLayer)).SetTo(layer))
		{
			Text::CStringNN name = fd.GetShortName().Or(CSTR("Untitled"));
			me->hdlr.AddAsset(layer);
			me->lbAsset->AddItem(name, layer);
			k = me->cboFeatureLayer->AddItem(name, layer);
			if (k == 0)
			{
				me->cboFeatureLayer->SetSelectedIndex(0);
			}
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
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->sslCert = nullptr;
	this->sslKey = nullptr;
	this->listener = nullptr;
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
	this->lblAssetCount = ui->NewLabel(this->pnlAsset, CSTR("Count"));
	this->lblAssetCount->SetRect(4, 28, 100, 23, false);
	this->txtAssetCount = ui->NewTextBox(this->pnlAsset, CSTR(""));
	this->txtAssetCount->SetRect(104, 28, 100, 23, false);
	this->txtAssetCount->SetReadOnly(true);
	this->lblAssetSRID = ui->NewLabel(this->pnlAsset, CSTR("SRID"));
	this->lblAssetSRID->SetRect(4, 52, 100, 23, false);
	this->txtAssetSRID = ui->NewTextBox(this->pnlAsset, CSTR(""));
	this->txtAssetSRID->SetRect(104, 52, 100, 23, false);
	this->txtAssetSRID->SetReadOnly(true);
	this->lblAssetMinX = ui->NewLabel(this->pnlAsset, CSTR("MinX"));
	this->lblAssetMinX->SetRect(4, 76, 100, 23, false);
	this->txtAssetMinX = ui->NewTextBox(this->pnlAsset, CSTR(""));
	this->txtAssetMinX->SetRect(104, 76, 100, 23, false);
	this->txtAssetMinX->SetReadOnly(true);
	this->lblAssetMinY = ui->NewLabel(this->pnlAsset, CSTR("MinY"));
	this->lblAssetMinY->SetRect(4, 100, 100, 23, false);
	this->txtAssetMinY = ui->NewTextBox(this->pnlAsset, CSTR(""));
	this->txtAssetMinY->SetRect(104, 100, 100, 23, false);
	this->txtAssetMinY->SetReadOnly(true);
	this->lblAssetMaxX = ui->NewLabel(this->pnlAsset, CSTR("MaxX"));
	this->lblAssetMaxX->SetRect(4, 124, 100, 23, false);
	this->txtAssetMaxX = ui->NewTextBox(this->pnlAsset, CSTR(""));
	this->txtAssetMaxX->SetRect(104, 124, 100, 23, false);
	this->txtAssetMaxX->SetReadOnly(true);
	this->lblAssetMaxY = ui->NewLabel(this->pnlAsset, CSTR("MaxY"));
	this->lblAssetMaxY->SetRect(4, 148, 100, 23, false);
	this->txtAssetMaxY = ui->NewTextBox(this->pnlAsset, CSTR(""));
	this->txtAssetMaxY->SetRect(104, 148, 100, 23, false);
	this->txtAssetMaxY->SetReadOnly(true);

	this->tpWS = this->tcMain->AddTabPage(CSTR("Workspace"));
	this->lbWS = ui->NewListBox(this->tpWS, false);
	this->lbWS->SetRect(0, 0, 150, 23, false);
	this->lbWS->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbWS->HandleSelectionChange(OnWSSelChg, this);
	this->hspWS = ui->NewHSplitter(this->tpWS, 3, false);
	this->pnlWS = ui->NewPanel(this->tpWS);
	this->pnlWS->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblWSName = ui->NewLabel(this->pnlWS, CSTR("Name"));
	this->lblWSName->SetRect(4, 4, 100, 23, false);
	this->txtWSName = ui->NewTextBox(this->pnlWS, CSTR(""));
	this->txtWSName->SetRect(104, 4, 150, 23, false);
	this->lblWSURI = ui->NewLabel(this->pnlWS, CSTR("URI"));
	this->lblWSURI->SetRect(4, 28, 100, 23, false);
	this->txtWSURI = ui->NewTextBox(this->pnlWS, CSTR(""));
	this->txtWSURI->SetRect(104, 28, 450, 23, false);
	this->btnWSAdd = ui->NewButton(this->pnlWS, CSTR("Add"));
	this->btnWSAdd->SetRect(104, 52, 75, 23, false);
	this->btnWSAdd->HandleButtonClick(OnWSAddClicked, this);

	this->tpFeature = this->tcMain->AddTabPage(CSTR("Feature"));
	this->lbFeature = ui->NewListBox(this->tpFeature, false);
	this->lbFeature->SetRect(0, 0, 150, 23, false);
	this->lbFeature->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->hspFeature = ui->NewHSplitter(this->tpFeature, 3, false);
	this->pnlFeature = ui->NewPanel(this->tpFeature);
	this->pnlFeature->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblFeatureLayer = ui->NewLabel(this->pnlFeature, CSTR("Layer"));
	this->lblFeatureLayer->SetRect(4, 4, 100, 23, false);
	this->cboFeatureLayer = ui->NewComboBox(this->pnlFeature, false);
	this->cboFeatureLayer->SetRect(104, 4, 150, 23, false);
	this->cboFeatureLayer->HandleSelectionChange(OnFeatureLayerSelChg, this);
	this->lblFeatureWS = ui->NewLabel(this->pnlFeature, CSTR("Workspace"));
	this->lblFeatureWS->SetRect(4, 28, 100, 23, false);
	this->cboFeatureWS = ui->NewComboBox(this->pnlFeature, false);
	this->cboFeatureWS->SetRect(104, 28, 150, 23, false);
	this->lblFeatureName = ui->NewLabel(this->pnlFeature, CSTR("Name"));
	this->lblFeatureName->SetRect(4, 52, 100, 23, false);
	this->txtFeatureName = ui->NewTextBox(this->pnlFeature, CSTR(""));
	this->txtFeatureName->SetRect(104, 52, 150, 23, false);
	this->btnFeatureAdd = ui->NewButton(this->pnlFeature, CSTR("Add"));
	this->btnFeatureAdd->SetRect(104, 76, 75, 23, false);
	this->btnFeatureAdd->HandleButtonClick(OnFeatureAddClicked, this);

	this->HandleDropFiles(OnFiles, this);
}

SSWR::AVIRead::AVIRGISServerForm::~AVIRGISServerForm()
{
	this->listener.Delete();
	this->ssl.Delete();
	this->sslCert.Delete();
	this->sslKey.Delete();
	this->ClearCACerts();
}

void SSWR::AVIRead::AVIRGISServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

