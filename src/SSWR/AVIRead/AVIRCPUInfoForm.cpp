#include "Stdafx.h"
#include "Manage/CPUDB.h"
#include "Manage/CPUInfoDetail.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRCPUInfoForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/Clipboard.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRCPUInfoForm::OnUploadClick(void *userObj)
{
#if defined(CPU_X86_32) || defined(CPU_X86_64)
	SSWR::AVIRead::AVIRCPUInfoForm *me = (SSWR::AVIRead::AVIRCPUInfoForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Manage::CPUInfo cpu;
	if ((sptr = cpu.GetCPUName(sbuff)) == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in getting CPU Name"), CSTR("Error"), me);
	}
	else
	{
		Text::CString cpuModel = Manage::CPUDB::X86CPUNameToModel(CSTRP(sbuff, sptr));
		if (cpuModel.v == 0)
		{
			Int32 respStatus;
			Text::StringBuilderUTF8 sbData;
			NotNullPtr<Net::SocketFactory> sockf = me->core->GetSocketFactory();
			Text::StringBuilderUTF8 sbURL;
			sbURL.AppendC(UTF8STRC("http://sswroom.no-ip.org:5080/benchmark/cpuinfo?family="));
			sbURL.AppendI32(cpu.GetFamilyId());
			sbURL.AppendC(UTF8STRC("&modelId="));
			sbURL.AppendI32(cpu.GetModelId());
			sbURL.AppendC(UTF8STRC("&stepping="));
			sbURL.AppendI32(cpu.GetStepping());

			sbData.AppendP(sbuff, sptr);
			NotNullPtr<Net::HTTPClient> cli;
			cli = Net::HTTPClient::CreateConnect(sockf, me->ssl, sbURL.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
			cli->AddContentLength(sbData.GetLength());
			cli->Write(sbData.ToString(), sbData.GetLength());
			respStatus = cli->GetRespStatus();
			cli.Delete();
			if (respStatus == 200)
			{
				UI::MessageDialog::ShowDialog(CSTR("Upload success"), CSTR("CPUInfo"), me);
			}
			else
			{
				UI::MessageDialog::ShowDialog(CSTR("Error in uploading to server"), CSTR("CPUInfo"), me);
			}
		}
		else
		{
			UI::MessageDialog::ShowDialog(CSTR("CPU Info already exist"), CSTR("CPUInfo"), me);
		}
	}

#endif
}

void __stdcall SSWR::AVIRead::AVIRCPUInfoForm::OnCopyInfoClick(void *userObj)
{
#if defined(CPU_X86_32) || defined(CPU_X86_64)
	SSWR::AVIRead::AVIRCPUInfoForm *me = (SSWR::AVIRead::AVIRCPUInfoForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Manage::CPUInfo cpu;
	if ((sptr = cpu.GetCPUName(sbuff)) == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in getting CPU Name"), CSTR("Error"), me);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendI32(cpu.GetFamilyId());
		sb.AppendUTF8Char('\t');
		sb.AppendI32(cpu.GetModelId());
		sb.AppendUTF8Char('\t');
		sb.AppendI32(cpu.GetStepping());
		sb.AppendUTF8Char('\t');
		sb.AppendP(sbuff, sptr);
		UI::Clipboard::SetString(me->GetHandle(), sb.ToCString());
	}
#endif
}

SSWR::AVIRead::AVIRCPUInfoForm::AVIRCPUInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("CPU Info"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, *this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpCPU = this->tcMain->AddTabPage(CSTR("CPU"));
	NEW_CLASSNN(this->pnlMain, UI::GUIPanel(ui, this->tpCPU));
	this->pnlMain->SetRect(0, 0, 100, 31, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnUpload, UI::GUIButton(ui, this->pnlMain, CSTR("Upload Info")));
	this->btnUpload->SetRect(4, 4, 100, 23, false);
	this->btnUpload->HandleButtonClick(OnUploadClick, this);
	NEW_CLASS(this->btnCopyInfo, UI::GUIButton(ui, this->pnlMain, CSTR("Copy Info")));
	this->btnCopyInfo->SetRect(108, 4, 100, 23, false);
	this->btnCopyInfo->HandleButtonClick(OnCopyInfoClick, this);
	NEW_CLASS(this->lvMain, UI::GUIListView(ui, this->tpCPU, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvMain->SetFullRowSelect(true);
	this->lvMain->SetShowGrid(true);
	this->lvMain->AddColumn(CSTR("Name"), 200);
	this->lvMain->AddColumn(CSTR("Value"), 550);

	this->tpCache = this->tcMain->AddTabPage(CSTR("Cache"));
	NEW_CLASS(this->lbCache, UI::GUIListBox(ui, this->tpCache, false));
	this->lbCache->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpFeature = this->tcMain->AddTabPage(CSTR("Feature"));
	NEW_CLASS(this->lvFeature, UI::GUIListView(ui, this->tpFeature, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvFeature->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFeature->SetFullRowSelect(true);
	this->lvFeature->SetShowGrid(true);
	this->lvFeature->AddColumn(CSTR("Short Name"), 100);
	this->lvFeature->AddColumn(CSTR("Value"), 50);
	this->lvFeature->AddColumn(CSTR("Name"), 150);
	this->lvFeature->AddColumn(CSTR("Description"), 450);

	Manage::CPUInfoDetail cpu;
	Text::StringBuilderUTF8 sb;
	UOSInt i = 0;
	UOSInt j = cpu.GetInfoCnt();
	UOSInt k;
	while (i < j)
	{
		sb.ClearStr();
		cpu.GetInfoName(i, sb);
		k = this->lvMain->AddItem(sb.ToCString(), 0);
		sb.ClearStr();
		cpu.GetInfoValue(i, sb);
		this->lvMain->SetSubItem(k, 1, sb.ToCString());
		i++;
	}
	Data::ArrayList<const UTF8Char *> infoList;
	cpu.GetCacheInfoList(&infoList);
	i = 0;
	j = infoList.GetCount();
	while (i < j)
	{
		const UTF8Char *info = infoList.GetItem(i);
		this->lbCache->AddItem({info, Text::StrCharCnt(info)}, 0);
		i++;
	}

	Int32 flags1;
	Int32 flags2;
	cpu.GetFeatureFlags(&flags1, &flags2);
	i = 0;
	while (i < 32)
	{
		k = this->lvFeature->AddItem(Manage::CPUInfo::GetFeatureShortName(i), 0);
		if (flags1 & (1 << i))
		{
			this->lvFeature->SetSubItem(k, 1, CSTR("1"));
		}
		else
		{
			this->lvFeature->SetSubItem(k, 1, CSTR("0"));
		}
		this->lvFeature->SetSubItem(k, 2, Manage::CPUInfo::GetFeatureName(i));
		this->lvFeature->SetSubItem(k, 3, Manage::CPUInfo::GetFeatureDesc(i));
		i++;
	}
	i = 0;
	while (i < 32)
	{
		k = this->lvFeature->AddItem(Manage::CPUInfo::GetFeatureShortName(i + 32), 0);
		if (flags2 & (1 << i))
		{
			this->lvFeature->SetSubItem(k, 1, CSTR("1"));
		}
		else
		{
			this->lvFeature->SetSubItem(k, 1, CSTR("0"));
		}
		this->lvFeature->SetSubItem(k, 2, Manage::CPUInfo::GetFeatureName(i + 32));
		this->lvFeature->SetSubItem(k, 3, Manage::CPUInfo::GetFeatureDesc(i + 32));
		i++;
	}

	Double t;
	Int32 r;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sb.ClearStr();
	if ((sptr = cpu.GetCPUName(sbuff)) != 0)
	{
		sb.AppendP(sbuff, sptr);
		k = this->lvMain->AddItem(CSTR("CPU Name"), 0);
		this->lvMain->SetSubItem(k, 1, sb.ToCString());
	}
	if (cpu.GetCPURatio(&r))
	{
		sb.ClearStr();
		sb.AppendI32(r);
		k = this->lvMain->AddItem(CSTR("Ratio"), 0);
		this->lvMain->SetSubItem(k, 1, sb.ToCString());
	}
	if (cpu.GetCPUTurboRatio(&r))
	{
		sb.ClearStr();
		sb.AppendI32(r);
		k = this->lvMain->AddItem(CSTR("Turbo Ratio"), 0);
		this->lvMain->SetSubItem(k, 1, sb.ToCString());
	}
	if (cpu.GetCPUTCC(&t))
	{
		sb.ClearStr();
		sb.AppendDouble(t);
		k = this->lvMain->AddItem(CSTR("TCC"), 0);
		this->lvMain->SetSubItem(k, 1, sb.ToCString());
	}
	if (cpu.GetCPUTemp(0, &t))
	{
		sb.ClearStr();
		sb.AppendDouble(t);
		k = this->lvMain->AddItem(CSTR("Temp"), 0);
		this->lvMain->SetSubItem(k, 1, sb.ToCString());
	}
}

SSWR::AVIRead::AVIRCPUInfoForm::~AVIRCPUInfoForm()
{
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRCPUInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
