#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/APE/APEIO.h"
#include "Text/MyString.h"

Media::APE::APEIO::APEIO(NN<IO::StreamData> data)
{
	this->data = data->GetPartialData(0, data->GetDataSize());
	this->currPos = 0;
}

Media::APE::APEIO::~APEIO()
{
	DEL_CLASS(this->data);
}

int Media::APE::APEIO::Open(const WChar * pName, BOOL bOpenReadOnly)
{
	return 0;
}

int Media::APE::APEIO::Close()
{
	return 0;
}

int Media::APE::APEIO::Read(void * pBuffer, UInt32 nBytesToRead, UInt32 * pBytesRead)
{
	Int32 byteRead;
	byteRead = (Int32)this->data->GetRealData(currPos, nBytesToRead, (UInt8*)pBuffer);
	if (pBytesRead)
	{
		*pBytesRead = byteRead;
	}
	if (byteRead > 0)
	{
		currPos += byteRead;
		return 0;
	}
	return 1;
}

int Media::APE::APEIO::Write(const void * pBuffer, UInt32 nBytesToWrite, UInt32 * pBytesWritten)
{
	return 0;
}

int Media::APE::APEIO::Seek(Int32 nDistance, UInt32 nMoveMode)
{
	if (nMoveMode == FILE_BEGIN)
	{
		this->currPos = nDistance;
	}
	else if (nMoveMode == FILE_CURRENT)
	{
		this->currPos += nDistance;
	}
	else if (nMoveMode == FILE_END)
	{
		this->currPos = this->data->GetDataSize() + nDistance;
	}
	return 0;
}

int Media::APE::APEIO::Create(const WChar * pName)
{
	return 0;
}

int Media::APE::APEIO::Delete()
{
	return 0;
}

int Media::APE::APEIO::SetEOF()
{
	return 0;
}

int Media::APE::APEIO::GetPosition()
{
	return (Int32)this->currPos;
}

int Media::APE::APEIO::GetSize()
{
	return (Int32)this->data->GetDataSize();
}

int Media::APE::APEIO::GetName(WChar * pBuffer)
{
	Text::StrConcat(pBuffer, this->data->GetFullName());
	return 0;
}

NN<IO::StreamData> Media::APE::APEIO::GetData()
{
	return this->data;
}
