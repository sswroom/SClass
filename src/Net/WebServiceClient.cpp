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

Net::WebServiceClient::WebServiceClient(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN serviceAddr, Text::CStringNN serviceName, Text::CStringNN targetNS)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->serviceAddr = Text::String::New(serviceAddr);
	this->serviceName = Text::String::New(serviceName);
	this->targetNS = Text::String::New(targetNS);
	this->soapAction = 0;
	this->responseVal = 0;
}

Net::WebServiceClient::~WebServiceClient()
{
	this->serviceAddr->Release();
	this->serviceName->Release();
	this->targetNS->Release();
	UIntOS i;
	NN<ParamInfo> param;
	i = this->paramList.GetCount();
	while (i-- > 0)
	{
		param = this->paramList.GetItemNoCheck(i);
		param->name->Release();
		param->val->Release();
		MemFreeNN(param);
	}
	OPTSTR_DEL(this->soapAction);
	OPTSTR_DEL(this->responseVal);
}

void Net::WebServiceClient::AddParam(Text::CStringNN paramName, Text::CStringNN paramVal)
{
	NN<ParamInfo> param;
	param = MemAllocNN(ParamInfo);
	param->name = Text::String::New(paramName);
	param->val = Text::String::New(paramVal);
	this->paramList.Add(param);
}

void Net::WebServiceClient::SetSOAPAction(Text::CString soapAction)
{
	OPTSTR_DEL(this->soapAction);
	if (soapAction.v)
	{
		this->soapAction = Text::String::New(soapAction);
	}
}

Bool Net::WebServiceClient::Request(RequestType rt)
{
	NN<Text::String> s;
	UIntOS i;
	UIntOS j;
	NN<ParamInfo> param;
	UInt8 *buff;
	UTF8Char sbuff[512];
	UTF8Char *sptr;

	if (this->serviceName->leng > 64)
		return false;
	if (rt == RT_SOAP11)
	{
		Bool succ;
		NN<Net::HTTPClient> cli;
		NN<IO::MemoryStream> mstm;
		Text::UTF8Writer *writer;
		NEW_CLASSNN(mstm, IO::MemoryStream());
		NEW_CLASS(writer, Text::UTF8Writer(mstm));
		writer->Write(CSTR("<?xml version=\"1.0\" encoding=\"utf-8\"?>"));
		writer->Write(CSTR("<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"));
		writer->Write(CSTR("<soap:Body>"));
		writer->Write(CSTR("<"));
		s = Text::XML::ToNewXMLText(this->serviceName->v);
		writer->Write(s->ToCString());
		s->Release();
		writer->Write(CSTR(" xmlns="));
		s = Text::XML::ToNewAttrText(this->targetNS->v);
		writer->Write(s->ToCString());
		s->Release();
		writer->Write(CSTR(">"));

		i = 0;
		j = this->paramList.GetCount();
		while (i < j)
		{
			param = this->paramList.GetItemNoCheck(i);
			writer->Write(CSTR("<"));
			s = Text::XML::ToNewXMLText(param->name->v);
			writer->Write(s->ToCString());
			s->Release();
			writer->Write(CSTR(">"));

			s = Text::XML::ToNewXMLTextLite(param->val->v);
			writer->Write(s->ToCString());
			s->Release();
			
			writer->Write(CSTR("</"));
			s = Text::XML::ToNewXMLText(param->name->v);
			writer->Write(s->ToCString());
			s->Release();
			writer->Write(CSTR(">"));
			i++;
		}
		writer->Write(CSTR("</"));
		s = Text::XML::ToNewXMLText(this->serviceName->v);
		writer->Write(s->ToCString());
		s->Release();
		writer->Write(CSTR(">"));
		writer->Write(CSTR("</soap:Body>"));
		writer->Write(CSTR("</soap:Envelope>"));
		DEL_CLASS(writer);

		cli = Net::HTTPClient::CreateConnect(sockf, this->ssl, this->serviceAddr->ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
		if (!this->soapAction.SetTo(s))
		{
			cli->AddHeaderC(CSTR("SOAPAction"), CSTR("\"\""));
		}
		else
		{
			sbuff[0] = '\"';
			sptr = Text::TextBinEnc::FormEncoding::FormEncode(&sbuff[1], s->v);
			*sptr++ = '\"';
			*sptr = 0;
			cli->AddHeaderC(CSTR("SOAPAction"), CSTRP(sbuff, sptr));
		}
		cli->AddHeaderC(CSTR("Content-Type"), CSTR("text/xml; charset=utf-8"));
		buff = mstm->GetBuff(i);
		sptr = Text::StrUIntOS(sbuff, i);
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
					j = cli->Read(Data::ByteArray(&buff[i], (UIntOS)contLeng - i));
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
					if (doc->ParseBuff(encFact, buff, contLeng))
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
										sptr = Text::StrConcatC(this->serviceName->ConcatTo(sbuff), UTF8STRC("Response"));
										i = node1->GetChildCnt();
										while (i-- > 0)
										{
											node2 = node1->GetChild(i);
											if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->Equals(sbuff, (UIntOS)(sptr - sbuff)))
											{
												node1 = node2;
												sptr = Text::StrConcatC(this->serviceName->ConcatTo(sbuff), UTF8STRC("Result"));
												i = node1->GetChildCnt();
												while (i-- > 0)
												{
													node2 = node1->GetChild(i);
													if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->Equals(sbuff, (UIntOS)(sptr - sbuff)))
													{
														sb.ClearStr();
														node2->GetInnerText(sb);
														this->responseVal = Text::String::New(sb.ToCString());
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
		mstm.Delete();
		cli.Delete();
		return succ;
	}
	else if (rt == RT_SOAP12)
	{
		Bool succ;
		NN<Net::HTTPClient> cli;
		NN<IO::MemoryStream> mstm;
		Text::UTF8Writer *writer;
		NEW_CLASSNN(mstm, IO::MemoryStream());
		NEW_CLASS(writer, Text::UTF8Writer(mstm));
		writer->Write(CSTR("<?xml version=\"1.0\" encoding=\"utf-8\"?>"));
		writer->Write(CSTR("<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">"));
		writer->Write(CSTR("<soap12:Body>"));
		writer->Write(CSTR("<"));
		s = Text::XML::ToNewXMLText(this->serviceName->v);
		writer->Write(s->ToCString());
		s->Release();
		writer->Write(CSTR(" xmlns="));
		s = Text::XML::ToNewAttrText(this->targetNS->v);
		writer->Write(s->ToCString());
		s->Release();
		writer->Write(CSTR(">"));

		i = 0;
		j = this->paramList.GetCount();
		while (i < j)
		{
			param = this->paramList.GetItemNoCheck(i);
			writer->Write(CSTR("<"));
			s = Text::XML::ToNewXMLText(param->name->v);
			writer->Write(s->ToCString());
			s->Release();
			writer->Write(CSTR(">"));

			s = Text::XML::ToNewXMLTextLite(param->val->v);
			writer->Write(s->ToCString());
			s->Release();
			
			writer->Write(CSTR("</"));
			s = Text::XML::ToNewXMLText(param->name->v);
			writer->Write(s->ToCString());
			s->Release();
			writer->Write(CSTR(">"));
			i++;
		}
		writer->Write(CSTR("</"));
		s = Text::XML::ToNewXMLText(this->serviceName->v);
		writer->Write(s->ToCString());
		s->Release();
		writer->Write(CSTR(">"));
		writer->Write(CSTR("</soap12:Body>"));
		writer->Write(CSTR("</soap12:Envelope>"));
		DEL_CLASS(writer);

		cli = Net::HTTPClient::CreateConnect(sockf, this->ssl, this->serviceAddr->ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
		cli->AddHeaderC(CSTR("Content-Type"), CSTR("application/soap+xml; charset=utf-8"));
		buff = mstm->GetBuff(i);
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
					j = cli->Read(Data::ByteArray(&buff[i], (UIntOS)contLeng - i));
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
					if (doc->ParseBuff(encFact, buff, contLeng))
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
										sptr = Text::StrConcatC(this->serviceName->ConcatTo(sbuff), UTF8STRC("Response"));
										i = node1->GetChildCnt();
										while (i-- > 0)
										{
											node2 = node1->GetChild(i);
											if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->Equals(sbuff, (UIntOS)(sptr - sbuff)))
											{
												node1 = node2;
												sptr = Text::StrConcatC(this->serviceName->ConcatTo(sbuff), UTF8STRC("Result"));
												i = node1->GetChildCnt();
												while (i-- > 0)
												{
													node2 = node1->GetChild(i);
													if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->Equals(sbuff, (UIntOS)(sptr - sbuff)))
													{
														sb.ClearStr();
														node2->GetInnerText(sb);
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
		mstm.Delete();
		cli.Delete();
		return succ;
	}
	/////////////////////////////////////////
	return false;
}

const UTF8Char *Net::WebServiceClient::GetResponseVal()
{
	return this->responseVal;
}
