#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Net/MIME.h"
#include "Net/WebServer/SiteRootHandler.h"

Net::WebServer::SiteRootHandler::~SiteRootHandler()
{
	UnsafeArray<UInt8> faviconBuff;
	if (this->faviconBuff.SetTo(faviconBuff))
	{
		MemFreeArr(faviconBuff);
		this->faviconBuff = nullptr;
	}
}

Bool Net::WebServer::SiteRootHandler::DoRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	UnsafeArray<UInt8> faviconBuff;
	if (subReq.Equals(UTF8STRC("/favicon.ico")))
	{
		if (this->faviconBuff.SetTo(faviconBuff))
		{
			this->AddResponseHeaders(req, resp);
			resp->AddContentLength(this->faviconSize);
			Text::CStringNN mime = Net::MIME::GetMIMEFromExt(CSTR("ico"));
			resp->AddContentType(mime);
			resp->Write(Data::ByteArrayR(faviconBuff, this->faviconSize));
			return true;
		}
		else
		{
			resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
			return true;
		}
	}

	return this->WebStandardHandler::DoRequest(req, resp, subReq);
}

Bool Net::WebServer::SiteRootHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	return this->SiteRootHandler::DoRequest(req, resp, subReq);
}

Net::WebServer::SiteRootHandler::SiteRootHandler(Text::CStringNN faviconPath)
{
	UnsafeArray<UInt8> faviconBuff;
	this->faviconBuff = nullptr;
	this->faviconSize = 0;
	IO::FileStream fs(faviconPath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		UInt64 leng = fs.GetLength();
		if (leng > 0 && leng < 65536)
		{
			this->faviconSize = (UOSInt)leng;
			this->faviconBuff = faviconBuff = MemAllocArr(UInt8, this->faviconSize);
			if (fs.Read(Data::ByteArray(faviconBuff, this->faviconSize)) != this->faviconSize)
			{
				MemFreeArr(faviconBuff);
				this->faviconSize = 0;
			}
		}
	}
}
