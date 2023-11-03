#include "Stdafx.h"
#include "Crypto/Cert/X509FileList.h"
#include "Net/SSLEngine.h"

Crypto::Cert::X509FileList::X509FileList(NotNullPtr<Text::String> sourceName, NotNullPtr<Crypto::Cert::X509Cert> cert) : Crypto::Cert::X509File(sourceName, cert->GetASN1Array())
{
	this->fileList.Add(cert);
}

Crypto::Cert::X509FileList::X509FileList(Text::CStringNN sourceName, NotNullPtr<Crypto::Cert::X509Cert> cert) : Crypto::Cert::X509File(sourceName, cert->GetASN1Array())
{
	this->fileList.Add(cert);
}

Crypto::Cert::X509FileList::~X509FileList()
{
	Crypto::Cert::X509File *file;
	UOSInt i = this->fileList.GetCount();
	while (i-- > 0)
	{
		file = this->fileList.GetItem(i);
		DEL_CLASS(file);
	}
}

Crypto::Cert::X509File::FileType Crypto::Cert::X509FileList::GetFileType() const
{
	return Crypto::Cert::X509File::FileType::FileList;
}

void Crypto::Cert::X509FileList::ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	return this->fileList.GetItem(0)->ToShortName(sb);
}

UOSInt Crypto::Cert::X509FileList::GetCertCount()
{
	UOSInt ret = 0;
	UOSInt i = this->fileList.GetCount();
	while (i-- > 0)
	{
		ret += this->fileList.GetItem(i)->GetCertCount();
	}
	return ret;
}

Bool Crypto::Cert::X509FileList::GetCertName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	UOSInt i = 0;
	UOSInt j = this->fileList.GetCount();
	UOSInt cnt;
	Crypto::Cert::X509File *file;
	while (i < j)
	{
		file = this->fileList.GetItem(i);
		cnt = file->GetCertCount();
		if (index < cnt)
		{
			return file->GetCertName(index, sb);
		}
		index -= cnt;
		i++;
	}
	return false;
}

Crypto::Cert::X509Cert *Crypto::Cert::X509FileList::GetNewCert(UOSInt index)
{
	UOSInt i = 0;
	UOSInt j = this->fileList.GetCount();
	UOSInt cnt;
	Crypto::Cert::X509File *file;
	while (i < j)
	{
		file = this->fileList.GetItem(i);
		cnt = file->GetCertCount();
		if (index < cnt)
		{
			return file->GetNewCert(index);
		}
		index -= cnt;
		i++;
	}
	return 0;
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509FileList::IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore) const
{
	Crypto::Cert::X509File::ValidStatus status;
	UOSInt i = this->fileList.GetCount();
	if (i > 1)
	{
		Crypto::Cert::X509File *file;
		if (trustStore == 0)
		{
			trustStore = ssl->GetTrustStore();
		}
		trustStore = trustStore->Clone().Ptr();
		while (i-- > 0)
		{
			file = this->fileList.GetItem(i);
			status = file->IsValid(ssl, trustStore);
			if (status != Crypto::Cert::X509File::ValidStatus::Valid)
			{
				DEL_CLASS(trustStore);
				return status;
			}
			if (file->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
			{
				trustStore->AddCert(NotNullPtr<Crypto::Cert::X509Cert>::ConvertFrom(file->Clone()));
			}
		}
		DEL_CLASS(trustStore);
		return Crypto::Cert::X509File::ValidStatus::Valid;
	}
	else
	{
		return this->fileList.GetItem(0)->IsValid(ssl, trustStore);
	}
}

NotNullPtr<Net::ASN1Data> Crypto::Cert::X509FileList::Clone() const
{
	NotNullPtr<Crypto::Cert::X509FileList> fileList;
	NEW_CLASSNN(fileList, Crypto::Cert::X509FileList(this->GetSourceNameObj(), NotNullPtr<Crypto::Cert::X509Cert>::ConvertFrom(this->fileList.GetItem(0)->Clone())));
	UOSInt i = 1;
	UOSInt j = this->fileList.GetCount();
	while (i < j)
	{
		fileList->AddFile(NotNullPtr<Crypto::Cert::X509File>::ConvertFrom(this->fileList.GetItem(i)->Clone()));
		i++;
	}
	return fileList;
}

Crypto::Cert::X509Cert *Crypto::Cert::X509FileList::CreateX509Cert() const
{
	if (this->fileList.GetCount() == 0)
		return 0;
	return (Crypto::Cert::X509Cert*)this->fileList.GetItem(0)->Clone().Ptr();
}

void Crypto::Cert::X509FileList::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	UOSInt i = 0;
	UOSInt j = this->fileList.GetCount();
	while (i < j)
	{
		this->fileList.GetItem(i)->ToString(sb);
		i++;
	}
}

Net::ASN1Names *Crypto::Cert::X509FileList::CreateNames() const
{
	return this->fileList.GetItem(0)->CreateNames();
}

void Crypto::Cert::X509FileList::AddFile(NotNullPtr<Crypto::Cert::X509File> file)
{
	this->fileList.Add(file);
}

UOSInt Crypto::Cert::X509FileList::GetFileCount() const
{
	return this->fileList.GetCount();
}

Crypto::Cert::X509File *Crypto::Cert::X509FileList::GetFile(UOSInt index) const
{
	return this->fileList.GetItem(index);
}

void Crypto::Cert::X509FileList::SetDefaultSourceName()
{
	Crypto::Cert::X509File *file;
	UOSInt j = INVALID_INDEX;
	UOSInt i = this->fileList.GetCount();
	while (i-- > 0)
	{
		file = this->fileList.GetItem(i);
		if (file->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
		{
			((Crypto::Cert::X509Cert*)file)->SetDefaultSourceName();
			j = i;
		}
	}
	if (j != INVALID_INDEX)
	{
		this->SetSourceName(this->fileList.GetItem(j)->GetSourceNameObj());
	}
}
