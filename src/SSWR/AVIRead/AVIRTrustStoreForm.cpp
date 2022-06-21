#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRTrustStoreForm.h"

class CertComparator : public Data::Comparator<SSWR::AVIRead::AVIRTrustStoreForm::CertEntry*>
{
public:
	virtual OSInt Compare(SSWR::AVIRead::AVIRTrustStoreForm::CertEntry* a, SSWR::AVIRead::AVIRTrustStoreForm::CertEntry* b)
	{
		return a->subjectCN->CompareToFast(b->subjectCN->ToCString());
	}
};

void __stdcall SSWR::AVIRead::AVIRTrustStoreForm::OnTrustCertDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRTrustStoreForm *me = (SSWR::AVIRead::AVIRTrustStoreForm*)userObj;
	Crypto::Cert::X509Cert *cert = (Crypto::Cert::X509Cert*)me->lvTrustCert->GetItem(index);
	me->core->OpenObject(cert->Clone());
}

SSWR::AVIRead::AVIRTrustStoreForm::AVIRTrustStoreForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Crypto::Cert::CertStore *store) : UI::GUIForm(parent, 1024, 768, ui)
{
	Text::StringBuilderUTF8 sb;
	this->SetFont(0, 0, 8.25, false);
	if (store)
	{
		sb.AppendC(UTF8STRC("Trust Store - "));
		sb.Append(store->GetStoreName());
		this->SetText(sb.ToCString());
	}
	else
	{
		this->SetText(CSTR("Trust Store"));
	}

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(core->GetSocketFactory(), false);
	this->store = store;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lvTrustCert, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvTrustCert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTrustCert->HandleDblClk(OnTrustCertDblClicked, this);
	this->lvTrustCert->AddColumn(CSTR("SubjectCN"), 200);
	this->lvTrustCert->AddColumn(CSTR("IssuerCN"), 200);
	this->lvTrustCert->AddColumn(CSTR("NotValidBefore"), 150);
	this->lvTrustCert->AddColumn(CSTR("NotValidAfter"), 150);

	if (this->store == 0)
	{
		store = this->ssl->GetTrustStore();
	}
	Data::ArrayList<CertEntry*> certs;
	Crypto::Cert::X509Cert *cert;
	CertEntry *entry;
	UOSInt i = 0;
	UOSInt j = store->GetCount();
	while (i < j)
	{
		cert = store->GetItem(i);
		sb.ClearStr();
		cert->GetSubjectCN(&sb);
		entry = MemAlloc(CertEntry, 1);
		entry->cert = cert;
		entry->subjectCN = Text::String::New(sb.ToCString());
		certs.Add(entry);
		i++;
	}
	CertComparator comp;
	Data::Sort::ArtificialQuickSort::Sort(&certs, &comp);

	Data::DateTime dt;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt k;
	i = 0;
	j = certs.GetCount();
	while (i < j)
	{
		entry = certs.GetItem(i);
		k = this->lvTrustCert->AddItem(entry->subjectCN, entry->cert);
		sb.ClearStr();
		entry->cert->GetIssuerCN(&sb);
		this->lvTrustCert->SetSubItem(k, 1, sb.ToCString());
		if (entry->cert->GetNotBefore(&dt))
		{
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			this->lvTrustCert->SetSubItem(k, 2, CSTRP(sbuff, sptr));
		}
		if (entry->cert->GetNotAfter(&dt))
		{
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			this->lvTrustCert->SetSubItem(k, 3, CSTRP(sbuff, sptr));
		}
		entry->subjectCN->Release();
		MemFree(entry);
		i++;
	}
}

SSWR::AVIRead::AVIRTrustStoreForm::~AVIRTrustStoreForm()
{
	SDEL_CLASS(this->ssl);
	SDEL_CLASS(this->store);
}

void SSWR::AVIRead::AVIRTrustStoreForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
