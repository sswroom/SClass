#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Sync/Thread.h"
#include "Net/FTPClient.h"

Net::FTPClient::FTPClient(const UTF8Char *url, Net::SocketFactory *sockf, Bool passiveMode, Int32 codePage) : IO::Stream(url)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *userName = 0;
	UTF8Char *password = 0;
	UTF8Char *host = 0;
	UTF8Char *port = 0;
	UTF8Char c;
	Text::StrConcat(sbuff, url);
	this->userName = 0;
	this->password = 0;
	this->host = 0;
	this->path = 0;
	this->conn = 0;
	this->cli2 = 0;
	this->codePage = codePage;

	sptr = sbuff;
	if (!Text::StrStartsWithICase(sbuff, (const UTF8Char*)"FTP://"))
		return;
	sptr = &sbuff[6];
	host = sptr;
	while ((c = *sptr++) != 0)
	{
		if (c == '/')
		{
			this->path = Text::StrCopyNew(&sptr[-1]);
			sptr[-1] = 0;
			if (userName == 0)
			{
				this->userName = Text::StrCopyNew((const UTF8Char*)"Annonymous");
				this->password = 0;
			}
			else
			{
				this->userName = Text::StrCopyNew(userName);
				if (password)
				{
					this->password = Text::StrCopyNew(password);
				}
			}
			this->host = Text::StrCopyNew(host);
			if (port)
			{
				if (!Text::StrToUInt16(port, &this->port))
				{
					this->port = 21;
				}
			}
			else
			{
				this->port = 21;
			}
			NEW_CLASS(this->conn, Net::FTPConn(this->host, this->port, sockf, this->codePage));
			this->conn->SendUser(this->userName);
			if (this->password)
			{
				this->conn->SendPassword(this->password);
			}
			if (this->conn->IsLogged())
			{
				Text::StrConcat(sbuff, this->path);
				OSInt i = Text::StrLastIndexOf(sbuff, '/');
				Bool dirOk;
				if (i == 0)
				{
					dirOk = this->conn->ChangeDirectory((const UTF8Char*)"/");
				}
				else
				{
					sbuff[i] = 0;
					dirOk = this->conn->ChangeDirectory(sbuff);
				}

				if (dirOk)
				{
//					Int64 fileSize;
//					Data::DateTime dt;

					Int32 ip;
					UInt16 port;
//					this->conn->GetFileSize(&sbuff[i + 1], &fileSize);
//					this->conn->GetFileModTime(&sbuff[i + 1], &dt);

					this->conn->ToBinaryType();
					if (this->conn->ChangePassiveMode(&ip, &port))
					{
						NEW_CLASS(this->cli2, Net::TCPClient(sockf, ip, port));
						this->conn->GetFile(&sbuff[i + 1]);
					}
				}
			}
		}
		else if (c == ':')
		{
			if (port)
			{
				return;
			}
			sptr[-1] = 0;
			port = sptr;
		}
		else if (c == '@')
		{
			if (userName)
			{
				return;
			}
			sptr[-1] = 0;
			userName = host;
			password = port;
			host = sptr;
			port = 0;
		}
	}
}

Net::FTPClient::~FTPClient()
{
	if (this->conn)
	{
		DEL_CLASS(this->conn);
	}
	if (this->userName)
	{
		Text::StrDelNew(this->userName);
	}
	if (this->password)
	{
		Text::StrDelNew(this->password);
	}
	if (this->host)
	{
		Text::StrDelNew(this->host);
	}
	if (this->path)
	{
		Text::StrDelNew(this->path);
	}
}

UOSInt Net::FTPClient::Read(UInt8 *buff, UOSInt size)
{
	if (this->cli2)
	{
		return this->cli2->Read(buff, size);
	}
	else
	{
		return 0;
	}
}

UOSInt Net::FTPClient::Write(const UInt8 *buff, UOSInt size)
{
	return 0;
}

Int32 Net::FTPClient::Flush()
{
	return 0;
}

void Net::FTPClient::Close()
{
	if (this->cli2)
	{
		DEL_CLASS(this->cli2);
		this->cli2 = 0;
	}
}

Bool Net::FTPClient::Recover()
{
	/////////////////////////////////////
	return false;
}
