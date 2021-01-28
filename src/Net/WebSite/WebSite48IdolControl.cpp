#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/WebSite/WebSite48IdolControl.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"

//#define BASEURL "https://48idol.net/"
#define BASEURL "https://48idol.tv/archive/"
#define TVBASEURL "https://48idol.tv/all-videos"

Net::WebSite::WebSite48IdolControl::WebSite48IdolControl(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, const UTF8Char *userAgent)
{
	this->sockf = sockf;
	this->encFact = encFact;
	this->userAgent = Text::StrCopyNew(userAgent);
}

Net::WebSite::WebSite48IdolControl::~WebSite48IdolControl()
{
	Text::StrDelNew(this->userAgent);
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
		sb.Append((const UTF8Char*)"/page/");
		sb.AppendOSInt(pageNo);
	}
	ItemData *item;
	Text::XMLReader *reader;
	Text::XMLAttrib *attr;
	Text::XMLAttrib *attr1;
	Text::XMLAttrib *attr2;
	Text::XMLAttrib *attr3;
	Data::DateTime dt;
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->userAgent, true, true);
	cli->Connect(sb.ToString(), "GET", 0, 0, true);
	NEW_CLASS(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"main") && reader->GetAttribCount() == 2)
		{
			attr = reader->GetAttrib((OSInt)0);
			if (attr && Text::StrEquals(attr->name, (const UTF8Char*)"id") && attr->value && Text::StrEquals(attr->value, (const UTF8Char*)"main-content"))
			{
				OSInt pathLev = reader->GetPathLev();
				while (reader->ReadNext() && reader->GetPathLev() > pathLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"a") && reader->GetAttribCount() == 4)
					{
						attr = reader->GetAttrib((OSInt)0);
						attr1 = reader->GetAttrib(1);
						attr2 = reader->GetAttrib(2);
						attr3 = reader->GetAttrib(3);
						if (attr &&
							attr1 &&
							attr2 &&
							attr3 &&
							Text::StrEquals(attr->name, (const UTF8Char*)"data-post-id") &&
							Text::StrEquals(attr1->name, (const UTF8Char*)"href") &&
							Text::StrEquals(attr2->name, (const UTF8Char*)"title") &&
							Text::StrEquals(attr3->name, (const UTF8Char*)"class") &&
							attr->value &&
							attr1->value &&
							attr2->value &&
							attr3->value &&
							Text::StrEquals(attr3->value, (const UTF8Char*)"blog-img"))
						{
							item = MemAlloc(ItemData, 1);
							item->id = Text::StrToInt32(attr->value);
							item->recTime = 0;
							item->title = Text::StrCopyNew(attr2->value);
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
		sb.Append((const UTF8Char*)"videos?page=");
		sb.AppendOSInt(pageNo);
	}
	ItemData *item;
	Text::XMLReader *reader;
	Text::XMLAttrib *attr;
	Data::DateTime dt;
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->userAgent, true, true);
	cli->Connect(sb.ToString(), "GET", 0, 0, true);
	NEW_CLASS(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"div"))
		{
			attr = reader->GetAttrib((OSInt)0);
			if (attr && Text::StrEquals(attr->name, (const UTF8Char*)"class") && attr->value && Text::StrEquals(attr->value, (const UTF8Char*)"post-des"))
			{
				OSInt pathLev = reader->GetPathLev();
				Bool lastIsH6 = false;
				Int32 id = 0;
				Int64 time = 0;
				const UTF8Char *title = 0;
				OSInt pullLeftLev = 0;
				while (reader->ReadNext() && reader->GetPathLev() > pathLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"h6"))
						{
							lastIsH6 = true;
						}
						else if (lastIsH6 && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"a"))
						{
							attr = reader->GetAttrib((OSInt)0);
							if (reader->GetAttribCount() == 1 && attr && Text::StrEquals(attr->name, (const UTF8Char*)"href") && attr->value && Text::StrStartsWith(attr->value, (const UTF8Char*)BASEURL))
							{
								id = Text::StrToInt32(&attr->value[sizeof(BASEURL) + 5]);
								sb.ClearStr();
								reader->ReadNodeText(&sb);
								SDEL_TEXT(title);
								title = Text::StrCopyNew(sb.ToString());
							}
						}
						else
						{
							lastIsH6 = false;
							if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"p"))
							{
								attr = reader->GetAttrib((OSInt)0);
								if (attr && Text::StrEquals(attr->name, (const UTF8Char*)"class") && attr->value && Text::StrEquals(attr->value, (const UTF8Char*)"pull-left"))
								{
									pullLeftLev = reader->GetPathLev();
								}
							}
							else if (pullLeftLev > 0 && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"span"))
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
				SDEL_TEXT(title);
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
	OSInt i = itemList->GetCount();
	while (i-- > 0)
	{
		item = itemList->GetItem(i);
		Text::StrDelNew(item->title);
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
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->userAgent, true, true);
	cli->Connect(sb.ToString(), "GET", 0, 0, true);
	NEW_CLASS(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (!found && reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"button") && reader->GetAttribCount() > 0)
		{
			attr = reader->GetAttrib((OSInt)0);
			if (Text::StrEquals(attr->name, (const UTF8Char*)"id") && attr->value && Text::StrEquals(attr->value, (const UTF8Char*)"ddb"))
			{
				attr = reader->GetAttrib(1);
				if (attr && Text::StrEquals(attr->name, (const UTF8Char*)"onclick") && attr->value && Text::StrStartsWith(attr->value, (const UTF8Char*)"window.open('"))
				{
					if (linkId == 0)
					{
						OSInt i = Text::StrIndexOf(&attr->value[13], '\'');
						link->AppendC(&attr->value[13], i);
						found = true;
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
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->userAgent, true, true);
	cli->Connect(sb.ToString(), "GET", 0, 0, true);
	NEW_CLASS(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (!found && reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"div") && reader->GetAttribCount() > 0)
		{
			attr = reader->GetAttrib((OSInt)0);
			if (Text::StrEquals(attr->name, (const UTF8Char*)"class") && attr->value && Text::StrEquals(attr->value, (const UTF8Char*)"post-title"))
			{
				OSInt initLev = reader->GetPathLev();
				while (reader->ReadNext() && reader->GetPathLev() > initLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"h2"))
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

void Net::WebSite::WebSite48IdolControl::Title2DisplayName(const UTF8Char *title, Text::StringBuilderUTF *dispName)
{
	OSInt i;
	Text::StringBuilderUTF8 sb;
	sb.Append(title);
	sb.Trim();
	while (sb.EndsWith(')'))
	{
		i = sb.LastIndexOf('(');
		if (i < 0)
			break;
		if (Text::StrStartsWith(&sb.ToString()[i], (const UTF8Char*)"(GYAO"))
		{
			break;
		}
		sb.TrimToLength(i);
		sb.Trim();
	}

	i = sb.IndexOf('|');
	if (i >= 0)
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
		if (i < 0)
			break;
		if (Text::StrStartsWith(&sb.ToString()[i], (const UTF8Char*)"(GYAO"))
		{
			break;
		}
		sb.TrimToLength(i);
		sb.Trim();
	}

	if (sb.ToString()[0] >= '0' && sb.ToString()[0] <= '9' && sb.ToString()[6] == ' ')
	{
		dispName->Append(&sb.ToString()[7]);
		dispName->Append((const UTF8Char*)" 20");
		dispName->AppendC(sb.ToString(), 6);
	}
	else if (sb.StartsWith((const UTF8Char*)"[BD/DVD] "))
	{
		dispName->Append(sb.ToString() + 9);
	}
	else
	{
		dispName->Append(sb.ToString());
	}
}
