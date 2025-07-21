#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Net/HTTPUtil.h"

Optional<IO::Stream> Net::HTTPUtil::DownloadAndOpen(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, Text::CStringNN fileName, UInt64 maxSize)
{
	NN<IO::FileStream> fs;
	NEW_CLASSNN(fs, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		fs.Delete();
		return 0;
	}
	if (Net::HTTPClient::LoadContent(clif, ssl, url, NN<IO::Stream>(fs), maxSize))
	{
		fs->SeekFromBeginning(0);
		return fs;
	}
	fs.Delete();
	return 0;
}

Bool Net::HTTPUtil::DownloadFile(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, Text::CStringNN fileName, UInt64 maxSize)
{
	NN<IO::Stream> stm;
	if (DownloadAndOpen(clif, ssl, url, fileName, maxSize).SetTo(stm))
	{
		stm.Delete();
		return true;
	}
	return false;
}
