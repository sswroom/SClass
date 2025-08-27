#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/UUID.h"
#include "Net/WSDiscoveryClient.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"

void __stdcall Net::WSDiscoveryClient::PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(data.Arr(), data.GetSize());
	printf("Recv:\r\n%s\r\n", sb.v.Ptr());
}

Net::WSDiscoveryClient::WSDiscoveryClient(NN<Net::SocketFactory> sockf)
{
	this->sockf = sockf;
	NEW_CLASSNN(this->svr, Net::UDPServer(sockf, 0, 0, nullptr, PacketHdlr, this, log, nullptr, 1, false));
}

Net::WSDiscoveryClient::~WSDiscoveryClient()
{
	this->svr.Delete();
}

Bool Net::WSDiscoveryClient::IsError() const
{
	return this->svr->IsError();
}

Bool Net::WSDiscoveryClient::Probe(Text::CStringNN type, Text::CStringNN ns, Text::CStringNN nsName) const
{
	NN<Text::String> s;
	Data::UUID uuid;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("<?xml version=\"1.0\" encoding=\"utf-8\"?>"));
	sb.Append(CSTR("<Envelope xmlns=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:a=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\" xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\""));
	sb.Append(CSTR(" xmlns:"));
	sb.Append(nsName);
	sb.Append(CSTR("="));
	s = Text::XML::ToNewAttrText(ns.v);
	sb.Append(s);
	s->Release();
	sb.AppendUTF8Char('>');
	sb.Append(CSTR("<Header>"));
	sb.Append(CSTR("<a:Action mustUnderstand=\"1\">http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</a:Action>"));
	sb.Append(CSTR("<a:To>urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To>"));
	sb.Append(CSTR("<a:ReplyTo><a:Address>http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous</a:Address></a:ReplyTo>"));
	uuid.GenerateV4();
  	sb.Append(CSTR("<a:MessageID>urn:uuid:"));
	uuid.ToString(sb);
	sb.Append(CSTR("</a:MessageID>"));
	sb.Append(CSTR("</Header>"));
	sb.Append(CSTR("<Body>"));
	sb.Append(CSTR("<d:Probe><d:Types>"));
	sb.Append(nsName);
	sb.AppendUTF8Char(':');
	sb.Append(type);
	sb.Append(CSTR("</d:Types></d:Probe>"));
	sb.Append(CSTR("</Body>"));
	sb.Append(CSTR("</Envelope>"));
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfo(addr, CSTR("239.255.255.250"));
	return this->svr->SendTo(addr, 3702, sb.v, sb.leng);
}
