#include "Stdafx.h"
#include "Crypto/Cert/CurlCert.h"
#include <curl/curl.h>

Crypto::Cert::CurlCert::CurlCert(void *certinfo)
{
	this->certinfo = certinfo;
}

Crypto::Cert::CurlCert::~CurlCert()
{

}

void Crypto::Cert::CurlCert::ToString(Text::StringBuilderUTF *sb)
{
	curl_slist *slist = (curl_slist*)this->certinfo;
	while (slist)
	{
		sb->Append((const UTF8Char*)slist->data);
		sb->Append((const UTF8Char*)"\r\n");
		slist = slist->next;
	}
}
