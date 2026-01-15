#include "Stdafx.h"
#include "Crypto/Cert/X509FileList.h"
#include "Net/SSLEngine.h"

Crypto::Cert::X509FileList::X509FileList(NN<Text::String> sourceName, NN<Crypto::Cert::X509Cert> cert) : Crypto::Cert::X509File(sourceName, cert->GetASN1Array())
{
	this->fileList.Add(cert);
}

Crypto::Cert::X509FileList::X509FileList(Text::CStringNN sourceName, NN<Crypto::Cert::X509Cert> cert) : Crypto::Cert::X509File(sourceName, cert->GetASN1Array())
{
	this->fileList.Add(cert);
}

Crypto::Cert::X509FileList::~X509FileList()
{
	UOSInt i = this->fileList.GetCount();
	while (i-- > 0)
	{
		this->fileList.GetItem(i).Delete();
	}
}

Crypto::Cert::X509File::FileType Crypto::Cert::X509FileList::GetFileType() const
{
	return Crypto::Cert::X509File::FileType::FileList;
}

void Crypto::Cert::X509FileList::ToShortName(NN<Text::StringBuilderUTF8> sb) const
{
	return this->fileList.GetItem(0).OrNull()->ToShortName(sb);
}

UOSInt Crypto::Cert::X509FileList::GetCertCount()
{
	UOSInt ret = 0;
	Data::ArrayIterator<NN<X509File>> it = this->fileList.Iterator();
	while (it.HasNext())
	{
		ret += it.Next()->GetCertCount();
	}
	return ret;
}

Bool Crypto::Cert::X509FileList::GetCertName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	Data::ArrayIterator<NN<X509File>> it = this->fileList.Iterator();
	UOSInt cnt;
	NN<Crypto::Cert::X509File> file;
	while (it.HasNext())
	{
		file = it.Next();
		cnt = file->GetCertCount();
		if (index < cnt)
		{
			return file->GetCertName(index, sb);
		}
		index -= cnt;
	}
	return false;
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::X509FileList::GetNewCert(UOSInt index)
{
	Data::ArrayIterator<NN<X509File>> it = this->fileList.Iterator();
	UOSInt cnt;
	NN<Crypto::Cert::X509File> file;
	while (it.HasNext())
	{
		file = it.Next();
		cnt = file->GetCertCount();
		if (index < cnt)
		{
			return file->GetNewCert(index);
		}
		index -= cnt;
	}
	return nullptr;
}

Crypto::Cert::X509File::ValidStatus Crypto::Cert::X509FileList::IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const
{
	Crypto::Cert::X509File::ValidStatus status;
	UOSInt i = this->fileList.GetCount();
	if (i > 1)
	{
		NN<Crypto::Cert::X509File> file;
		NN<Crypto::Cert::CertStore> trusts;
		if (!trustStore.SetTo(trusts))
		{
			trusts = ssl->GetTrustStore();
		}
		trusts = trusts->Clone();
		Data::ArrayIterator<NN<X509File>> it = this->fileList.Iterator();
		while (it.HasNext())
		{
			file = it.Next();
			status = file->IsValid(ssl, trusts);
			if (status != Crypto::Cert::X509File::ValidStatus::Valid)
			{
				trusts.Delete();
				return status;
			}
			if (file->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
			{
				trusts->AddCert(NN<Crypto::Cert::X509Cert>::ConvertFrom(file->Clone()));
			}
		}
		trusts.Delete();
		return Crypto::Cert::X509File::ValidStatus::Valid;
	}
	else
	{
		return this->fileList.GetItem(0).OrNull()->IsValid(ssl, trustStore);
	}
}

NN<Net::ASN1Data> Crypto::Cert::X509FileList::Clone() const
{
	NN<Crypto::Cert::X509FileList> fileList;
	Data::ArrayIterator<NN<X509File>> it = this->fileList.Iterator();
	NEW_CLASSNN(fileList, Crypto::Cert::X509FileList(this->GetSourceNameObj(), NN<Crypto::Cert::X509Cert>::ConvertFrom(it.Next()->Clone())));
	while (it.HasNext())
	{
		fileList->AddFile(NN<Crypto::Cert::X509File>::ConvertFrom(it.Next()->Clone()));
	}
	return fileList;
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::X509FileList::CreateX509Cert() const
{
	NN<Crypto::Cert::X509File> file;
	if (!this->fileList.GetItem(0).SetTo(file))
		return nullptr;
	return NN<Crypto::Cert::X509Cert>::ConvertFrom(file->Clone());
}

void Crypto::Cert::X509FileList::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	Data::ArrayIterator<NN<X509File>> it = this->fileList.Iterator();
	while (it.HasNext())
	{
		it.Next()->ToString(sb);
	}
}

NN<Net::ASN1Names> Crypto::Cert::X509FileList::CreateNames() const
{
	NN<X509File> file;
	if (this->fileList.GetItem(0).SetTo(file))
	{
		return file->CreateNames();
	}
	NN<Net::ASN1Names> names;
	NEW_CLASSNN(names, Net::ASN1Names())
	return names;
}

void Crypto::Cert::X509FileList::AddFile(NN<Crypto::Cert::X509File> file)
{
	this->fileList.Add(file);
}

UOSInt Crypto::Cert::X509FileList::GetFileCount() const
{
	return this->fileList.GetCount();
}

Optional<Crypto::Cert::X509File> Crypto::Cert::X509FileList::GetFile(UOSInt index) const
{
	return this->fileList.GetItem(index);
}

void Crypto::Cert::X509FileList::SetDefaultSourceName()
{
	NN<Crypto::Cert::X509File> file;
	UOSInt j = INVALID_INDEX;
	UOSInt i = this->fileList.GetCount();
	while (i-- > 0)
	{
		if (this->fileList.GetItem(i).SetTo(file) && file->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
		{
			NN<Crypto::Cert::X509Cert>::ConvertFrom(file)->SetDefaultSourceName();
			j = i;
		}
	}
	if (j != INVALID_INDEX)
	{
		this->SetSourceName(this->fileList.GetItem(j).OrNull()->GetSourceNameObj());
	}
}
