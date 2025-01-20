#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Net/MIME.h"
#include "Net/WebServer/SiteRootHandler.h"

Net::WebServer::SiteRootHandler::~SiteRootHandler()
{
	if (this->faviconBuff)
	{
		MemFree(this->faviconBuff);
		this->faviconBuff = 0;
	}
}

Bool Net::WebServer::SiteRootHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	if (subReq.Equals(UTF8STRC("/favicon.ico")))
	{
		if (this->faviconBuff)
		{
			this->AddResponseHeaders(req, resp);
			resp->AddContentLength(this->faviconSize);
			Text::CStringNN mime = Net::MIME::GetMIMEFromExt(CSTR("ico"));
			resp->AddContentType(mime);
			resp->Write(Data::ByteArrayR(this->faviconBuff, this->faviconSize));
			return true;
		}
		else
		{
			resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
			return true;
		}
	}

	return this->DoRequest(req, resp, subReq);
}

Net::WebServer::SiteRootHandler::SiteRootHandler(Text::CStringNN faviconPath)
{
	this->faviconBuff = 0;
	this->faviconSize = 0;
	IO::FileStream fs(faviconPath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		UInt64 leng = fs.GetLength();
		if (leng > 0 && leng < 65536)
		{
			this->faviconSize = (UOSInt)leng;
			this->faviconBuff = MemAlloc(UInt8, this->faviconSize);
			if (fs.Read(Data::ByteArray(this->faviconBuff, this->faviconSize)) != this->faviconSize)
			{
				MemFree(this->faviconBuff);
				this->faviconSize = 0;
			}
		}
	}
}
