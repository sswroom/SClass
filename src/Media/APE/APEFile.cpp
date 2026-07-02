#include "Stdafx.h"
#include "Media/APE/APEAudioSource.h"
#include "Media/APE/APEFile.h"

Media::MediaFile *Media::APE::APEFile::ParseData(NN<IO::StreamData> data)
{
	NN<Media::APE::APEIO> io;
	Int32 err;
	NEW_CLASSNN(io, Media::APE::APEIO(data));
	IAPEDecompress *ape = CreateIAPEDecompressEx(io.Ptr(), &err);
	if (ape)
	{
		NN<Media::APE::APEAudioSource> audStm;
		NN<Media::MediaFile> mf;
		NEW_CLASSNN(audStm, Media::APE::APEAudioSource(ape, io));
		NEW_CLASSNN(mf, Media::MediaFile(data->GetFullName()));
		mf->AddSource(audStm, 0);
		return mf;
	}
	else
	{
		DEL_CLASS(io);
		return 0;
	}
}
