#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "Net/HTTPClient.h"
#include "Net/WebServiceClient.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"
#include "Text/XMLDOM.h"
#include "Text/UTF8Writer.h"
#include "Text/TextBinEnc/FormEncoding.h"

Net::WebServiceClient::WebServiceClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString serviceAddr, const UTF8Char *serviceName, const UTF8Char *targetNS)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->serviceAddr = Text::String::New(serviceAddr);
	this->serviceName = Text::StrCopyNew(serviceName);
	this->targetNS = Text::StrCopyNew(targetNS);
	this->soapAction = 0;
	NEW_CLASS(this->paramList, Data::ArrayList<ParamInfo*>());
	this->responseVal = 0;
}

Net::WebServiceClient::~WebServiceClient()
{
	this->serviceAddr->Release();
	Text::StrDelNew(this->serviceName);
	Text::StrDelNew(this->targetNS);
	OSInt i;
	ParamInfo *param;
	i = this->paramList->GetCount();
	while (i-- > 0)
	{
		param = this->paramList->GetItem(i);
		Text::StrDelNew(param->name);
		Text::StrDelNew(param->val);
		MemFree(param);
	}
	SDEL_TEXT(this->soapAction);
	DEL_CLASS(this->paramList);
	SDEL_TEXT(this->responseVal);
}

void Net::WebServiceClient::AddParam(const UTF8Char *paramName, const UTF8Char *paramVal)
{
	ParamInfo *param;
	param = MemAlloc(ParamInfo, 1);
	param->name = Text::StrCopyNew(paramName);
	param->val = Text::StrCopyNew(paramVal);
	this->paramList->Add(param);
}

void Net::WebServiceClient::SetSOAPAction(const UTF8Char *soapAction)
{
	SDEL_TEXT(this->soapAction);
	if (soapAction)
	{
		this->soapAction = Text::StrCopyNew(soapAction);
	}
}

Bool Net::WebServiceClient::Request(RequestType rt)
{
	Text::String *s;
	UOSInt i;
	UOSInt j;
	ParamInfo *param;
	UInt8 *buff;
	UTF8Char sbuff[512];
	UTF8Char *sptr;

	if (Text::StrCharCnt(this->serviceName) > 64)
		return false;
	if (rt == RT_SOAP11)
	{
		Bool succ;
		NotNullPtr<Net::HTTPClient> cli;
		IO::MemoryStream *mstm;
		Text::UTF8Writer *writer;
		NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("Net.WebServiceClient.Request.S11")));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));
		writer->WriteStrC(UTF8STRC("<?xml version=\"1.0\" encoding=\"utf-8\"?>"));
		writer->WriteStrC(UTF8STRC("<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"));
		writer->WriteStrC(UTF8STRC("<soap:Body>"));
		writer->WriteStrC(UTF8STRC("<"));
		s = Text::XML::ToNewXMLText(this->serviceName);
		writer->WriteStrC(s->v, s->leng);
		s->Release();
		writer->WriteStrC(UTF8STRC(" xmlns="));
		s = Text::XML::ToNewAttrText(this->targetNS);
		writer->WriteStrC(s->v, s->leng);
		s->Release();
		writer->WriteStrC(UTF8STRC(">"));

		i = 0;
		j = this->paramList->GetCount();
		while (i < j)
		{
			param = this->paramList->GetItem(i);
			writer->WriteStrC(UTF8STRC("<"));
			s = Text::XML::ToNewXMLText(param->name);
			writer->WriteStrC(s->v, s->leng);
			s->Release();
			writer->WriteStrC(UTF8STRC(">"));

			s = Text::XML::ToNewXMLTextLite(param->val);
			writer->WriteStrC(s->v, s->leng);
			s->Release();
			
			writer->WriteStrC(UTF8STRC("</"));
			s = Text::XML::ToNewXMLText(param->name);
			writer->WriteStrC(s->v, s->leng);
			s->Release();
			writer->WriteStrC(UTF8STRC(">"));
			i++;
		}
		writer->WriteStrC(UTF8STRC("</"));
		s = Text::XML::ToNewXMLText(this->serviceName);
		writer->WriteStrC(s->v, s->leng);
		s->Release();
		writer->WriteStrC(UTF8STRC(">"));
		writer->WriteStrC(UTF8STRC("</soap:Body>"));
		writer->WriteStrC(UTF8STRC("</soap:Envelope>"));
		DEL_CLASS(writer);

		cli = Net::HTTPClient::CreateConnect(sockf, this->ssl, this->serviceAddr->ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
		if (this->soapAction == 0)
		{
			cli->AddHeaderC(CSTR("SOAPAction"), CSTR("\"\""));
		}
		else
		{
			sbuff[0] = '\"';
			sptr = Text::TextBinEnc::FormEncoding::FormEncode(&sbuff[1], this->soapAction);
			*sptr++ = '\"';
			*sptr = 0;
			cli->AddHeaderC(CSTR("SOAPAction"), CSTRP(sbuff, sptr));
		}
		cli->AddHeaderC(CSTR("Content-Type"), CSTR("text/xml; charset=utf-8"));
		buff = mstm->GetBuff(&i);
		sptr = Text::StrUOSInt(sbuff, i);
		cli->AddHeaderC(CSTR("Content-Length"), CSTRP(sbuff, sptr));

		cli->Write(buff, i);

		succ = false;
		Int32 status = cli->GetRespStatus();
		if (status == 200)
		{
			UInt64 contLeng = cli->GetContentLength();
			if (contLeng > 0 && contLeng < 1048576)
			{
				buff = MemAlloc(UInt8, contLeng);
				i = 0;
				while (i < contLeng)
				{
					j = cli->Read(&buff[i], (OSInt)contLeng - i);
					if (j == 0)
						break;
					i += j;
				}
				if (i == contLeng)
				{
					Text::EncodingFactory encFact;
					Text::XMLDocument *doc;
					Text::XMLNode *node1;
					Text::XMLNode *node2;
					Text::StringBuilderUTF8 sb;
					NEW_CLASS(doc, Text::XMLDocument());
					if (doc->ParseBuff(&encFact, buff, contLeng))
					{
						i = doc->GetChildCnt();
						while (i-- > 0)
						{
							node1 = doc->GetChild(i);
							if (node1->GetNodeType() == Text::XMLNode::NodeType::Element && node1->name->Equals(UTF8STRC("soap:Envelope")))
							{
								i = node1->GetChildCnt();
								while (i-- > 0)
								{
									node2 = node1->GetChild(i);
									if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->Equals(UTF8STRC("soap:Body")))
									{
										node1 = node2;
										sptr = Text::StrConcatC(Text::StrConcat(sbuff, this->serviceName), UTF8STRC("Response"));
										i = node1->GetChildCnt();
										while (i-- > 0)
										{
											node2 = node1->GetChild(i);
											if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->Equals(sbuff, (UOSInt)(sptr - sbuff)))
											{
												node1 = node2;
												sptr = Text::StrConcatC(Text::StrConcat(sbuff, this->serviceName), UTF8STRC("Result"));
												i = node1->GetChildCnt();
												while (i-- > 0)
												{
													node2 = node1->GetChild(i);
													if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->Equals(sbuff, (UOSInt)(sptr - sbuff)))
													{
														sb.ClearStr();
														node2->GetInnerText(&sb);
														this->responseVal = Text::StrCopyNew(sb.ToString());
														succ = true;
														break;
													}
												}
												break;
											}
										}

										break;
									}
								}
								break;
							}
						}
					}
					DEL_CLASS(doc);
				}
				MemFree(buff);
			}
		}
		DEL_CLASS(mstm);
		DEL_CLASS(cli);
		return succ;
	}
	else if (rt == RT_SOAP12)
	{
		Bool succ;
		NotNullPtr<Net::HTTPClient> cli;
		IO::MemoryStream *mstm;
		Text::UTF8Writer *writer;
		NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("Net.WebServiceClient.Request.S11")));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));
		writer->WriteStrC(UTF8STRC("<?xml version=\"1.0\" encoding=\"utf-8\"?>"));
		writer->WriteStrC(UTF8STRC("<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">"));
		writer->WriteStrC(UTF8STRC("<soap12:Body>"));
		writer->WriteStrC(UTF8STRC("<"));
		s = Text::XML::ToNewXMLText(this->serviceName);
		writer->WriteStrC(s->v, s->leng);
		s->Release();
		writer->WriteStrC(UTF8STRC(" xmlns="));
		s = Text::XML::ToNewAttrText(this->targetNS);
		writer->WriteStrC(s->v, s->leng);
		s->Release();
		writer->WriteStrC(UTF8STRC(">"));

		i = 0;
		j = this->paramList->GetCount();
		while (i < j)
		{
			param = this->paramList->GetItem(i);
			writer->WriteStrC(UTF8STRC("<"));
			s = Text::XML::ToNewXMLText(param->name);
			writer->WriteStrC(s->v, s->leng);
			s->Release();
			writer->WriteStrC(UTF8STRC(">"));

			s = Text::XML::ToNewXMLTextLite(param->val);
			writer->WriteStrC(s->v, s->leng);
			s->Release();
			
			writer->WriteStrC(UTF8STRC("</"));
			s = Text::XML::ToNewXMLText(param->name);
			writer->WriteStrC(s->v, s->leng);
			s->Release();
			writer->WriteStrC(UTF8STRC(">"));
			i++;
		}
		writer->WriteStrC(UTF8STRC("</"));
		s = Text::XML::ToNewXMLText(this->serviceName);
		writer->WriteStrC(s->v, s->leng);
		s->Release();
		writer->WriteStrC(UTF8STRC(">"));
		writer->WriteStrC(UTF8STRC("</soap12:Body>"));
		writer->WriteStrC(UTF8STRC("</soap12:Envelope>"));
		DEL_CLASS(writer);

		cli = Net::HTTPClient::CreateConnect(sockf, this->ssl, this->serviceAddr->ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
		cli->AddHeaderC(CSTR("Content-Type"), CSTR("application/soap+xml; charset=utf-8"));
		buff = mstm->GetBuff(&i);
		cli->AddContentLength(i);

		cli->Write(buff, i);

		succ = false;
		Int32 status = cli->GetRespStatus();
		if (status == 200)
		{
			UInt64 contLeng = cli->GetContentLength();
			if (contLeng > 0 && contLeng < 1048576)
			{
				buff = MemAlloc(UInt8, contLeng);
				i = 0;
				while (i < contLeng)
				{
					j = cli->Read(&buff[i], (OSInt)contLeng - i);
					if (j == 0)
						break;
					i += j;
				}
				if (i == contLeng)
				{
					Text::EncodingFactory encFact;
					Text::XMLDocument *doc;
					Text::XMLNode *node1;
					Text::XMLNode *node2;
					Text::StringBuilderUTF8 sb;
					NEW_CLASS(doc, Text::XMLDocument());
					if (doc->ParseBuff(&encFact, buff, contLeng))
					{
						i = doc->GetChildCnt();
						while (i-- > 0)
						{
							node1 = doc->GetChild(i);
							if (node1->GetNodeType() == Text::XMLNode::NodeType::Element && node1->name->EndsWithICase(UTF8STRC(":Envelope")))
							{
								i = node1->GetChildCnt();
								while (i-- > 0)
								{
									node2 = node1->GetChild(i);
									if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->EndsWithICase(UTF8STRC(":Body")))
									{
										node1 = node2;
										sptr = Text::StrConcatC(Text::StrConcat(sbuff, this->serviceName), UTF8STRC("Response"));
										i = node1->GetChildCnt();
										while (i-- > 0)
										{
											node2 = node1->GetChild(i);
											if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->Equals(sbuff, (UOSInt)(sptr - sbuff)))
											{
												node1 = node2;
												sptr = Text::StrConcatC(Text::StrConcat(sbuff, this->serviceName), UTF8STRC("Result"));
												i = node1->GetChildCnt();
												while (i-- > 0)
												{
													node2 = node1->GetChild(i);
													if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->Equals(sbuff, (UOSInt)(sptr - sbuff)))
													{
														sb.ClearStr();
														node2->GetInnerText(&sb);
														this->responseVal = Text::StrCopyNew(sb.ToString());
														succ = true;
														break;
													}
												}
												break;
											}
										}

										break;
									}
								}
								break;
							}
						}
					}
					DEL_CLASS(doc);
				}
				MemFree(buff);
			}
		}
		DEL_CLASS(mstm);
		DEL_CLASS(cli);
		return succ;
	}
	/////////////////////////////////////////
	return false;
}

const UTF8Char *Net::WebServiceClient::GetResponseVal()
{
	return this->responseVal;
}
