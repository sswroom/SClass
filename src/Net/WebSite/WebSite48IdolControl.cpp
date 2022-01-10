#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/WebSite/WebSite48IdolControl.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"

//#define BASEURL "https://48idol.net/"
#define BASEURL "https://48idol.tv/archive/"
#define TVBASEURL "https://48idol.tv/all-videos"

Net::WebSite::WebSite48IdolControl::WebSite48IdolControl(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::String *userAgent)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->userAgent = SCOPY_STRING(userAgent);
}

Net::WebSite::WebSite48IdolControl::~WebSite48IdolControl()
{
	SDEL_STRING(this->userAgent);
}

OSInt Net::WebSite::WebSite48IdolControl::GetTVPageItems(OSInt pageNo, Data::ArrayList<Net::WebSite::WebSite48IdolControl::ItemData*> *itemList)
{
	Text::StringBuilderUTF8 sb;
	OSInt retCnt = 0;
	if (pageNo <= 0)
		return 0;
	if (pageNo == 1)
	{
		sb.Append((const UTF8Char*)TVBASEURL);
	}
	else
	{
		sb.Append((const UTF8Char*)TVBASEURL);
		sb.AppendC(UTF8STRC("/page/"));
		sb.AppendOSInt(pageNo);
	}
	ItemData *item;
	Text::XMLReader *reader;
	Text::XMLAttrib *attr;
	Text::XMLAttrib *attr1;
	Text::XMLAttrib *attr2;
	Text::XMLAttrib *attr3;
	Data::DateTime dt;
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, STR_PTRC(this->userAgent), true, true);
	cli->Connect(sb.ToString(), sb.GetLength(), "GET", 0, 0, true);
	NEW_CLASS(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals((const UTF8Char*)"main") && reader->GetAttribCount() == 2)
		{
			attr = reader->GetAttrib((UOSInt)0);
			if (attr && attr->name->Equals((const UTF8Char*)"id") && attr->value && attr->value->Equals((const UTF8Char*)"main-content"))
			{
				UOSInt pathLev = reader->GetPathLev();
				while (reader->ReadNext() && reader->GetPathLev() > pathLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals((const UTF8Char*)"a") && reader->GetAttribCount() == 4)
					{
						attr = reader->GetAttrib((UOSInt)0);
						attr1 = reader->GetAttrib(1);
						attr2 = reader->GetAttrib(2);
						attr3 = reader->GetAttrib(3);
						if (attr &&
							attr1 &&
							attr2 &&
							attr3 &&
							attr->name->Equals((const UTF8Char*)"data-post-id") &&
							attr1->name->Equals((const UTF8Char*)"href") &&
							attr2->name->Equals((const UTF8Char*)"title") &&
							attr3->name->Equals((const UTF8Char*)"class") &&
							attr->value &&
							attr1->value &&
							attr2->value &&
							attr3->value &&
							attr3->value->Equals((const UTF8Char*)"blog-img"))
						{
							item = MemAlloc(ItemData, 1);
							item->id = attr->value->ToInt32();
							item->recTime = 0;
							item->title = attr2->value->Clone();
							itemList->Add(item);
							retCnt++;
						}
					}
				}
			}
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	return retCnt;
}

OSInt Net::WebSite::WebSite48IdolControl::GetArcPageItems(OSInt pageNo, Data::ArrayList<Net::WebSite::WebSite48IdolControl::ItemData*> *itemList)
{
	Text::StringBuilderUTF8 sb;
	OSInt retCnt = 0;
	if (pageNo <= 0)
		return 0;
	if (pageNo == 1)
	{
		sb.Append((const UTF8Char*)BASEURL "videos");
	}
	else
	{
		sb.Append((const UTF8Char*)BASEURL);
		sb.AppendC(UTF8STRC("videos?page="));
		sb.AppendOSInt(pageNo);
	}
	ItemData *item;
	Text::XMLReader *reader;
	Text::XMLAttrib *attr;
	Data::DateTime dt;
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, STR_PTRC(this->userAgent), true, true);
	cli->Connect(sb.ToString(), sb.GetLength(), "GET", 0, 0, true);
	NEW_CLASS(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals((const UTF8Char*)"div"))
		{
			attr = reader->GetAttrib((UOSInt)0);
			if (attr && attr->name->Equals((const UTF8Char*)"class") && attr->value && attr->value->Equals((const UTF8Char*)"post-des"))
			{
				UOSInt pathLev = reader->GetPathLev();
				Bool lastIsH6 = false;
				Int32 id = 0;
				Int64 time = 0;
				Text::String *title = 0;
				UOSInt pullLeftLev = 0;
				while (reader->ReadNext() && reader->GetPathLev() > pathLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (reader->GetNodeText()->Equals((const UTF8Char*)"h6"))
						{
							lastIsH6 = true;
						}
						else if (lastIsH6 && reader->GetNodeText()->Equals((const UTF8Char*)"a"))
						{
							attr = reader->GetAttrib((UOSInt)0);
							if (reader->GetAttribCount() == 1 && attr && attr->name->Equals((const UTF8Char*)"href") && attr->value && attr->value->StartsWith((const UTF8Char*)BASEURL))
							{
								id = Text::StrToInt32(&attr->value->v[sizeof(BASEURL) + 5]);
								sb.ClearStr();
								reader->ReadNodeText(&sb);
								SDEL_STRING(title);
								title = Text::String::New(sb.ToString(), sb.GetLength());
							}
						}
						else
						{
							lastIsH6 = false;
							if (reader->GetNodeText()->Equals((const UTF8Char*)"p"))
							{
								attr = reader->GetAttrib((UOSInt)0);
								if (attr && attr->name->Equals((const UTF8Char*)"class") && attr->value && attr->value->Equals((const UTF8Char*)"pull-left"))
								{
									pullLeftLev = reader->GetPathLev();
								}
							}
							else if (pullLeftLev > 0 && reader->GetNodeText()->Equals((const UTF8Char*)"span"))
							{
								sb.ClearStr();
								reader->ReadNodeText(&sb);
								sb.Trim();
								dt.SetValue(sb.ToString());
								time = dt.ToTicks();
							}
						}
					}
					else
					{
						lastIsH6 = false;
						if (reader->GetPathLev() <= pullLeftLev)
						{
							pullLeftLev = 0;
						}
					}
				}
				if (id != 0 && time != 0 && title != 0)
				{
					item = MemAlloc(ItemData, 1);
					item->id = id;
					item->recTime = time;
					item->title = title;
					title = 0;
					itemList->Add(item);
					retCnt++;
				}
				SDEL_STRING(title);
			}
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	return retCnt;
}

void Net::WebSite::WebSite48IdolControl::FreeItems(Data::ArrayList<Net::WebSite::WebSite48IdolControl::ItemData*> *itemList)
{
	ItemData *item;
	UOSInt i = itemList->GetCount();
	while (i-- > 0)
	{
		item = itemList->GetItem(i);
		item->title->Release();
		MemFree(item);
	}
	itemList->Clear();
}

Bool Net::WebSite::WebSite48IdolControl::GetDownloadLink(Int32 videoId, Int32 linkId, Text::StringBuilderUTF *link)
{
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)BASEURL "video/");
	sb.AppendI32(videoId);
	Text::XMLReader *reader;
	Text::XMLAttrib *attr;
	Bool found = false;
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, STR_PTRC(this->userAgent), true, true);
	cli->Connect(sb.ToString(), sb.GetLength(), "GET", 0, 0, true);
	NEW_CLASS(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (!found && reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals((const UTF8Char*)"button") && reader->GetAttribCount() > 0)
		{
			attr = reader->GetAttrib((UOSInt)0);
			if (attr->name->Equals((const UTF8Char*)"id") && attr->value && attr->value->Equals((const UTF8Char*)"ddb"))
			{
				attr = reader->GetAttrib(1);
				if (attr && attr->name->Equals((const UTF8Char*)"onclick") && attr->value && attr->value->StartsWith((const UTF8Char*)"window.open('"))
				{
					if (linkId == 0)
					{
						UOSInt i = Text::StrIndexOf(&attr->value->v[13], '\'');
						if (i != INVALID_INDEX)
						{
							link->AppendC(&attr->value->v[13], i);
							found = true;
						}
					}
					else
					{
						linkId--;
					}
				}
			}
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	return found;
}

Bool Net::WebSite::WebSite48IdolControl::GetVideoName(Int32 videoId, Text::StringBuilderUTF *name)
{
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)BASEURL "video/");
	sb.AppendI32(videoId);
	Text::XMLReader *reader;
	Text::XMLAttrib *attr;
	Bool found = false;
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, STR_PTRC(this->userAgent), true, true);
	cli->Connect(sb.ToString(), sb.GetLength(), "GET", 0, 0, true);
	NEW_CLASS(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (!found && reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals((const UTF8Char*)"div") && reader->GetAttribCount() > 0)
		{
			attr = reader->GetAttrib((UOSInt)0);
			if (attr->name->Equals((const UTF8Char*)"class") && attr->value && attr->value->Equals((const UTF8Char*)"post-title"))
			{
				UOSInt initLev = reader->GetPathLev();
				while (reader->ReadNext() && reader->GetPathLev() > initLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals((const UTF8Char*)"h2"))
					{
						found = reader->ReadNodeText(name);
						break;
					}
				}
			}
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	return found;
}

void Net::WebSite::WebSite48IdolControl::Title2DisplayName(Text::String *title, Text::StringBuilderUTF *dispName)
{
	UOSInt i;
	Text::StringBuilderUTF8 sb;
	sb.Append(title);
	sb.Trim();
	while (sb.EndsWith(')'))
	{
		i = sb.LastIndexOf('(');
		if (i == INVALID_INDEX)
			break;
		if (Text::StrStartsWith(&sb.ToString()[i], (const UTF8Char*)"(GYAO"))
		{
			break;
		}
		sb.TrimToLength(i);
		sb.Trim();
	}

	i = sb.IndexOf('|');
	if (i != INVALID_INDEX)
	{
		sb.TrimToLength(i);
		sb.Trim();
	}
	UTF8Char sbuff[4];
	sbuff[0] = 0xEF;
	sbuff[1] = 0xBC;
	sbuff[2] = 0x9F;
	sbuff[3] = 0;
	sb.Replace((const UTF8Char*)"?", sbuff);
	sbuff[0] = 0xEF;
	sbuff[1] = 0xBC;
	sbuff[2] = 0x9A;
	sb.Replace((const UTF8Char*)":", sbuff);
	while (sb.EndsWith(')'))
	{
		i = sb.LastIndexOf('(');
		if (i == INVALID_INDEX)
			break;
		if (Text::StrStartsWith(&sb.ToString()[i], (const UTF8Char*)"(GYAO"))
		{
			break;
		}
		sb.TrimToLength((UOSInt)i);
		sb.Trim();
	}

	if (sb.ToString()[0] >= '0' && sb.ToString()[0] <= '9' && sb.ToString()[6] == ' ')
	{
		dispName->Append(&sb.ToString()[7]);
		dispName->AppendC(UTF8STRC(" 20"));
		dispName->AppendC(sb.ToString(), 6);
	}
	else if (sb.StartsWith((const UTF8Char*)"[BD/DVD] "))
	{
		dispName->Append(sb.ToString() + 9);
	}
	else
	{
		dispName->AppendC(sb.ToString(), sb.GetLength());
	}
}
