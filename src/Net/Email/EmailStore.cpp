#include "Stdafx.h"
#include "Net/Email/EmailStore.h"
#include "Sync/MutexUsage.h"

Net::Email::EmailStore::EmailStore()
{

}

Net::Email::EmailStore::~EmailStore()
{
	NN<EmailInfo> email;
	UIntOS i;
	i = this->mailList.GetCount();
	while (i-- > 0)
	{
		email = this->mailList.GetItemNoCheck(i);
		email->fromAddr->Release();
		MemFreeNN(email);
	}
}

UIntOS Net::Email::EmailStore::GetAllEmails(NN<Data::ArrayListNN<EmailInfo>> emailList)
{
	Sync::MutexUsage mutUsage(this->mailMut);
	emailList->AddAll(this->mailList);
	return this->mailList.GetCount();
}
