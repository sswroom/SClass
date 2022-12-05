#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRSolarEdgeForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSolarEdgeForm::OnAPIKeyClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSolarEdgeForm *me = (SSWR::AVIRead::AVIRSolarEdgeForm*)userObj;
	if (me->seAPI)
	{
		DEL_CLASS(me->seAPI);
		me->seAPI = 0;
		me->btnAPIKey->SetText(CSTR("Start"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		me->txtAPIKey->GetText(&sb);
		if (sb.GetLength() == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter API Key"), CSTR("SolarEdge API"), me);
			return;
		}
		NEW_CLASS(me->seAPI, Net::SolarEdgeAPI(me->core->GetSocketFactory(), me->ssl, sb.ToCString()));
		Text::String *s = me->seAPI->GetCurrentVersion();
		if (s)
		{
			me->txtCurrVer->SetText(s->ToCString());
			s->Release();

			Data::ArrayList<Text::String *> vers;
			if (me->seAPI->GetSupportedVersions(&vers))
			{
				sb.ClearStr();
				UOSInt i = 0;
				UOSInt j = vers.GetCount();
				while (i < j)
				{
					if (i > 0)
					{
						sb.AppendC(UTF8STRC(", "));
					}
					sb.Append(vers.GetItem(i));
					i++;
				}
				me->txtSuppVer->SetText(sb.ToCString());
				LIST_FREE_STRING(&vers);
			}
			else
			{
				me->txtSuppVer->SetText(CSTR(""));
			}
			me->btnAPIKey->SetText(CSTR("Stop"));
		}
		else
		{
			DEL_CLASS(me->seAPI);
			me->seAPI = 0;
			UI::MessageDialog::ShowDialog(CSTR("API Key error"), CSTR("SolarEdge API"), me);
			return;
		}
	}
}

SSWR::AVIRead::AVIRSolarEdgeForm::AVIRSolarEdgeForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("SolarEdge API"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->seAPI = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlAPIKey, UI::GUIPanel(ui, this));
	this->pnlAPIKey->SetRect(0, 0, 100, 31, false);
	this->pnlAPIKey->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblAPIKey, UI::GUILabel(ui, this->pnlAPIKey, CSTR("API Key")));
	this->lblAPIKey->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtAPIKey, UI::GUITextBox(ui, this->pnlAPIKey, CSTR("")));
	this->txtAPIKey->SetRect(104, 4, 300, 23, false);
	NEW_CLASS(this->btnAPIKey, UI::GUIButton(ui, this->pnlAPIKey, CSTR("Start")));
	this->btnAPIKey->SetRect(404, 4, 75, 23, false);
	this->btnAPIKey->HandleButtonClick(OnAPIKeyClicked, this);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpVersion = this->tcMain->AddTabPage(CSTR("Version"));
	NEW_CLASS(this->lblCurrVer, UI::GUILabel(ui, this->tpVersion, CSTR("Current Version")));
	this->lblCurrVer->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtCurrVer, UI::GUITextBox(ui, this->tpVersion, CSTR("")));
	this->txtCurrVer->SetRect(104, 4, 100, 23, false);
	this->txtCurrVer->SetReadOnly(true);
	NEW_CLASS(this->lblSuppVer, UI::GUILabel(ui, this->tpVersion, CSTR("Supported Versions")));
	this->lblSuppVer->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtSuppVer, UI::GUITextBox(ui, this->tpVersion, CSTR("")));
	this->txtSuppVer->SetRect(104, 28, 300, 23, false);
	this->txtSuppVer->SetReadOnly(true);
}

SSWR::AVIRead::AVIRSolarEdgeForm::~AVIRSolarEdgeForm()
{
	SDEL_CLASS(this->seAPI);
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRSolarEdgeForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
