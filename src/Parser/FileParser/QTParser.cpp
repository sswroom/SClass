#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "Data/FastMap.h"
#include "Math/Math.h"
#include "Media/AudioFrameSource.h"
#include "Media/AACFrameSource.h"
#include "Media/AudioFormat.h"
#include "Media/FileVideoSource.h"
#include "Media/FrameInfo.h"
#include "Media/H264Parser.h"
#include "Media/MPEGVideoParser.h"
#include "Parser/FileParser/QTParser.h"
#undef SetProp

//https://developer.apple.com/library/mac/#documentation/QuickTime/QTFF/QTFFPreface/qtffPreface.html

Parser::FileParser::QTParser::QTParser()
{
}

Parser::FileParser::QTParser::~QTParser()
{
}

Int32 Parser::FileParser::QTParser::GetName()
{
	return *(Int32*)"QTFF";
}

void Parser::FileParser::QTParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.mov"), CSTR("Quicktime Movie File"));
		selector->AddFilter(CSTR("*.mp4"), CSTR("MP4 File"));
	}
}

IO::ParserType Parser::FileParser::QTParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

Optional<IO::ParsedObject> Parser::FileParser::QTParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt64 size;
	UInt64 ofst = 0;
	UInt64 endOfst = fd->GetDataSize();
	UInt8 buff[16];
	if (*(Int32*)&hdr[4] == *(Int32*)"ftyp")
	{
		ofst = ReadMUInt32(&hdr[0]);
	}
	else if (*(Int32*)&hdr[4] == *(Int32*)"moov")
	{
		ofst = 0;
	}
	else
	{
		return 0;
	}

	while (true)
	{
		if (fd->GetRealData(ofst, 16, BYTEARR(buff)) < 8)
			return 0;
		size = ReadMUInt32(&buff[0]);
		if (size == 1)
		{
			size = ReadMUInt64(&buff[8]);
		}
		if (size == 0 || ofst + size > endOfst)
		{
			return 0;
		}
		if (*(Int32*)&buff[4] == *(Int32*)"free")
		{
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"skip")
		{
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"mdat")
		{
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"moov")
		{
			return this->ParseMoovAtom(fd, ofst, size);
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"wide")
		{

		}
		else if (*(Int32*)&buff[4] == *(Int32*)"uuid")
		{
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"yqoo")
		{
		}
		else
		{
			return 0;
		}
		ofst += size;
	}
	return 0;
}

Media::MediaFile *Parser::FileParser::QTParser::ParseMoovAtom(NN<IO::StreamData> fd, UInt64 ofst, UInt64 size)
{
	UInt64 i;
	UInt8 hdr[8];
	UInt8 buff[256];
	UInt32 atomSize;
	UInt32 mvTimeScale = 1;
	Int32 trackSkip;
	Int32 trackDelay;
	Int32 vTrackDelay = 0;
	Int32 vTrackSkip = 0;
	Media::MediaFile *file;
	NN<Media::IMediaSource> src;
	NEW_CLASS(file, Media::MediaFile(fd->GetFullName()));
	i = 8;
	while (i < size)
	{
		if (fd->GetRealData(ofst + i, 8, BYTEARR(hdr)) != 8)
			return 0;
		atomSize = ReadMUInt32(&hdr[0]);
		if (atomSize < 8 || i + atomSize > size)
		{
			break;
		}

		if (*(Int32*)&hdr[4] == *(Int32*)"prfl")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"mvhd")
		{
			fd->GetRealData(ofst + i + 8, atomSize - 8, BYTEARR(buff));
			mvTimeScale = ReadMUInt32(&buff[12]);
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"clip")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"trak")
		{
			if (src.Set(ParseTrakAtom(fd, ofst + i, atomSize, &trackDelay, &trackSkip, mvTimeScale)))
			{
				if (src->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
				{
					vTrackDelay = trackDelay;
					vTrackSkip = trackSkip;
				}
				Int32 d1 = MulDiv32(vTrackDelay - trackDelay, 1000, (Int32)mvTimeScale);
				Int32 d2 = (vTrackSkip - trackSkip);
				file->AddSource(src, -d1 + d2);
			}
			else
			{
			}
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"udta")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"ctab")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"cmov")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"rmra")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"iods") //Initial Object Descriptors
		{
			/////////////////////////////////
		}
		else
		{
		}
		i += atomSize;
	}
	if (file->GetStream(0, 0).IsNull())
	{
		DEL_CLASS(file);
		file = 0;
	}
	return file;
}

Media::IMediaSource *Parser::FileParser::QTParser::ParseTrakAtom(NN<IO::StreamData> fd, UInt64 ofst, UInt32 size, Int32 *trackDelay, Int32 *trackSkipMS, UInt32 mvTimeScale)
{
	UInt32 i;
	UInt8 hdr[8];
//	UInt8 buff[256];
	UInt32 atomSize;
	Int32 sampleSkip = 0;
	Int32 delay = 0;
	UInt32 mediaTimeScale = mvTimeScale;
	Media::IMediaSource *src = 0;
	i = 8;
	while (i < size)
	{
		if (fd->GetRealData(ofst + i, 8, BYTEARR(hdr)) != 8)
			return 0;
		atomSize = ReadMUInt32(&hdr[0]);
		if (atomSize < 8 || i + atomSize > size)
		{
			break;
		}
		if (*(Int32*)&hdr[4] == *(Int32*)"prfl")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"tkhd")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"tapt")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"clip")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"matt")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"edts")
		{
			ParseEdtsAtom(fd, ofst + i, atomSize, &delay, &sampleSkip);
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"tref")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"txas")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"load")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"imap")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"mdia")
		{
			SDEL_CLASS(src);
			src = ParseMdiaAtom(fd, ofst + i, atomSize, &mediaTimeScale);
			if (mediaTimeScale == 0)
			{
				mediaTimeScale = mvTimeScale;
			}
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"udta")
		{
			/////////////////////////////////
		}
		else
		{
			hdr[0] = 0;
		}
		i += atomSize;
	}
	*trackDelay = delay;
	*trackSkipMS = MulDiv32(sampleSkip, 1000, (Int32)mediaTimeScale);
	return src;
}

Media::IMediaSource *Parser::FileParser::QTParser::ParseMdiaAtom(NN<IO::StreamData> fd, UInt64 ofst, UInt32 size, UInt32 *timeScaleOut)
{
	UInt32 i;
	UInt8 hdr[8];
	UInt8 buff[256];
	UInt32 atomSize;
	UInt32 timeScale = *timeScaleOut;
	Media::IMediaSource *src = 0;
	Media::MediaType mtyp = Media::MEDIA_TYPE_UNKNOWN;
	i = 8;
	while (i < size)
	{
		if (fd->GetRealData(ofst + i, 8, BYTEARR(hdr)) != 8)
			return 0;
		atomSize = ReadMUInt32(&hdr[0]);
		if (atomSize < 8 || i + atomSize > size)
		{
			break;
		}
		if (*(Int32*)&hdr[4] == *(Int32*)"mdhd")
		{
			fd->GetRealData(ofst + i + 8, atomSize, BYTEARR(buff));
			timeScale = ReadMUInt32(&buff[12]);
			if (timeScale == 0)
			{
//				timeScale = 10000000;
			}
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"elng")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"hdlr")
		{
			fd->GetRealData(ofst + i + 8, 12, BYTEARR(buff));
			if (*(Int32*)&buff[8] == *(Int32*)"vide")
			{
				mtyp = Media::MEDIA_TYPE_VIDEO;
			}
			else if (*(Int32*)&buff[8] == *(Int32*)"soun")
			{
				mtyp = Media::MEDIA_TYPE_AUDIO;
			}
			else if (*(Int32*)&buff[8] == *(Int32*)"subt")
			{
				mtyp = Media::MEDIA_TYPE_SUBTITLE;
			}
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"minf")
		{
			SDEL_CLASS(src);
			if (mtyp == Media::MEDIA_TYPE_VIDEO || mtyp == Media::MEDIA_TYPE_AUDIO || mtyp == Media::MEDIA_TYPE_SUBTITLE)
			{
				src = ParseMinfAtom(fd, ofst + i, atomSize, mtyp, timeScale);
			}
			else
			{
				hdr[0] = 0;
			}
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"udta")
		{
			/////////////////////////////////
		}
		else
		{
			hdr[0] = 0;
		}
		i += atomSize;
	}
	*timeScaleOut = timeScale;
	return src;
}

Media::IMediaSource *Parser::FileParser::QTParser::ParseMinfAtom(NN<IO::StreamData> fd, UInt64 ofst, UInt32 size, Media::MediaType mtyp, UInt32 timeScale)
{
	UInt32 i;
	UInt8 hdr[8];
//	UInt8 buff[256];
	UInt32 atomSize;
	Media::IMediaSource *src = 0;
	i = 8;
	while (i < size)
	{
		if (fd->GetRealData(ofst + i, 8, BYTEARR(hdr)) != 8)
			return 0;
		atomSize = ReadMUInt32(&hdr[0]);
		if (atomSize < 8 || i + atomSize > size)
		{
			break;
		}
		if (*(Int32*)&hdr[4] == *(Int32*)"vmhd")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"smhd")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"hdlr")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"dinf")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"stbl")
		{
			src = ParseStblAtom(fd, ofst + i, atomSize, mtyp, timeScale);
		}
		else
		{
			hdr[0] = 0;
		}
		i += atomSize;
	}
	return src;
}

Media::IMediaSource *Parser::FileParser::QTParser::ParseStblAtom(NN<IO::StreamData> fd, UInt64 ofst, UInt32 size, Media::MediaType mtyp, UInt32 timeScale)
{
	UInt32 i;
	UInt8 hdr[8];
	UInt8 dataBuff[1024];
	Data::ByteBuffer dataBuff2(0);
	Data::ByteArray buff = BYTEARR(dataBuff);
	UInt32 atomSize;
	Media::IMediaSource *src = 0;
	Media::AudioFormat afmt;
	Media::FrameInfo frInfo;
//	Int32 frameCnt = 0;
	UInt64 ttsOfst = 0;
	UInt32 ttsAtomSize = 0;
	UInt64 stcOfst = 0;
	UInt32 stcAtomSize = 0;
	UInt64 stszOfst = 0;
	UInt32 stszAtomSize = 0;
	UInt64 stcoOfst = 0;
	UInt32 stcoAtomSize = 0;
	UInt64 stssOfst = 0;
	UInt32 stssAtomSize = 0;
	UOSInt atomOfst;
	UInt32 subAtomSize;
	UInt32 frameRate = 24;
	UInt32 frameRateDenorm = 1;
	UInt64 avccOfst = 0;
	UInt32 avccAtomSize = 0;

	if (mtyp == Media::MEDIA_TYPE_VIDEO)
	{
		frInfo.fourcc = (UInt32)-1;
		frInfo.storeSize = Math::Size2D<UOSInt>(0, 0);
		frInfo.dispSize = Math::Size2D<UOSInt>(0, 0);
		frInfo.storeBPP = 0;
		frInfo.pf = Media::PF_UNKNOWN;
		frInfo.byteSize = 0;
		frInfo.par2 = 1;
		frInfo.hdpi = 96;
		frInfo.vdpi = 96;
		frInfo.ftype = Media::FT_NON_INTERLACE;
		frInfo.atype = Media::AT_NO_ALPHA;
		frInfo.color.SetCommonProfile(Media::ColorProfile::CPT_VUNKNOWN);
		frInfo.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
		frInfo.ycOfst = Media::YCOFST_C_CENTER_LEFT;
		frInfo.rotateType = Media::RotateType::None;
	}

	i = 8;
	while (i < size)
	{
		if (fd->GetRealData(ofst + i, 8, BYTEARR(hdr)) != 8)
			return 0;
		atomSize = ReadMUInt32(&hdr[0]);
		if (atomSize < 8 || i + atomSize > size)
		{
			break;
		}
		if (*(Int32*)&hdr[4] == *(Int32*)"stsd")
		{
//			Int32 nEnt;
//			Int32 descSize;
			if (atomSize > 1032)
			{
				dataBuff2.ChangeSize(atomSize - 8);
				buff = dataBuff2;
				fd->GetRealData(ofst + i + 8, atomSize - 8, buff);
			}
			else
			{
				fd->GetRealData(ofst + i + 8, atomSize - 8, BYTEARR(dataBuff));
				buff = BYTEARR(dataBuff);
			}
//			nEnt = ReadMInt32(&buff[4]);
//			descSize = ReadMInt32(&buff[8]);
			if (mtyp == Media::MEDIA_TYPE_VIDEO)
			{
				frInfo.dispSize.x = ReadMUInt16(&buff[40]);
				frInfo.dispSize.y = ReadMUInt16(&buff[42]);
				frInfo.storeSize = frInfo.dispSize;
				frInfo.storeBPP = ReadMUInt16(&buff[90]);
				frInfo.pf = Media::PF_UNKNOWN;
//				frameCnt = ReadMUInt16(&buff[56]);
				atomOfst = 94;
				if (*(Int32*)&buff[12] == *(Int32*)"cvid")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"jpeg")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"smc ")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"rle ")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"rpza")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"kpcd")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"png ")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"mjpa")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"mjpb")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"SVQ1")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"SVQ3")
				{
					frInfo.fourcc = *(UInt32*)"SVQ3";
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"mp4v")
				{
					frInfo.fourcc = *(UInt32*)"XVID";
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"avc1")
				{
					frInfo.fourcc = *(UInt32*)"ravc";
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"hvc1")
				{
					frInfo.fourcc = *(UInt32*)"rhvc";
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"hev1")
				{
					frInfo.fourcc = *(UInt32*)"HEVC";
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"m2v1")
				{
					frInfo.fourcc = *(UInt32*)"m2v1";
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"dvc ")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"dvcp")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"gif ")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"h263")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"tiff")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"raw ")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"2vuY")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"yuv2")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"v308")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"v408")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"v216")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"v410")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"v210")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"XVID")
				{
					frInfo.fourcc = *(UInt32*)"XVID";
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"VP6F")
				{
					frInfo.fourcc = *(UInt32*)"VP6F";
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"FLV1")
				{
					frInfo.fourcc = *(UInt32*)"FLV1";
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"CFHD")
				{
					frInfo.fourcc = *(UInt32*)"CFHD";
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"VP90")
				{
					frInfo.fourcc = *(UInt32*)"VP90";
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"vp09")
				{
					frInfo.fourcc = *(UInt32*)"vp09";
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"av01")
				{
					frInfo.fourcc = *(UInt32*)"av01";
				}

				while (atomOfst < atomSize - 8)
				{
					subAtomSize = ReadMUInt32(&buff[atomOfst]);
					if (subAtomSize == 0)
						break;
					if (*(Int32*)&buff[atomOfst + 4] == *(Int32*)"gama")
					{
					}
					else if (*(Int32*)&buff[atomOfst + 4] == *(Int32*)"fiel")
					{
					}
					else if (*(Int32*)&buff[atomOfst + 4] == *(Int32*)"mjqt")
					{
					}
					else if (*(Int32*)&buff[atomOfst + 4] == *(Int32*)"mjht")
					{
					}
					else if (*(Int32*)&buff[atomOfst + 4] == *(Int32*)"esds")
					{
					}
					else if (*(Int32*)&buff[atomOfst + 4] == *(Int32*)"avcC")
					{
						avccOfst = ofst + i + 8 + atomOfst;
						avccAtomSize = subAtomSize;
					}
					else if (*(Int32*)&buff[atomOfst + 4] == *(Int32*)"hvcC")
					{
						avccOfst = ofst + i + 8 + atomOfst;
						avccAtomSize = subAtomSize;
					}
					else if (*(Int32*)&buff[atomOfst + 4] == *(Int32*)"pasp")
					{
						frInfo.par2 = ReadMUInt32(&buff[atomOfst + 8]) / (Double)ReadMUInt32(&buff[atomOfst + 12]);
						frInfo.vdpi = frInfo.hdpi / frInfo.par2;
					}
					else if (*(Int32*)&buff[atomOfst + 4] == *(Int32*)"colr")
					{
						if (*(Int32*)&buff[atomOfst + 8] == *(Int32*)"nclc")
						{
							Media::CS::TransferType ttype = Media::CS::TRANT_VUNKNOWN;
							switch (ReadMUInt16(&buff[atomOfst + 14])) //Transfer Function
							{
							case 1:
								ttype = Media::CS::TRANT_BT709;
								break;
							case 7:
								ttype = Media::CS::TRANT_SMPTE240;
								break;
							}
							if (ttype != Media::CS::TRANT_VUNKNOWN)
							{
								frInfo.color.GetRTranParam()->Set(ttype, 2.2);
								frInfo.color.GetGTranParam()->Set(ttype, 2.2);
								frInfo.color.GetBTranParam()->Set(ttype, 2.2);
							}

							switch (ReadMUInt16(&buff[atomOfst + 16])) //YUV Matrix
							{
							case 1:
								frInfo.yuvType = Media::ColorProfile::YUVT_BT709;
								break;
							case 6:
								frInfo.yuvType = Media::ColorProfile::YUVT_SMPTE170M;
								break;
							case 7:
								frInfo.yuvType = Media::ColorProfile::YUVT_SMPTE240M;
								break;
							}
							switch (ReadMUInt16(&buff[atomOfst + 12])) //Color Primaries
							{
							case 1:
								frInfo.color.GetPrimaries()->SetColorType(Media::ColorProfile::CT_SRGB);
								break;
							case 5:
								frInfo.color.GetPrimaries()->SetColorType(Media::ColorProfile::CT_BT470BG);
								break;
							case 6:
								frInfo.color.GetPrimaries()->SetColorType(Media::ColorProfile::CT_SMPTE240M);
								break;
							}
						}
					}
					else if (*(Int32*)&buff[atomOfst + 4] == *(Int32*)"clap")
					{
					}
					else if (*(Int32*)&buff[atomOfst + 4] == *(Int32*)"glbl")
					{
						if (frInfo.fourcc == *(UInt32*)"m2v1")
						{
							UInt32 norm;
							UInt32 denorm;
							UInt64 br;
							Media::MPEGVideoParser::GetFrameInfo(&buff[atomOfst + 8], subAtomSize - 8, frInfo, norm, denorm, &br, false);
							frInfo.fourcc = *(UInt32*)"m2v1";
						}
					}
					else
					{
					
					}

					atomOfst += subAtomSize;
				}
			}
			else if (mtyp == Media::MEDIA_TYPE_AUDIO)
			{
				UOSInt bofst;
				if (ReadMInt16(&buff[24]) == 0)
				{
					afmt.nChannels = ReadMUInt16(&buff[32]);
					afmt.bitpersample = ReadMUInt16(&buff[34]);
					afmt.frequency = ReadMUInt16(&buff[40]);
					afmt.bitRate = 0;
					bofst = 44;
				}
				else if (ReadMInt16(&buff[24]) == 1)
				{
					afmt.nChannels = ReadMUInt16(&buff[32]);
					afmt.bitpersample = ReadMUInt16(&buff[34]);
					afmt.frequency = ReadMUInt16(&buff[40]);
					afmt.align = ReadMUInt32(&buff[56]);
					afmt.bitRate = 0;
					bofst = 60;
				}
				else if (ReadMInt16(&buff[24]) == 2)
				{
					afmt.nChannels = (UInt16)ReadMUInt32(&buff[56]);
					afmt.bitpersample = (UInt16)ReadMUInt32(&buff[64]);
					afmt.frequency = (UInt32)Double2Int32(ReadMDouble(&buff[48]));
					afmt.bitRate = 0;
					bofst = 80;
				}
				else
				{
					bofst = 0;
				}
				if (afmt.frequency == 0)
				{
					afmt.frequency = timeScale;
				}

				if (*(Int32*)&buff[12] == 0)
				{
					afmt.formatId = 1;
					afmt.intType = Media::AudioFormat::IT_BIGENDIAN;
					afmt.align = (UInt32)(afmt.nChannels * afmt.bitpersample >> 3);
					afmt.bitRate = afmt.align * afmt.frequency << 3;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"NONE")
				{
					afmt.formatId = 1;
					afmt.intType = Media::AudioFormat::IT_BIGENDIAN;
					afmt.align = (UInt32)(afmt.nChannels * afmt.bitpersample >> 3);
					afmt.bitRate = afmt.align * afmt.frequency << 3;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"raw ")
				{
					afmt.formatId = 1;
					afmt.intType = Media::AudioFormat::IT_NORMAL;
					afmt.align = (UInt32)(afmt.nChannels * afmt.bitpersample >> 3);
					afmt.bitRate = afmt.align * afmt.frequency << 3;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"twos")
				{
					afmt.formatId = 1;
					afmt.intType = Media::AudioFormat::IT_BIGENDIAN;
					afmt.align = (UInt32)(afmt.nChannels * afmt.bitpersample >> 3);
					afmt.bitRate = afmt.align * afmt.frequency << 3;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"sowt")
				{
					afmt.formatId = 1;
					afmt.intType = Media::AudioFormat::IT_NORMAL;
					afmt.align = (UInt32)(afmt.nChannels * afmt.bitpersample >> 3);
					afmt.bitRate = afmt.align * afmt.frequency << 3;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"lpcm")
				{
					afmt.formatId = 1;
					afmt.intType = Media::AudioFormat::IT_NORMAL;
					afmt.align = (UInt32)(afmt.nChannels * afmt.bitpersample >> 3);
					afmt.bitRate = afmt.align * afmt.frequency << 3;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"MAC3")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"MAC6")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"ima4")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"fl32")
				{
					afmt.formatId = 3;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"fl64")
				{
					afmt.formatId = 3;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"in24")
				{
					afmt.formatId = 1;
					afmt.intType = Media::AudioFormat::IT_NORMAL;
					afmt.bitpersample = 24;
					afmt.align = 3 * (UInt32)afmt.nChannels;
					afmt.bitRate = afmt.align * afmt.frequency << 3;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"in32")
				{
					afmt.formatId = 1;
					afmt.intType = Media::AudioFormat::IT_NORMAL;
					afmt.bitpersample = 32;
					afmt.align = 4 * (UInt32)afmt.nChannels;
					afmt.bitRate = afmt.align * afmt.frequency << 3;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"ulaw")
				{
					afmt.formatId = 7;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"alaw")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"dvca")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"QDMC")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"QDM2")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"Qclp")
				{
				}
				else if (*(Int32*)&buff[12] == *(Int32*)".mp3")
				{
					afmt.formatId = 0x55;
					afmt.align = 1;
					if (afmt.extra == 0)
					{
						afmt.extraSize = 12;
						afmt.extra = MemAlloc(UInt8, 12);
						*(Int16*)&afmt.extra[0] = 1; //wID
						*(Int32*)&afmt.extra[2] = 2; //fdwFlags
						*(Int16*)&afmt.extra[6] = 0x2bc; //nBlockSize
						*(Int16*)&afmt.extra[8] = 1; //nFramesPerBlock
						*(Int16*)&afmt.extra[10] = 0; //nCodecDelay
					}
					if (buff[36] == 0xff && (buff[37] & 0xf8) == 0xf8)
					{
						
					}
				}
				else if (*(Int32*)&buff[12] == *(Int32*)".mp2")
				{
					afmt.formatId = 0x50;
					afmt.align = 1;
					if (afmt.extra == 0)
					{
						afmt.extraSize = 12;
						afmt.extra = MemAlloc(UInt8, 12);
						*(Int16*)&afmt.extra[0] = 2; //wID
						*(Int32*)&afmt.extra[2] = 211712; //fdwFlags
						*(Int16*)&afmt.extra[6] = 1; //nBlockSize
						*(Int16*)&afmt.extra[8] = 1; //nFramesPerBlock
						*(Int16*)&afmt.extra[10] = 1; //nCodecDelay
					}
					if (buff[36] == 0xff && (buff[37] & 0xf8) == 0xf8)
					{
						
					}
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"mp4a")
				{
					afmt.formatId = 255;
					afmt.align = 1;
					afmt.bitRate = 128000;
					afmt.extraSize = 2;
					afmt.extra = MemAlloc(UInt8, 2);
	//   LSB      MSB
	// xxxxx... ........  object type (00010 = AAC LC)
	// .....xxx x.......  sampling frequency index (0 to 11)
	// ........ .xxxx...  channels configuration (1 to 7)
	// ........ .....x..  frame length flag (0 = 1024, 1 = 960)
	// ........ ......x.  depends on core coder (0)
	// ........ .......x  extensions flag (0)
					afmt.extra[0] = 0x10;
					afmt.extra[1] = (UInt8)(afmt.nChannels << 3);
					switch (afmt.frequency)
					{
					case 96000:
						afmt.extra[0] |= 0;
						afmt.extra[1] |= 0;
						break;
					case 88200:
						afmt.extra[0] |= 0;
						afmt.extra[1] |= 0x80;
						break;
					case 64000:
						afmt.extra[0] |= 1;
						afmt.extra[1] |= 0;
						break;
					case 48000:
						afmt.extra[0] |= 1;
						afmt.extra[1] |= 0x80;
						break;
					case 44100:
						afmt.extra[0] |= 2;
						afmt.extra[1] |= 0;
						break;
					case 32000:
						afmt.extra[0] |= 2;
						afmt.extra[1] |= 0x80;
						break;
					case 24000:
						afmt.extra[0] |= 3;
						afmt.extra[1] |= 0x00;
						break;
					case 22050:
						afmt.extra[0] |= 3;
						afmt.extra[1] |= 0x80;
						break;
					case 16000:
						afmt.extra[0] |= 4;
						afmt.extra[1] |= 0x00;
						break;
					case 12000:
						afmt.extra[0] |= 4;
						afmt.extra[1] |= 0x80;
						break;
					case 11025:
						afmt.extra[0] |= 5;
						afmt.extra[1] |= 0x00;
						break;
					case 8000:
						afmt.extra[0] |= 5;
						afmt.extra[1] |= 0x80;
						break;
					}

					//WriteInt16(afmt.extra, 0x4015);
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"ac-3")
				{
					afmt.formatId = 0x2000;
					afmt.align = 1;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"AC-3")
				{
					afmt.formatId = 0x2000;
					afmt.align = 1;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"nmos")
				{
					afmt.formatId = 0xa200;
					afmt.align = 1;
				}
				else if (*(Int32*)&buff[12] == *(Int32*)"HDMV")
				{
/*					afmt.formatId = 1;
					afmt.intType = Media::AudioFormat::IT_BIGENDIAN;
					afmt.align = afmt.nChannels * afmt.bitpersample >> 3;
					afmt.bitRate = afmt.align * afmt.frequency << 3;*/
				}
				else if (buff[12] == 0x6d && buff[13] == 0x73)
				{
					afmt.formatId = ReadMUInt16(&buff[14]);
					if (atomSize > 0x64)
					{
						afmt.extraSize = atomSize - 0x64;
						afmt.extra = MemAlloc(UInt8, afmt.extraSize);
						MemCopyNO(afmt.extra, &buff[0x44], afmt.extraSize);
					}
				}

				if (bofst != 0)
				{
					while (bofst < atomSize - 8)
					{
						subAtomSize = ReadMUInt32(&buff[bofst]);
						if (subAtomSize == 0)
							break;
						if (*(Int32*)&buff[bofst + 4] == *(Int32*)"esds")
						{
							if (afmt.formatId == 255)
							{
								if (subAtomSize >= 0x2e && Text::StrEqualsCh((Char*)&buff[bofst + 0x2f], "vorbis"))
								{
									afmt.formatId = 0x566f;
									afmt.extraSize = subAtomSize - 49;
									if (afmt.extra)
									{
										MemFree(afmt.extra);
									}
									afmt.extra = MemAlloc(UInt8, afmt.extraSize);
									MemCopyNO(afmt.extra, &buff[bofst + 0x2b], afmt.extraSize);
								}
								else if (buff[bofst + 13] == 0x1e)
								{
									afmt.formatId = 0x1610;
									//afmt.frequency *= 2;
								}
	//							WriteInt16(afmt.extra, ReadMInt16(&buff[bofst + 19]));
							}
						}
						else if (*(Int32*)&buff[bofst + 4] == *(Int32*)"wave")
						{
							UInt8 *subBuff = &buff[bofst + 8];
							UOSInt subOfst = 0;
							Int32 subSubAtomSize;
							while (subOfst < subAtomSize - 8)
							{
								subSubAtomSize = ReadMInt32(&subBuff[subOfst]);
								if (subSubAtomSize <= 0)
									break;
								if (*(Int32*)&subBuff[subOfst + 4] == *(Int32*)"dac3")
								{
									static UInt32 bitrate[] = {32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 448, 512, 576, 640};
//									Int32 fscod = (subBuff[subOfst + 8] & 0xc0) >> 6;
//									Int32 bsid = (subBuff[subOfst + 8] & 0x3e) >> 1;
//									Int32 bsmod = ((subBuff[subOfst + 8] & 1) << 2) | ((subBuff[subOfst + 9] & 0xc0) >> 6);
//									Int32 acmod = (subBuff[subOfst + 9] & 0x38) >> 3;
//									Int32 lfeon = (subBuff[subOfst + 9] & 4) >> 2;
									Int32 bit_rate_code = ((subBuff[subOfst + 9] & 3) << 3) | ((subBuff[subOfst + 10] & 0xe0) >> 5);
									afmt.bitRate = bitrate[bit_rate_code] * 1000;
								}
								subOfst += (UInt32)subSubAtomSize;
							}
						}

						bofst += subAtomSize;
					}
				}
			}
			if (atomSize > 1032)
			{
				buff = BYTEARR(dataBuff);
			}
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"stts")
		{
			ttsOfst = ofst + i;
			ttsAtomSize = atomSize;
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"ctts")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"cslg")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"stss")
		{
			stssOfst = ofst + i;
			stssAtomSize = atomSize;
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"stps")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"stsc")
		{
			stcOfst = ofst + i;
			stcAtomSize = atomSize;
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"stsz")
		{
			stszOfst = ofst + i;
			stszAtomSize = atomSize;
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"stco")
		{
			stcoOfst = ofst + i;
			stcoAtomSize = atomSize;
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"co64")
		{
			stcoOfst = ofst + i;
			stcoAtomSize = atomSize;
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"stsh")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"sgpd")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"sbgp")
		{
			/////////////////////////////////
		}
		else if (*(Int32*)&hdr[4] == *(Int32*)"sdtp")
		{
			/////////////////////////////////
		}
		else
		{
			hdr[0] = 0;
			if (atomSize < 8)
			{
				break;
			}
		}
		i += atomSize;
	}

	if (mtyp == Media::MEDIA_TYPE_AUDIO)
	{
		if (afmt.formatId != 0 && stszOfst != 0 && stcOfst != 0 && ttsOfst != 0 && stcoOfst != 0)
		{
			Media::IAudioFrameSource *asrc = 0;
//			Int32 ntts;
			Int32 nstc;
//			Int32 nstco;
			Int32 nstsz;
			UOSInt ttsBuffOfst;
			OSInt stcBuffOfst;
			OSInt stcoBuffOfst;
			OSInt stszBuffOfst;
			Int32 ttsNLeft;
			UInt32 ttsDur;
			Bool stco64;
			Int32 samplePerChunk;
			Int32 chunkId;
			Int32 chunkSample;
			Int32 nSample;
			Int32 currStc;
			UInt64 currChOfst;
			UInt32 currSampleSize;

			Data::ByteBuffer ttsBuff(ttsAtomSize);
			Data::ByteBuffer stcBuff(stcAtomSize);
			Data::ByteBuffer stcoBuff(stcoAtomSize);
			Data::ByteBuffer stszBuff(stszAtomSize);

			fd->GetRealData(ttsOfst, ttsAtomSize, ttsBuff);
			fd->GetRealData(stcOfst, stcAtomSize, stcBuff);
			fd->GetRealData(stcoOfst, stcoAtomSize, stcoBuff);
			fd->GetRealData(stszOfst, stszAtomSize, stszBuff);

			currSampleSize = ReadMUInt32(&stszBuff[12]);
			if (currSampleSize > 0 && currSampleSize < afmt.align)
			{
				currSampleSize = afmt.align;
			}

			if (afmt.formatId == 0x55)
			{
				if (currSampleSize == 0)
				{
					currSampleSize = ReadMUInt32(&stszBuff[20]);
				}
				*(UInt16*)&afmt.extra[6] = (UInt16)currSampleSize; //nBlockSize
				afmt.bitpersample = 0;
			}

			if (afmt.formatId == 255 || afmt.formatId == 0x1610)
			{
				NEW_CLASS(asrc, Media::AACFrameSource(fd, afmt, fd->GetFullName()));
			}
			else
			{
				NEW_CLASS(asrc, Media::AudioFrameSource(fd, afmt, fd->GetFullName()));
			}

//			ntts = ReadMInt32(&ttsBuff[12]);
			ttsBuffOfst = 16;
			ttsNLeft = 0;
			ttsDur = 0;
			nstc = ReadMInt32(&stcBuff[12]);
			stcBuffOfst = 16;
			stco64 = *(Int32*)&stcoBuff[4] == *(Int32*)"co64";
//			nstco = ReadMInt32(&stcoBuff[12]);
			stcoBuffOfst = 16;
			stszBuffOfst = 16;
			chunkId = 0;
			nSample = 0;
			chunkSample = 0;
			samplePerChunk = 0;
			currChOfst = 0;
			currStc = 0;
			nstsz = ReadMInt32(&stszBuff[16]);
			if (currSampleSize == 0)
			{
				stszBuffOfst = 20;

				while (nSample < nstsz)
				{
					currSampleSize = ReadMUInt32(&stszBuff[stszBuffOfst]);
					stszBuffOfst += 4;
					if (chunkSample >= samplePerChunk)
					{
						chunkId++;
						chunkSample = 0;
						if (currStc < nstc && ReadMInt32(&stcBuff[stcBuffOfst]) <= chunkId)
						{
							samplePerChunk = ReadMInt32(&stcBuff[stcBuffOfst + 4]);
							stcBuffOfst += 12;
							currStc++;
						}
						if (stco64)
						{
							currChOfst = ReadMUInt64(&stcoBuff[stcoBuffOfst]);
							stcoBuffOfst += 8;
						}
						else
						{
							currChOfst = ReadMUInt32(&stcoBuff[stcoBuffOfst]);
							stcoBuffOfst += 4;
						}
					}
					if (--ttsNLeft <= 0 && ttsBuffOfst < ttsAtomSize)
					{
						ttsNLeft = ReadMInt32(&ttsBuff[ttsBuffOfst]);
						ttsDur = ReadMUInt32(&ttsBuff[ttsBuffOfst + 4]);
						ttsBuffOfst += 8;
					}

					asrc->AddBlock(currChOfst, currSampleSize, ttsDur);
					currChOfst += currSampleSize;
					chunkSample++;
					nSample++;
				}
			}
			else
			{
				UInt64 lastOfst = (UInt64)-1;
				UInt64 lastEndOfst = (UInt64)-1;
				UInt32 lastSampleCnt = 0;
				while (nSample < nstsz)
				{
					if (chunkSample >= samplePerChunk)
					{
						chunkId++;
						chunkSample = 0;
						if (currStc < nstc && ReadMInt32(&stcBuff[stcBuffOfst]) <= chunkId)
						{
							samplePerChunk = ReadMInt32(&stcBuff[stcBuffOfst + 4]);
							stcBuffOfst += 12;
							currStc++;
						}
						if (stco64)
						{
							currChOfst = ReadMUInt64(&stcoBuff[stcoBuffOfst]);
							stcoBuffOfst += 8;
						}
						else
						{
							currChOfst = ReadMUInt32(&stcoBuff[stcoBuffOfst]);
							stcoBuffOfst += 4;
						}
					}
					if (ttsNLeft-- <= 0)
					{
						ttsNLeft = ReadMInt32(&ttsBuff[ttsBuffOfst]);
						ttsDur = ReadMUInt32(&ttsBuff[ttsBuffOfst + 4]);
						ttsBuffOfst += 8;
					}
					
					if (afmt.formatId == 1)
					{
						if (lastSampleCnt >= (afmt.frequency >> 2) || lastEndOfst != currChOfst)
						{
							if (lastSampleCnt > 0)
							{
								asrc->AddBlock(lastOfst, (UInt32)(lastEndOfst - lastOfst), lastSampleCnt);
							}
							lastSampleCnt = 0;
							lastOfst = currChOfst;
							lastEndOfst = currChOfst;
						}
						lastEndOfst += currSampleSize;
						lastSampleCnt += ttsDur;
					}
					else
					{
						asrc->AddBlock(currChOfst, currSampleSize, ttsDur);
					}

					currChOfst += currSampleSize;
					chunkSample++;
					nSample++;
				}
				if (afmt.formatId == 1)
				{
					if (lastSampleCnt)
					{
						asrc->AddBlock(lastOfst, (UInt32)(lastEndOfst - lastOfst), lastSampleCnt);
						lastSampleCnt = 0;
					}
				}
			}

			src = asrc;
		}
	}
	else if (mtyp == Media::MEDIA_TYPE_VIDEO)
	{
		if (frInfo.fourcc != 0xFFFFFFFF && stszOfst != 0 && stcOfst != 0 && ttsOfst != 0 && stcoOfst != 0 && frameRate != 0)
		{
//			UInt32 ntts;
			UInt32 nstc;
//			UInt32 nstco;
			UInt32 nstsz;
			OSInt ttsBuffOfst;
			OSInt stcBuffOfst;
			OSInt stcoBuffOfst;
			OSInt stszBuffOfst;
			Int64 currTime;
			Int32 ttsNLeft;
			Int32 ttsDur;
			Bool stco64;
			Int32 samplePerChunk;
			Int32 chunkId;
			Int32 chunkSample;
			UInt32 nSample;
			UInt32 currStc;
			UInt64 currChOfst;
			UInt32 currSampleSize;

			Data::FastMap<UInt32, Int32> keyMap;
			keyMap.Put(0, 1);

			Media::FileVideoSource *fsrc;
			NEW_CLASS(fsrc, Media::FileVideoSource(fd, frInfo, frameRate, frameRateDenorm, timeScale != 0));
			Data::ByteBuffer ttsBuff(ttsAtomSize);
			Data::ByteBuffer stcBuff(stcAtomSize);
			Data::ByteBuffer stcoBuff(stcoAtomSize);
			Data::ByteBuffer stszBuff(stszAtomSize);

			fd->GetRealData(ttsOfst, ttsAtomSize, ttsBuff);
			fd->GetRealData(stcOfst, stcAtomSize, stcBuff);
			fd->GetRealData(stcoOfst, stcoAtomSize, stcoBuff);
			fd->GetRealData(stszOfst, stszAtomSize, stszBuff);

			UInt32 lastKey = 0;
			if (stssOfst != 0)
			{
				Data::ByteBuffer stssBuff(stssAtomSize);
				fd->GetRealData(stssOfst, stssAtomSize, stssBuff);

				UInt32 thisKey;
				nSample = 16;
				while (nSample < stssAtomSize)
				{
					thisKey = ReadMUInt32(&stssBuff[nSample]) - 1;
					while (thisKey - lastKey > 300)
					{
						lastKey += 300;
						keyMap.Put(lastKey, 1);
					}
					keyMap.Put(thisKey, 1);
					lastKey = thisKey;
					nSample += 4;
				}
			}
			else
			{
				nstsz = ReadMUInt32(&stszBuff[16]);
				nSample = 0;
				while (nSample < nstsz)
				{
					keyMap.Put(nSample, 1);
					nSample++;
				}
				lastKey = nstsz;
			}

//			ntts = ReadMInt32(&ttsBuff[12]);
			ttsBuffOfst = 16;
			ttsNLeft = 0;
			ttsDur = 0;
			currTime = 0;
			nstc = ReadMUInt32(&stcBuff[12]);
			stcBuffOfst = 16;
			stco64 = *(Int32*)&stcoBuff[4] == *(Int32*)"co64";
//			nstco = ReadMInt32(&stcoBuff[12]);
			stcoBuffOfst = 16;
			nstsz = ReadMUInt32(&stszBuff[16]);
			stszBuffOfst = 20;
			chunkId = 0;
			nSample = 0;
			chunkSample = 0;
			samplePerChunk = 0;
			currChOfst = 0;
			currTime = 0;
			currStc = 0;

			while (nstsz - lastKey > 300)
			{
				lastKey += 300;
				keyMap.Put(lastKey, 1);
			}

			while (nSample < nstsz)
			{
				currSampleSize = ReadMUInt32(&stszBuff[stszBuffOfst]);
				stszBuffOfst += 4;
				if (chunkSample >= samplePerChunk)
				{
					chunkId++;
					chunkSample = 0;
					if (currStc < nstc && ReadMInt32(&stcBuff[stcBuffOfst]) <= chunkId)
					{
						samplePerChunk = ReadMInt32(&stcBuff[stcBuffOfst + 4]);
						stcBuffOfst += 12;
						currStc++;
					}
					if (stco64)
					{
						currChOfst = ReadMUInt64(&stcoBuff[stcoBuffOfst]);
						stcoBuffOfst += 8;
					}
					else
					{
						currChOfst = ReadMUInt32(&stcoBuff[stcoBuffOfst]);
						stcoBuffOfst += 4;
					}
				}
				if (ttsNLeft-- <= 0)
				{
					ttsNLeft = ReadMInt32(&ttsBuff[ttsBuffOfst]) - 1;
					ttsDur = ReadMInt32(&ttsBuff[ttsBuffOfst + 4]);
					ttsBuffOfst += 8;
				}

				UInt32 t;
				if (timeScale == 0)
				{
					t = 0;
				}
				else
				{
					t = (UInt32)((UInt64)currTime * 1000 / timeScale);
				}
				fsrc->AddNewFrame(currChOfst, currSampleSize, keyMap.Get(nSample) != 0, t);
				currTime += ttsDur;
				currChOfst += currSampleSize;
				chunkSample++;
				nSample++;
			}

			if (avccOfst != 0)
			{
				if (frInfo.fourcc == *(UInt32*)"ravc")
				{
					Int32 nsps;
					stszBuff.ChangeSize(avccAtomSize);
					fd->GetRealData(avccOfst, avccAtomSize, stszBuff);

					fsrc->SetProp(*(Int32*)"AVCH", &stszBuff[8], avccAtomSize - 8);

					nsps = stszBuff[13] & 0x1f;
					stszOfst = 14;
					while (nsps-- > 0)
					{
						nSample = ReadMUInt16(&stszBuff[(UOSInt)stszOfst]);
						fsrc->SetProp(*(Int32*)"sps", &stszBuff[(UOSInt)stszOfst + 2], nSample);
						stszOfst += 2 + (UInt64)nSample;
					}
					nsps = stszBuff[(UOSInt)stszOfst];
					stszOfst += 1;
					while (nsps-- > 0)
					{
						nSample = ReadMUInt16(&stszBuff[(UOSInt)stszOfst]);
						fsrc->SetProp(*(Int32*)"pps", &stszBuff[(UOSInt)stszOfst + 2], nSample);
						stszOfst += 2 + (UInt64)nSample;
					}

					nsps = stszBuff[13] & 0x1f;
					stszOfst = 14;
					if(nsps > 0)
					{
						nSample = ReadMUInt16(&stszBuff[(UOSInt)stszOfst]);
						UInt8 *tmpBuff = MemAlloc(UInt8, (UOSInt)nSample + 4);
						MemCopyNO(&tmpBuff[4], &stszBuff[(UOSInt)stszOfst + 2], nSample);
						WriteMInt32(tmpBuff, 1);
						Media::H264Parser::H264Flags flags;
						flags.frameRateNorm = 0;
						flags.frameRateDenorm = 0;
						Media::FrameInfo fInfo;
						Media::H264Parser::GetFrameInfo(tmpBuff, (UOSInt)nSample + 4, fInfo, &flags);
						MemFree(tmpBuff);
						if (flags.frameRateDenorm != 0)
						{
							nSample = (UInt32)fsrc->GetFrameSize(0);
							tmpBuff = MemAlloc(UInt8, nSample);
							fsrc->ReadFrame(0, tmpBuff);

							MemFree(tmpBuff);
							fsrc->SetFrameRate(flags.frameRateNorm, flags.frameRateDenorm * 2);
						}
					}
				}
				else if (frInfo.fourcc == *(UInt32*)"rhvc")
				{
					OSInt narr;
					stszBuff.ChangeSize(avccAtomSize);
					fd->GetRealData(avccOfst, avccAtomSize, stszBuff);
/*
00 01
01 General Information (From VPS)
0d f000
0f fc
10 fc | chroma_idc
11 f8 | bit_depth_luma_minus8
12 f8 | bit_depth_chroma_minus8
13 0000
15 (((i_numTemporalLayer & 0x07) << 3) | (b_temporalIdNested << 2) | 0x03))
16 num_array
17 array[num_array]

array:
00 & 7f = array type (0x20 = VPS, 0x21 = SPS, 0x22 = PPS, 0x27 = SEI
01 array_count
03 array_item[array_count]

array_item:
00 array_size
02 array_content
*/
					narr = stszBuff[0x1e];
					stszOfst = 0x1f;
					while (narr-- > 0)
					{
						OSInt arrCnt;
						Int32 propType = 0;
						switch (stszBuff[(UOSInt)stszOfst] & 0x3f)
						{
						case 0x20:
							propType = *(Int32*)"vps";
							break;
						case 0x21:
							propType = *(Int32*)"sps";
							break;
						case 0x22:
							propType = *(Int32*)"pps";
							break;
						case 0x27:
							propType = *(Int32*)"sei";
							break;
						default:
							propType = 0;
							break;
						}

						arrCnt = ReadMUInt16(&stszBuff[(UOSInt)stszOfst + 1]);
						stszOfst += 3;
						while (arrCnt-- > 0)
						{
							nSample = ReadMUInt16(&stszBuff[(UOSInt)stszOfst]);
							if (propType != 0)
							{
								fsrc->SetProp(propType, &stszBuff[(UOSInt)stszOfst + 2], nSample);
							}
							stszOfst += 2 + (UInt64)nSample;
						}
					}
				}
				else if (frInfo.fourcc == *(UInt32*)"HEVC")
				{
					stszBuff.ChangeSize(avccAtomSize - 8);
					fd->GetRealData(avccOfst + 8, avccAtomSize - 8, stszBuff);
					fsrc->SetProp(*(Int32*)"HEVC", stszBuff.Arr(), avccAtomSize - 8);


/*					OSInt i;
					OSInt j;
					stszBuff = MemAlloc(UInt8, avccAtomSize - 8);
					fd->GetRealData(avccOfst + 8, avccAtomSize - 8, stszBuff);
					i = 0;
					j = 0;
					while (i < avccAtomSize - 8)
					{
						if (i <= avccAtomSize - 11 && stszBuff[i] == 0 && stszBuff[i + 1] == 0 && stszBuff[i + 2] == 3)
						{
							stszBuff[j] = 0;
							stszBuff[j + 1] = 0;
							j += 2;
							i += 3;
						}
						else
						{
							stszBuff[j] = stszBuff[i];
							i++;
							j++;
						}
					}
					fsrc->SetProp(*(Int32*)"HEVC", stszBuff, j);
					MemFree(stszBuff);*/
				}
			}

			src = fsrc;
		}
	}
	return src;
}

Bool Parser::FileParser::QTParser::ParseEdtsAtom(NN<IO::StreamData> fd, UInt64 ofst, UInt32 size, Int32 *delay, Int32 *sampleSkip)
{
	UInt32 i;
	UInt8 hdr[8];
	UInt32 atomSize;
	i = 8;
	while (i < size)
	{
		if (fd->GetRealData(ofst + i, 8, BYTEARR(hdr)) != 8)
			return 0;
		atomSize = ReadMUInt32(&hdr[0]);
		if (atomSize < 8 || i + atomSize > size)
		{
			break;
		}
		if (*(Int32*)&hdr[4] == *(Int32*)"elst")
		{
			Int32 cnt;
			Data::ByteBuffer buff(atomSize - 8);
			fd->GetRealData(ofst + i + 8, atomSize - i, buff);
			cnt = ReadMInt32(&buff[4]);
			if (cnt == 2 && ReadMInt32(&buff[12]) == -1)
			{
				*delay = ReadMInt32(&buff[8]);
				*sampleSkip = ReadMInt32(&buff[24]);
			}
		}
		else
		{
			hdr[0] = 0;
		}
		i += atomSize;
	}
	return true;
}
