#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Sync/ThreadUtil.h"
#include "Net/FTPClient.h"

Net::FTPClient::FTPClient(Text::CStringNN url, NN<Net::SocketFactory> sockf, Bool passiveMode, UInt32 codePage, Data::Duration timeout) : IO::Stream(url)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArrayOpt<UTF8Char> userName = 0;
	UnsafeArrayOpt<UTF8Char> password = 0;
	UnsafeArray<UTF8Char> host;
	UnsafeArrayOpt<UTF8Char> port = 0;
	UnsafeArray<UTF8Char> nns;
	UTF8Char c;
	sptr = url.ConcatTo(sbuff);
	this->userName = 0;
	this->password = 0;
	this->host = 0;
	this->path = 0;
	this->conn = 0;
	this->cli2 = 0;
	this->codePage = codePage;

	if (!Text::StrStartsWithICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("FTP://")))
		return;
	sptr = &sbuff[6];
	host = sptr;
	while ((c = *sptr++) != 0)
	{
		if (c == '/')
		{
			this->path = Text::StrCopyNew(&sptr[-1]).Ptr();
			sptr[-1] = 0;
			if (!userName.SetTo(nns))
			{
				this->userName = Text::StrCopyNewC(UTF8STRC("Annonymous")).Ptr();
				this->password = 0;
			}
			else
			{
				this->userName = Text::StrCopyNew(nns).Ptr();
				if (password.SetTo(nns))
				{
					this->password = Text::StrCopyNew(nns).Ptr();
				}
			}
			this->host = Text::String::New(host, (UOSInt)(sptr - host - 1)).Ptr();
			if (port.SetTo(nns))
			{
				if (!Text::StrToUInt16(nns, this->port))
				{
					this->port = 21;
				}
			}
			else
			{
				this->port = 21;
			}
			NEW_CLASS(this->conn, Net::FTPConn(this->host->ToCString(), this->port, sockf, this->codePage, timeout));
			this->conn->SendUser(this->userName);
			if (this->password)
			{
				this->conn->SendPassword(this->password);
			}
			if (this->conn->IsLogged())
			{
				sptr = Text::StrConcat(sbuff, this->path);
				UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '/');
				Bool dirOk;
				if (i == INVALID_INDEX || i == 0)
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

					UInt32 ip;
					UInt16 port;
//					this->conn->GetFileSize(&sbuff[i + 1], &fileSize);
//					this->conn->GetFileModTime(&sbuff[i + 1], &dt);

					this->conn->ToBinaryType();
					if (this->conn->ChangePassiveMode(&ip, &port))
					{
						NEW_CLASS(this->cli2, Net::TCPClient(sockf, ip, port, timeout));
						this->conn->GetFile(&sbuff[i + 1]);
					}
				}
			}
		}
		else if (c == ':')
		{
			if (port.NotNull())
			{
				return;
			}
			sptr[-1] = 0;
			port = sptr;
		}
		else if (c == '@')
		{
			if (userName.NotNull())
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
	SDEL_STRING(this->host);
	if (this->path)
	{
		Text::StrDelNew(this->path);
	}
}

Bool Net::FTPClient::IsDown() const
{
	return this->cli2 == 0;
}

UOSInt Net::FTPClient::Read(const Data::ByteArray &buff)
{
	if (this->cli2)
	{
		return this->cli2->Read(buff);
	}
	else
	{
		return 0;
	}
}

UOSInt Net::FTPClient::Write(Data::ByteArrayR buff)
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

IO::StreamType Net::FTPClient::GetStreamType() const
{
	return IO::StreamType::FTPClient;
}
