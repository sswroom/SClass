#include "Stdafx.h"
#include "Net/Email/EmailStore.h"
#include "Sync/MutexUsage.h"

Net::Email::EmailStore::EmailStore()
{

}

Net::Email::EmailStore::~EmailStore()
{
	EmailInfo *email;
	UOSInt i;
	i = this->mailList.GetCount();
	while (i-- > 0)
	{
		email = this->mailList.GetItem(i);
		email->fromAddr->Release();
		MemFree(email);
	}
}

UOSInt Net::Email::EmailStore::GetAllEmails(Data::ArrayList<EmailInfo*> *emailList)
{
	Sync::MutexUsage mutUsage(&this->mailMut);
	emailList->AddAll(&this->mailList);
	return this->mailList.GetCount();
}
