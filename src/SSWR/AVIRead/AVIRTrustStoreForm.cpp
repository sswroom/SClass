#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRTrustStoreForm.h"

class CertComparator : public Data::Comparator<NN<SSWR::AVIRead::AVIRTrustStoreForm::CertEntry>>
{
public:
	virtual OSInt Compare(NN<SSWR::AVIRead::AVIRTrustStoreForm::CertEntry> a, NN<SSWR::AVIRead::AVIRTrustStoreForm::CertEntry> b) const
	{
		return a->subjectCN->CompareToFast(b->subjectCN->ToCString());
	}
};

void __stdcall SSWR::AVIRead::AVIRTrustStoreForm::OnTrustCertDblClicked(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRTrustStoreForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTrustStoreForm>();
	NN<Crypto::Cert::X509Cert> cert;
	if (me->lvTrustCert->GetItem(index).GetOpt<Crypto::Cert::X509Cert>().SetTo(cert))
	{
		me->core->OpenObject(cert->Clone());
	}
}

SSWR::AVIRead::AVIRTrustStoreForm::AVIRTrustStoreForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Crypto::Cert::CertStore> store) : UI::GUIForm(parent, 1024, 768, ui)
{
	Text::StringBuilderUTF8 sb;
	this->SetFont(0, 0, 8.25, false);
	NN<Crypto::Cert::CertStore> trusts;
	if (store.SetTo(trusts))
	{
		sb.AppendC(UTF8STRC("Trust Store - "));
		sb.Append(trusts->GetStoreName());
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

	this->lvTrustCert = ui->NewListView(*this, UI::ListViewStyle::Table, 5);
	this->lvTrustCert->SetShowGrid(true);
	this->lvTrustCert->SetFullRowSelect(true);
	this->lvTrustCert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTrustCert->HandleDblClk(OnTrustCertDblClicked, this);
	this->lvTrustCert->AddColumn(CSTR("SubjectCN"), 200);
	this->lvTrustCert->AddColumn(CSTR("IssuerCN"), 200);
	this->lvTrustCert->AddColumn(CSTR("NotValidBefore"), 150);
	this->lvTrustCert->AddColumn(CSTR("NotValidAfter"), 150);
	this->lvTrustCert->AddColumn(CSTR("Valid Status"), 150);

	NN<Net::SSLEngine> ssl;
	if (!this->store.SetTo(trusts) && this->ssl.SetTo(ssl))
	{
		this->store = ssl->GetTrustStore();
	}
	Data::ArrayListNN<CertEntry> certs;
	NN<Crypto::Cert::X509Cert> cert;
	NN<CertEntry> entry;
	UOSInt i;
	UOSInt j;
	if (this->store.SetTo(trusts))
	{
		i = 0;
		j = trusts->GetCount();
		while (i < j)
		{
			cert = trusts->GetItemNoCheck(i);
			sb.ClearStr();
			cert->GetSubjectCN(sb);
			entry = MemAllocNN(CertEntry);
			entry->cert = cert;
			entry->subjectCN = Text::String::New(sb.ToCString());
			certs.Add(entry);
			i++;
		}
	}
	CertComparator comp;
	Data::Sort::ArtificialQuickSort::Sort<NN<CertEntry>>(&certs, comp);

	Data::DateTime dt;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt k;
	i = 0;
	j = certs.GetCount();
	while (i < j)
	{
		entry = certs.GetItemNoCheck(i);
		k = this->lvTrustCert->AddItem(entry->subjectCN, entry->cert);
		sb.ClearStr();
		entry->cert->GetIssuerCN(sb);
		this->lvTrustCert->SetSubItem(k, 1, sb.ToCString());
		if (entry->cert->GetNotBefore(dt))
		{
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			this->lvTrustCert->SetSubItem(k, 2, CSTRP(sbuff, sptr));
		}
		if (entry->cert->GetNotAfter(dt))
		{
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			this->lvTrustCert->SetSubItem(k, 3, CSTRP(sbuff, sptr));
		}
		if (this->ssl.SetTo(ssl))
		{
			this->lvTrustCert->SetSubItem(k, 4, Crypto::Cert::X509File::ValidStatusGetDesc(entry->cert->IsValid(ssl, store)));
		}
		else
		{
			this->lvTrustCert->SetSubItem(k, 4, CSTR("SSL engine error"));
		}
		entry->subjectCN->Release();
		MemFreeNN(entry);
		i++;
	}
}

SSWR::AVIRead::AVIRTrustStoreForm::~AVIRTrustStoreForm()
{
	this->ssl.Delete();
	this->store.Delete();
}

void SSWR::AVIRead::AVIRTrustStoreForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
