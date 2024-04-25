#include "Stdafx.h"
#include "Media/APE/APEAudioSource.h"
#include "Media/APE/APEFile.h"

Media::MediaFile *Media::APE::APEFile::ParseData(NN<IO::StreamData> data)
{
	Media::APE::APEIO *io;
	Int32 err;
	NEW_CLASS(io, Media::APE::APEIO(data));
	IAPEDecompress *ape = CreateIAPEDecompressEx(io, &err);
	if (ape)
	{
		Media::APE::APEAudioSource *audStm;
		Media::MediaFile *mf;
		NEW_CLASS(audStm, Media::APE::APEAudioSource(ape, io));
		NEW_CLASS(mf, Media::MediaFile(data->GetFullName()));
		mf->AddSource(audStm, 0);
		return mf;
	}
	else
	{
		DEL_CLASS(io);
		return 0;
	}
}
