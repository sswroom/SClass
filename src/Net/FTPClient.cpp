#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/FTPClient.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

Net::FTPClient::FTPClient(Text::CStringNN url, NN<Net::TCPClientFactory> clif, Bool passiveMode, UInt32 codePage, Data::Duration timeout) : IO::Stream(url)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArrayOpt<UTF8Char> userName = nullptr;
	UnsafeArrayOpt<UTF8Char> password = nullptr;
	UnsafeArray<UTF8Char> host;
	UnsafeArrayOpt<UTF8Char> port = nullptr;
	UnsafeArray<UTF8Char> nns;
	UTF8Char c;
	sptr = url.ConcatTo(sbuff);
	this->userName = nullptr;
	this->password = nullptr;
	this->host = nullptr;
	this->path = nullptr;
	this->conn = nullptr;
	this->cli2 = nullptr;
	this->codePage = codePage;

	if (!Text::StrStartsWithICaseC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("FTP://")))
		return;
	sptr = &sbuff[6];
	host = sptr;
	while ((c = *sptr++) != 0)
	{
		if (c == '/')
		{
			this->path = Text::StrCopyNew(&sptr[-1]).Ptr();
			UnsafeArray<const UTF8Char> nnuserName;
			sptr[-1] = 0;
			if (!userName.SetTo(nns))
			{
				this->userName = nnuserName = Text::StrCopyNewC(UTF8STRC("Annonymous"));
				this->password = nullptr;
			}
			else
			{
				this->userName = nnuserName = Text::StrCopyNew(nns);
				if (password.SetTo(nns))
				{
					this->password = Text::StrCopyNew(nns);
				}
			}
			NN<Text::String> nnhost;
			this->host = nnhost = Text::String::New(host, (UIntOS)(sptr - host - 1));
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
			NN<Net::FTPConn> conn;
			NEW_CLASSNN(conn, Net::FTPConn(nnhost->ToCString(), this->port, clif, this->codePage, timeout));
			this->conn = conn;
			conn->SendUser(nnuserName);
			
			if (this->password.SetTo(nnuserName))
			{
				conn->SendPassword(nnuserName);
			}
			UnsafeArray<const UTF8Char> nnpath;
			if (conn->IsLogged() && this->path.SetTo(nnpath))
			{
				sptr = Text::StrConcat(sbuff, nnpath);
				UIntOS i = Text::StrLastIndexOfCharC(sbuff, (UIntOS)(sptr - sbuff), '/');
				Bool dirOk;
				if (i == INVALID_INDEX || i == 0)
				{
					dirOk = conn->ChangeDirectory((const UTF8Char*)"/");
				}
				else
				{
					sbuff[i] = 0;
					dirOk = conn->ChangeDirectory(sbuff);
				}

				if (dirOk)
				{
//					Int64 fileSize;
//					Data::DateTime dt;

					UInt32 ip;
					UInt16 port;
//					this->conn->GetFileSize(&sbuff[i + 1], &fileSize);
//					this->conn->GetFileModTime(&sbuff[i + 1], &dt);

					conn->ToBinaryType();
					if (conn->ChangePassiveMode(ip, port))
					{
						this->cli2 = clif->Create(ip, port, timeout);
						conn->GetFile(&sbuff[i + 1]);
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
			port = nullptr;
		}
	}
}

Net::FTPClient::~FTPClient()
{
	UnsafeArray<const UTF8Char> nns;
	this->conn.Delete();
	if (this->userName.SetTo(nns))
	{
		Text::StrDelNew(nns);
	}
	if (this->password.SetTo(nns))
	{
		Text::StrDelNew(nns);
	}
	OPTSTR_DEL(this->host);
	if (this->path.SetTo(nns))
	{
		Text::StrDelNew(nns);
	}
}

Bool Net::FTPClient::IsDown() const
{
	return this->cli2.IsNull();
}

UIntOS Net::FTPClient::Read(const Data::ByteArray &buff)
{
	NN<Net::TCPClient> cli2;
	if (this->cli2.SetTo(cli2))
	{
		return cli2->Read(buff);
	}
	else
	{
		return 0;
	}
}

UIntOS Net::FTPClient::Write(Data::ByteArrayR buff)
{
	return 0;
}

Int32 Net::FTPClient::Flush()
{
	return 0;
}

void Net::FTPClient::Close()
{
	this->cli2.Delete();
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
