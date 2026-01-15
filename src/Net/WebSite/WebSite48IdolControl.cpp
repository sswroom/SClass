#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/WebSite/WebSite48IdolControl.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"

//#define BASEURL "https://48idol.net/"
#define BASEURL "https://48idol.tv/archive/"
#define TVBASEURL "https://48idol.tv/all-videos"

Net::WebSite::WebSite48IdolControl::WebSite48IdolControl(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Text::String *userAgent)
{
	this->clif = clif;
	this->ssl = ssl;
	this->encFact = encFact;
	this->userAgent = SCOPY_STRING(userAgent);
}

Net::WebSite::WebSite48IdolControl::~WebSite48IdolControl()
{
	SDEL_STRING(this->userAgent);
}

IntOS Net::WebSite::WebSite48IdolControl::GetTVPageItems(IntOS pageNo, NN<Data::ArrayListNN<Net::WebSite::WebSite48IdolControl::ItemData>> itemList)
{
	Text::StringBuilderUTF8 sb;
	IntOS retCnt = 0;
	if (pageNo <= 0)
		return 0;
	if (pageNo == 1)
	{
		sb.AppendC(UTF8STRC(TVBASEURL));
	}
	else
	{
		sb.AppendC(UTF8STRC(TVBASEURL));
		sb.AppendC(UTF8STRC("/page/"));
		sb.AppendIntOS(pageNo);
	}
	NN<ItemData> item;
	NN<Text::XMLAttrib> attr;
	NN<Text::XMLAttrib> attr1;
	NN<Text::XMLAttrib> attr2;
	NN<Text::XMLAttrib> attr3;
	Data::DateTime dt;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(this->clif, this->ssl, {STR_PTRC(this->userAgent)}, true, true);
	cli->Connect(sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	Text::XMLReader reader(this->encFact, cli, Text::XMLReader::PM_HTML);
	while (reader.ReadNext())
	{
		if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("main")) && reader.GetAttribCount() == 2)
		{
			if (reader.GetAttrib((UIntOS)0).SetTo(attr) && Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("id")) && Text::String::OrEmpty(attr->value)->Equals(UTF8STRC("main-content")))
			{
				UIntOS pathLev = reader.GetPathLev();
				while (reader.ReadNext() && reader.GetPathLev() > pathLev)
				{
					if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("a")) && reader.GetAttribCount() == 4)
					{
						if (reader.GetAttrib((UIntOS)0).SetTo(attr) &&
							reader.GetAttrib(1).SetTo(attr1) &&
							reader.GetAttrib(2).SetTo(attr2) &&
							reader.GetAttrib(3).SetTo(attr3) &&
							Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("data-post-id")) &&
							Text::String::OrEmpty(attr1->name)->Equals(UTF8STRC("href")) &&
							Text::String::OrEmpty(attr2->name)->Equals(UTF8STRC("title")) &&
							Text::String::OrEmpty(attr3->name)->Equals(UTF8STRC("class")) &&
							attr->value.NotNull() &&
							attr1->value.NotNull() &&
							attr2->value.NotNull() &&
							Text::String::OrEmpty(attr3->value)->Equals(UTF8STRC("blog-img")))
						{
							item = MemAllocNN(ItemData);
							item->id = Text::String::OrEmpty(attr->value)->ToInt32();
							item->recTime = 0;
							item->title = Text::String::OrEmpty(attr2->value)->Clone();
							itemList->Add(item);
							retCnt++;
						}
					}
				}
			}
		}
	}
	cli.Delete();
	return retCnt;
}

IntOS Net::WebSite::WebSite48IdolControl::GetArcPageItems(IntOS pageNo, NN<Data::ArrayListNN<Net::WebSite::WebSite48IdolControl::ItemData>> itemList)
{
	Text::StringBuilderUTF8 sb;
	IntOS retCnt = 0;
	if (pageNo <= 0)
		return 0;
	if (pageNo == 1)
	{
		sb.AppendC(UTF8STRC(BASEURL "videos"));
	}
	else
	{
		sb.AppendC(UTF8STRC(BASEURL));
		sb.AppendC(UTF8STRC("videos?page="));
		sb.AppendIntOS(pageNo);
	}
	NN<ItemData> item;
	NN<Text::XMLAttrib> attr;
	Data::DateTime dt;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(this->clif, this->ssl, {STR_PTRC(this->userAgent)}, true, true);
	cli->Connect(sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	Text::XMLReader reader(this->encFact, cli, Text::XMLReader::PM_HTML);
	while (reader.ReadNext())
	{
		if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("div")))
		{
			if (reader.GetAttrib((UIntOS)0).SetTo(attr) && Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("class")) && Text::String::OrEmpty(attr->value)->Equals(UTF8STRC("post-des")))
			{
				UIntOS pathLev = reader.GetPathLev();
				Bool lastIsH6 = false;
				Int32 id = 0;
				Int64 time = 0;
				Text::String *title = 0;
				UIntOS pullLeftLev = 0;
				while (reader.ReadNext() && reader.GetPathLev() > pathLev)
				{
					if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
					{
						if (reader.GetNodeTextNN()->Equals(UTF8STRC("h6")))
						{
							lastIsH6 = true;
						}
						else if (lastIsH6 && reader.GetNodeTextNN()->Equals(UTF8STRC("a")))
						{
							if (reader.GetAttribCount() == 1 && reader.GetAttrib((UIntOS)0).SetTo(attr) && Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("href")) && Text::String::OrEmpty(attr->value)->StartsWith(UTF8STRC(BASEURL)))
							{
								id = Text::StrToInt32(&Text::String::OrEmpty(attr->value)->v[(UIntOS)sizeof(BASEURL) + 5]);
								sb.ClearStr();
								reader.ReadNodeText(sb);
								SDEL_STRING(title);
								title = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
							}
						}
						else
						{
							lastIsH6 = false;
							if (reader.GetNodeTextNN()->Equals(UTF8STRC("p")))
							{
								if (reader.GetAttrib((UIntOS)0).SetTo(attr) && Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("class")) && Text::String::OrEmpty(attr->value)->Equals(UTF8STRC("pull-left")))
								{
									pullLeftLev = reader.GetPathLev();
								}
							}
							else if (pullLeftLev > 0 && reader.GetNodeTextNN()->Equals(UTF8STRC("span")))
							{
								sb.ClearStr();
								reader.ReadNodeText(sb);
								sb.Trim();
								dt.SetValue(sb.ToCString());
								time = dt.ToTicks();
							}
						}
					}
					else
					{
						lastIsH6 = false;
						if (reader.GetPathLev() <= pullLeftLev)
						{
							pullLeftLev = 0;
						}
					}
				}
				NN<Text::String> titleStr;
				if (id != 0 && time != 0 && titleStr.Set(title))
				{
					item = MemAllocNN(ItemData);
					item->id = id;
					item->recTime = time;
					item->title = titleStr;
					title = 0;
					itemList->Add(item);
					retCnt++;
				}
				SDEL_STRING(title);
			}
		}
	}
	cli.Delete();
	return retCnt;
}

void Net::WebSite::WebSite48IdolControl::FreeItems(NN<Data::ArrayListNN<Net::WebSite::WebSite48IdolControl::ItemData>> itemList)
{
	NN<ItemData> item;
	UIntOS i = itemList->GetCount();
	while (i-- > 0)
	{
		item = itemList->GetItemNoCheck(i);
		item->title->Release();
		MemFreeNN(item);
	}
	itemList->Clear();
}

Bool Net::WebSite::WebSite48IdolControl::GetDownloadLink(Int32 videoId, Int32 linkId, NN<Text::StringBuilderUTF8> link)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC(BASEURL "video/"));
	sb.AppendI32(videoId);
	NN<Text::XMLAttrib> attr;
	Bool found = false;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(this->clif, this->ssl, {STR_PTRC(this->userAgent)}, true, true);
	cli->Connect(sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	Text::XMLReader reader(this->encFact, cli, Text::XMLReader::PM_HTML);
	while (!found && reader.ReadNext())
	{
		if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("button")) && reader.GetAttribCount() > 0)
		{
			attr = reader.GetAttribNoCheck((UIntOS)0);
			if (Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("id")) && Text::String::OrEmpty(attr->value)->Equals(UTF8STRC("ddb")))
			{
				if (reader.GetAttrib(1).SetTo(attr) && Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("onclick")) && Text::String::OrEmpty(attr->value)->StartsWith(UTF8STRC("window.open('")))
				{
					if (linkId == 0)
					{
						UIntOS i = Text::StrIndexOfChar(&Text::String::OrEmpty(attr->value)->v[13], '\'');
						if (i != INVALID_INDEX)
						{
							link->AppendC(&Text::String::OrEmpty(attr->value)->v[13], i);
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
	cli.Delete();
	return found;
}

Bool Net::WebSite::WebSite48IdolControl::GetVideoName(Int32 videoId, NN<Text::StringBuilderUTF8> name)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC(BASEURL "video/"));
	sb.AppendI32(videoId);
	NN<Text::XMLAttrib> attr;
	Bool found = false;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(this->clif, this->ssl, {STR_PTRC(this->userAgent)}, true, true);
	cli->Connect(sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	Text::XMLReader reader(this->encFact, cli, Text::XMLReader::PM_HTML);
	while (!found && reader.ReadNext())
	{
		if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("div")) && reader.GetAttribCount() > 0)
		{
			attr = reader.GetAttribNoCheck((UIntOS)0);
			if (Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("class")) && Text::String::OrEmpty(attr->value)->Equals(UTF8STRC("post-title")))
			{
				UIntOS initLev = reader.GetPathLev();
				while (reader.ReadNext() && reader.GetPathLev() > initLev)
				{
					if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("h2")))
					{
						found = reader.ReadNodeText(name);
						break;
					}
				}
			}
		}
	}
	cli.Delete();
	return found;
}

void Net::WebSite::WebSite48IdolControl::Title2DisplayName(NN<Text::String> title, NN<Text::StringBuilderUTF8> dispName)
{
	UIntOS i;
	Text::StringBuilderUTF8 sb;
	sb.Append(title);
	sb.Trim();
	while (sb.EndsWith(')'))
	{
		i = sb.LastIndexOf('(');
		if (i == INVALID_INDEX)
			break;
		if (Text::StrStartsWithC(&sb.ToString()[i], sb.GetLength() - i, UTF8STRC("(GYAO")))
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
	sb.ReplaceStr(UTF8STRC("?"), sbuff, 3);
	sbuff[0] = 0xEF;
	sbuff[1] = 0xBC;
	sbuff[2] = 0x9A;
	sb.ReplaceStr(UTF8STRC(":"), sbuff, 3);
	while (sb.EndsWith(')'))
	{
		i = sb.LastIndexOf('(');
		if (i == INVALID_INDEX)
			break;
		if (Text::StrStartsWithC(&sb.ToString()[i], sb.GetLength(), UTF8STRC("(GYAO")))
		{
			break;
		}
		sb.TrimToLength((UIntOS)i);
		sb.Trim();
	}

	if (sb.ToString()[0] >= '0' && sb.ToString()[0] <= '9' && sb.ToString()[6] == ' ')
	{
		dispName->AppendC(&sb.ToString()[7], sb.GetLength() - 7);
		dispName->AppendC(UTF8STRC(" 20"));
		dispName->AppendC(sb.ToString(), 6);
	}
	else if (sb.StartsWith(UTF8STRC("[BD/DVD] ")))
	{
		dispName->AppendC(sb.ToString() + 9, sb.GetLength() - 9);
	}
	else
	{
		dispName->AppendC(sb.ToString(), sb.GetLength());
	}
}
