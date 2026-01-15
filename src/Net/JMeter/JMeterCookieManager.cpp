#include "Stdafx.h"
#include "Net/JMeter/JMeterCookieManager.h"
#include "Text/URLString.h"

typedef struct
{
	NN<Text::String> name;
	NN<Text::String> value;
	NN<Text::String> domain;
	Optional<Text::String> path;
	Int64 expireTime;
	Bool secure;
} HTTPCookie;

void __stdcall HTTPCookie_Free(NN<HTTPCookie> cookie)
{
	cookie->name->Release();
	cookie->value->Release();
	cookie->domain->Release();
	OPTSTR_DEL(cookie->path);
	MemFreeNN(cookie);
}

AnyType Net::JMeter::JMeterCookieManager::IterationBegin(NN<JMeterIteration> iter) const
{
	NN<Data::ArrayListNN<HTTPCookie>> cookieList;
	NEW_CLASSNN(cookieList, Data::ArrayListNN<HTTPCookie>());
	return cookieList;
}

void Net::JMeter::JMeterCookieManager::IterationEnd(NN<JMeterIteration> iter, AnyType data) const
{
	NN<Data::ArrayListNN<HTTPCookie>> cookieList = data.GetNN<Data::ArrayListNN<HTTPCookie>>();
	cookieList->FreeAll(HTTPCookie_Free);
	cookieList.Delete();
}

void Net::JMeter::JMeterCookieManager::HTTPBegin(NN<const JMeterIteration> iter, AnyType data, NN<Net::HTTPClient> cli) const
{
	NN<Data::ArrayListNN<HTTPCookie>> cookieList = data.GetNN<Data::ArrayListNN<HTTPCookie>>();
	NN<Text::String> url;
	if (cli->GetURL().SetTo(url))
	{
		Text::StringBuilderUTF8 sbCookie;
		UInt8 buff[4096];
		UnsafeArray<UTF8Char> sptr;
		NN<HTTPCookie> cookie;
		UIntOS len1;
		UIntOS len2;
		UIntOS i;
		UIntOS j;
		NN<Text::String> cpath;
		UnsafeArray<UTF8Char> pathPtr;
		UnsafeArray<UTF8Char> pathPtrEnd;
		sptr = Text::URLString::GetURLDomain(buff, url->ToCString(), 0);
		pathPtr = sptr + 1;
		pathPtrEnd = Text::URLString::GetURLPath(pathPtr, url->ToCString());
		len1 = (UIntOS)(sptr - buff);;
		i = 0;
		j = cookieList->GetCount();
		while (i < j)
		{
			cookie = cookieList->GetItemNoCheck(i);
			len2 = cookie->domain->leng;
			Bool valid = false;
			if (len1 == len2 && cookie->domain->Equals(buff, len1))
			{
				valid = true;
			}
			else if (len1 > len2 && buff[len1 - len2 - 1] == '.' && Text::StrEquals(&buff[len1 - len2], cookie->domain->v))
			{
				valid = true;
			}
			else if (len1 + 1 == len2 && cookie->domain->v[0] == '.' && Text::StrEquals(buff, &cookie->domain->v[1]))
			{
				valid = true;
			}
			if (valid)
			{
				if (!cookie->path.SetTo(cpath) || Text::StrStartsWithC(pathPtr, (UIntOS)(pathPtrEnd - pathPtr), cpath->v, cpath->leng))
				{
					if (sbCookie.leng > 0)
					{
						sbCookie.Append(CSTR("; "));
					}
					sbCookie.Append(cookie->name);
					sbCookie.AppendUTF8Char('=');
					sbCookie.Append(cookie->value);
				}
			}
			i++;
		}
		if (sbCookie.leng > 0)
		{
			cli->AddHeaderC(CSTR("Cookie"), sbCookie.ToCString());
		}
	}
}

void Net::JMeter::JMeterCookieManager::HTTPEnd(NN<const JMeterIteration> iter, AnyType data, NN<Net::HTTPClient> cli, Bool succ) const
{
	NN<Data::ArrayListNN<HTTPCookie>> cookieList = data.GetNN<Data::ArrayListNN<HTTPCookie>>();
	NN<Text::String> url;
	NN<Text::String> header;
	NN<HTTPCookie> cookie;
	Text::CStringNN cookieStr;	
	UTF8Char domain[512];
	UTF8Char path[512];
	UnsafeArray<UTF8Char> pathEnd;
	Text::PString sarr[2];
	UnsafeArray<UTF8Char> cookieValue;
	UnsafeArray<UTF8Char> cookieValueEnd;
	UIntOS cnt;
	UIntOS j;
	Bool secure;
	Int64 expiryTime;
	Bool valid;
	if (cli->GetURL().SetTo(url))
	{
		Text::StringBuilderUTF8 sb;
		UIntOS i = cli->GetRespHeaderCnt();
		while (i-- > 0)
		{
			if (cli->GetRespHeader(i).SetTo(header))
			{
				if (header->StartsWithICase(UTF8STRC("Set-Cookie: ")))
				{
					cookieStr = header->ToCString().Substring(12);
					secure = false;
					expiryTime = 0;
					valid = true;
					path[0] = 0;
					pathEnd = path;
					UnsafeArray<UTF8Char> domainEnd = Text::URLString::GetURLDomain(domain, url->ToCString(), 0);
					sb.ClearStr();
					sb.Append(cookieStr);
					cnt = Text::StrSplitTrimP(sarr, 2, sb, ';');
					cookieValue = sarr[0].v;
					cookieValueEnd = cookieValue + sarr[0].leng;
					j = Text::StrIndexOfCharC(cookieValue, sarr[0].leng, '=');
					if (j != INVALID_INDEX)
					{
						while (cnt == 2)
						{
							cnt = Text::StrSplitTrimP(sarr, 2, sarr[1], ';');
							if (Text::StrEqualsC(sarr[0].v, sarr[0].leng, UTF8STRC("Secure")))
							{
								secure = true;
							}
							else if (sarr[0].Equals(UTF8STRC("HttpOnly")))
							{

							}
							else if (sarr[0].StartsWith(UTF8STRC("SameSite=")))
							{

							}
							else if (sarr[0].StartsWith(UTF8STRC("Expires=")))
							{
								Data::DateTime dt;
								dt.SetValue(sarr[0].ToCString().Substring(8));
								expiryTime = dt.ToTicks();
							}
							else if (sarr[0].StartsWith(UTF8STRC("Max-Age=")))
							{
								Data::DateTime dt;
								dt.SetCurrTimeUTC();
								dt.AddSecond(Text::StrToIntOS(&sarr[0].v[8]));
								expiryTime = dt.ToTicks();
							}
							else if (sarr[0].StartsWith(UTF8STRC("Domain=")))
							{
								if (Text::StrEqualsICaseC(domain, (UIntOS)(domainEnd - domain), &sarr[0].v[7], sarr[0].leng - 7))
								{

								}
								else
								{
									UIntOS len1 = (UIntOS)(domainEnd - domain);
									UIntOS len2 = sarr[0].leng - 7;
									if (len1 > len2 && len2 > 0 && domain[len1 - len2 - 1] == '.' && Text::StrEquals(&domain[len1 - len2], &sarr[0].v[7]))
									{
										domainEnd = Text::StrConcatC(domain, &sarr[0].v[7], len2);
									}
									else if (len1 + 1 == len2 && sarr[0].v[7] == '.' && Text::StrEqualsC(domain, len1, &sarr[0].v[8], len2 - 1))
									{
										domainEnd = Text::StrConcatC(domain, &sarr[0].v[7], len2);
									}
									else
									{
										valid = false;
									}
								}
							}
							else if (sarr[0].StartsWith(UTF8STRC("Path=")))
							{
								pathEnd = Text::StrConcatC(path, &sarr[0].v[5], sarr[0].leng - 5);
							}
						}
						if (valid)
						{
							NN<Text::String> cookieName = Text::String::New(cookieValue, (UIntOS)j);
							NN<Text::String> cpath;
							Bool eq;
							Bool found = false;
							UIntOS k = cookieList->GetCount();
							while (k-- > 0)
							{
								cookie = cookieList->GetItemNoCheck(k);
								eq = cookie->domain->Equals(domain, (UIntOS)(domainEnd - domain)) && cookie->secure == secure && cookie->name->Equals(cookieName);
								if (!cookie->path.SetTo(cpath))
								{
									eq = eq && (path[0] == 0);
								}
								else
								{
									eq = eq && cpath->Equals(path, (UIntOS)(pathEnd - path));
								}
								if (eq)
								{
									cookie->value->Release();
									cookie->value  = Text::String::NewP(&cookieValue[j + 1], cookieValueEnd);
									cookieName->Release();
									found = true;
								}
							}
							if (!found)
							{
								cookie = MemAllocNN(HTTPCookie);
								cookie->domain = Text::String::NewP(domain, domainEnd);
								if (path[0])
								{
									cookie->path = Text::String::New(path, (UIntOS)(pathEnd - path));
								}
								else
								{
									cookie->path = nullptr;
								}
								cookie->secure = secure;
								cookie->expireTime = expiryTime;
								cookie->name = cookieName;
								cookie->value = Text::String::NewP(&cookieValue[j + 1], cookieValueEnd);
								cookieList->Add(cookie);
							}
						}
					}
				}
			}
		}
	}
}
