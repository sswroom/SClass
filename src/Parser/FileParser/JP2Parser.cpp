#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/JP2Parser.h"

#include <openjpeg.h>

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif
Parser::FileParser::JP2Parser::JP2Parser()
{
}

Parser::FileParser::JP2Parser::~JP2Parser()
{
}

Int32 Parser::FileParser::JP2Parser::GetName()
{
	return *(Int32*)"JP2P";
}

void Parser::FileParser::JP2Parser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.JP2"), CSTR("JPEG 2000 File"));
	}
}

IO::ParserType Parser::FileParser::JP2Parser::GetParserType()
{
	return IO::ParserType::ImageList;
}

struct JP2Session
{
	NN<IO::StreamData> fd;
	UInt64 currOfst;
};

OPJ_SIZE_T JP2Parser_Read(void * p_buffer, OPJ_SIZE_T p_nb_bytes, void * p_user_data)
{
	JP2Session *sess = (JP2Session*)p_user_data;
	UOSInt readSize = sess->fd->GetRealData(sess->currOfst, (UOSInt)p_nb_bytes, Data::ByteArray((UInt8*)p_buffer, p_nb_bytes));
	sess->currOfst += readSize;
	if (readSize == 0 && p_nb_bytes != 0)
	{
		return (OPJ_SIZE_T)-1;
	}
	return (OPJ_SIZE_T)readSize;
}

IO::ParsedObject *Parser::FileParser::JP2Parser::ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (ReadMUInt32(hdr) != 12 || *(Int32*)&hdr[4] != *(Int32*)"jP  " || ReadMUInt32(&hdr[8]) != 0x0d0a870a)
	{
		return 0;
	}
	JP2Session sess;
	sess.fd = fd;
	sess.currOfst = 0;

	opj_dparameters_t param;
	opj_set_default_decoder_parameters(&param);
	param.decod_format = 1;
	param.cp_layer = 0;
	param.cp_reduce = 0;
	opj_stream_t *stm = opj_stream_default_create(true);
	opj_stream_set_read_function(stm, JP2Parser_Read);
	opj_stream_set_user_data(stm, &sess, 0);
	opj_stream_set_user_data_length(stm, fd->GetDataSize());
	opj_codec_t *codec = opj_create_decompress(OPJ_CODEC_JP2);

#if defined(VERBOSE)
	printf("JP2 1\r\n");
#endif
	if (!opj_setup_decoder(codec, &param))
	{
#if defined(VERBOSE)
		printf("JP2 Error in setup decoder\r\n");
#endif
		opj_stream_destroy(stm);
		opj_destroy_codec(codec);
		return 0;
	}

	opj_image_t *imgInfo;
	if (!opj_read_header(stm, codec, &imgInfo))
	{
#if defined(VERBOSE)
		printf("JP2 Error in read header\r\n");
#endif
		opj_stream_destroy(stm);
		opj_destroy_codec(codec);
		return 0;
	}
	Bool succ = false;
	Media::ImageList *imgList = 0;
#if 1
	Media::StaticImage *img;
	UOSInt w = (UInt32)(imgInfo->x1 - imgInfo->x0);
	UOSInt h = (UInt32)(imgInfo->y1 - imgInfo->y0);
	UInt32 dataSize = (UInt32)(w * h * 3);
	NEW_CLASS(img, Media::StaticImage(Math::Size2D<UOSInt>(w, h), 0, 24, Media::PF_B8G8R8, dataSize, Media::ColorProfile(), Media::ColorProfile::YUVT_BT709, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	UInt8 *dataBuff = 0;
	UOSInt dataBuffSize = 0;
	while (true)
	{
		UInt32 tileIndex;
		Int32 x0;
		Int32 y0;
		Int32 x1;
		Int32 y1;
		UInt32 nbComps;
		OPJ_BOOL shouldGoOn;

		if (!opj_read_tile_header(codec, stm, &tileIndex, &dataSize, &x0, &y0, &x1, &y1, &nbComps, &shouldGoOn))
		{
#if defined(VERBOSE)
			printf("JP2 Error in reading tile header\r\n");
#endif
			break;
		}
		if (!shouldGoOn)
		{
			succ = true;
#if defined(VERBOSE)
			printf("JP2 End of file\r\n");
#endif
			if (!opj_end_decompress(codec, stm))
			{
#if defined(VERBOSE)
				printf("JP2 Error in ending decompress\r\n");
#endif
				succ = false;
			}
			break;
		}
		if (dataSize > dataBuffSize)
		{
			if (dataBuff)
			{
				MemFree(dataBuff);
			}
			dataBuffSize = dataSize;
			dataBuff = MemAlloc(UInt8, dataBuffSize);
		}
#if defined(VERBOSE)
		printf("JP2 Tile: %d, Sz %d, (%d, %d), (%d, %d)\r\n", tileIndex, dataSize, x0, y0, x1, y1);
#endif
		if (!opj_decode_tile_data(codec, tileIndex, dataBuff, dataSize, stm))
		{
#if defined(VERBOSE)
			printf("JP2 Error in decoding tile\r\n");
#endif
			break;
		}
		UOSInt tileW = (UInt32)(x1 - x0);
		UOSInt tileH = (UInt32)(y1 - y0);
		UInt8 *dptr = &img->data[y0 * (OSInt)img->GetDataBpl() + x0 * 3];
		UInt8 *rptr = dataBuff;
		UInt8 *gptr = rptr + tileW * tileH;
		UInt8 *bptr = gptr + tileW * tileH;
		UOSInt i;
		UOSInt dAdd = img->GetDataBpl() - tileW * 3;
		while (tileH-- > 0)
		{
			i = tileW;
			while (i-- > 0)
			{
				dptr[0] = bptr[0];
				dptr[1] = gptr[0];
				dptr[2] = rptr[0];
				dptr += 3;
				rptr++;
				gptr++;
				bptr++;
			}
			dptr += dAdd;
		}
	}
	if (dataBuff)
	{
		MemFree(dataBuff);
	}
	if (succ)
	{
		NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
		imgList->AddImage(img, 0);
	}
	else
	{
		DEL_CLASS(img);
	}
#else
	if (!opj_decode(codec, stm, imgInfo))
	{
		succ = false;
#if defined(VERBOSE)
		printf("JP2 Error in decoding\r\n");
#endif
	}
	else
	{
		succ = true;
		if (!opj_end_decompress(codec, stm))
		{
#if defined(VERBOSE)
			printf("JP2 Error in ending decompress\r\n");
#endif
			succ = false;
		}
	}
#if defined(VERBOSE)
	printf("JP2 2\r\n");
#endif
	if (succ)
	{
#if defined(VERBOSE)
		printf("JP2 decode succeed\r\n");
		printf("Color space = %d, (%d, %d), (%d, %d), ncomp = %d, icc_len = %d\r\n", imgInfo->color_space, imgInfo->x0, imgInfo->y0, imgInfo->x1, imgInfo->y1, imgInfo->numcomps, imgInfo->icc_profile_len);
#endif
		Media::StaticImage *img;
		UOSInt w = (UInt32)(imgInfo->x1 - imgInfo->x0);
		UOSInt h = (UInt32)(imgInfo->y1 - imgInfo->y0);
		if (imgInfo->color_space == OPJ_CLRSPC_SRGB && imgInfo->numcomps == 3)
		{
			UOSInt dataSize = (w * h * 3);
			Int32 *rptr = imgInfo->comps[0].data;
			Int32 *gptr = imgInfo->comps[1].data;	
			Int32 *bptr = imgInfo->comps[2].data;	
			NEW_CLASS(img, Media::StaticImage(w, h, 0, 24, Media::PF_B8G8R8, dataSize, 0, Media::ColorProfile::YUVT_BT709, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			UInt8 *dptr = img->data;
			UOSInt i;
			while (h-- > 0)
			{
				i = w;
				while (i-- > 0)
				{
					dptr[0] = (UInt8)bptr[0];
					dptr[1] = (UInt8)gptr[0];
					dptr[2] = (UInt8)rptr[0];
					dptr += 3;
					rptr++;
					gptr++;
					bptr++;
				}
			}
			NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
			imgList->AddImage(img, 0);
		}
	}
#endif
	opj_image_destroy(imgInfo);
	opj_stream_destroy(stm);
	opj_destroy_codec(codec);

	return imgList;
}
