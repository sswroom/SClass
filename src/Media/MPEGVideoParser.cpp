#include "Stdafx.h"
#include "Data/ByteTool.h" 
#include "IO/BitReaderMSB.h"
#include "Media/MPEGVideoParser.h"

Bool Media::MPEGVideoParser::GetFrameInfo(UInt8 *frame, UOSInt frameSize, Media::FrameInfo *frameInfo, UInt32 *fRateNorm, UInt32 *fRateDenorm, UInt64 *bitRate, Bool decoderFix)
{
	decoderFix = false;
	if (ReadMInt32(frame) != 0x1b3)
		return false;
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
//	Bool progressive_sequence = false;
	Bool mpg2 = false;

	UOSInt ofst;

	UInt32 horizontal_size = (UInt32)((frame[4] << 4) | (frame[5] >> 4));
	UInt32 vertical_size = (UInt32)(((frame[5] & 0xf) << 8) | frame[6]);
	Int32 aspect_ratio_information = frame[7] >> 4;
	Int32 frame_rate_code = frame[7] & 0xf;
	UInt32 bit_rate = (UInt32)((frame[8] << 10) | (frame[9] << 2) | (frame[10] >> 6));
	Int32 vbv_buffer_size = ((frame[10] & 0x1f) << 5) | (frame[11] >> 3);
//	Int32 constrained_parameters_flag = (frame[11] & 4) >> 2;
	Bool load_intra_quantiser_matrix = (frame[11] & 2) != 0;
	if (decoderFix && aspect_ratio_information != 1)
	{
		frame[7] = 0x10 | (frame[7] & 0xf);
	}
	if (load_intra_quantiser_matrix)
	{
		ofst = 75;
	}
	else
	{
		ofst = 11;
	}
	Bool load_non_intra_quantiser_matrix = (frame[ofst] & 1) != 0;
	if (load_non_intra_quantiser_matrix)
	{
		ofst += 65;
	}
	else
	{
		ofst++;
	}
	Bool pal = false;
	switch (frame_rate_code)
	{
	case 1:
		frameRateNorm = 24000;
		frameRateDenorm = 1001;
		break;
	case 2:
		frameRateNorm = 24;
		frameRateDenorm = 1;
		break;
	case 3:
		frameRateNorm = 25;
		frameRateDenorm = 1;
		pal = true;
		break;
	default:
	case 4:
		frameRateNorm = 30000;
		frameRateDenorm = 1001;
		break;
	case 5:
		frameRateNorm = 30;
		frameRateDenorm = 1;
		break;
	case 6:
		frameRateNorm = 50;
		frameRateDenorm = 1;
		pal = true;
		break;
	case 7:
		frameRateNorm = 60000;
		frameRateDenorm = 1001;
		break;
	case 8:
		frameRateNorm = 60;
		frameRateDenorm = 1;
		break;
	}
	//MPEG-1: Color = BT601
	frameInfo->storeWidth = horizontal_size;
	frameInfo->storeHeight = vertical_size;
	frameInfo->dispWidth = frameInfo->storeWidth;
	frameInfo->dispHeight = frameInfo->storeHeight;
	frameInfo->fourcc = *(UInt32*)"MPG2"; // 0 = bmp
	frameInfo->storeBPP = 0;
	frameInfo->pf = Media::PF_UNKNOWN;
	frameInfo->byteSize = 0;
	frameInfo->par2 = 1; // def = 1;
	frameInfo->hdpi = 96;
	frameInfo->ftype = Media::FT_NON_INTERLACE;
	frameInfo->atype = Media::AT_NO_ALPHA;
	if (pal)
	{
		frameInfo->color->SetCommonProfile(Media::ColorProfile::CPT_PAL);
	}
	else
	{
		frameInfo->color->SetCommonProfile(Media::ColorProfile::CPT_NTSC);
	}
	frameInfo->yuvType = Media::ColorProfile::YUVT_BT601;
	frameInfo->ycOfst = Media::YCOFST_C_CENTER_CENTER;

	while (ofst < frameSize)
	{
		if (ReadMInt32(&frame[ofst]) != 0x1b5)
		{
			break;
		}
		if ((frame[ofst + 4] & 0xf0) == 0x10) //Sequence extension
		{
			mpg2 = true;
			//MPEG-2: BT709
			frameInfo->color->SetCommonProfile(Media::ColorProfile::CPT_BT709);
//			frameInfo->yuvType = Media::ColorProfile::YUVT_BT709;
			frameInfo->yuvType = Media::ColorProfile::YUVT_UNKNOWN;
			frameInfo->ycOfst = Media::YCOFST_C_CENTER_LEFT;

//			Int32 profile_and_level_indication = ((frame[ofst + 4] & 0xf) << 4) | (frame[ofst + 5] >> 4);
//			progressive_sequence = (frame[ofst + 5] & 8) != 0;
//			Int32 chroma_format = (frame[ofst + 5] & 6) >> 1;
			horizontal_size = horizontal_size | ((frame[ofst + 5] & 1) << 13) | ((frame[ofst + 6] & 0x80) << 5);
			vertical_size = vertical_size | ((frame[ofst + 6] & 0x60) << 7);
			bit_rate = bit_rate | ((frame[ofst + 6] & 0x1f) << 25) | ((frame[ofst + 7] & 0xfe) << 17);
			vbv_buffer_size = vbv_buffer_size | (frame[ofst + 8] << 10);
//			Bool low_delay = (frame[ofst + 9] & 0x80) != 0;
			UInt32 frame_rate_extension_n = ((UInt32)frame[ofst + 9] & 0x60) >> 5;
			UInt32 frame_rate_extension_d = (UInt32)frame[ofst + 9] & 0x1f;
			frameRateNorm *= (frame_rate_extension_n + 1);
			frameRateDenorm *= (frame_rate_extension_d + 1);

			frameInfo->storeWidth = horizontal_size;
			frameInfo->storeHeight = vertical_size;
			frameInfo->dispWidth = frameInfo->storeWidth;
			frameInfo->dispHeight = frameInfo->storeHeight;

			ofst += 10;
		}
		else if ((frame[ofst + 4] & 0xf0) == 0x20) //Sequence Display Extension
		{
//			Int32 video_format = (frame[ofst + 4] & 0xe) >> 1;
			if ((frame[ofst + 4] & 1) != 0)
			{
				switch (frame[ofst + 7]) //matrix_coefficients
				{
				case 1:
					frameInfo->yuvType = Media::ColorProfile::YUVT_BT709;
					break;
				case 4:
					frameInfo->yuvType = Media::ColorProfile::YUVT_FCC;
					break;
				case 5:
					frameInfo->yuvType = Media::ColorProfile::YUVT_SMPTE170M;
					break;
				case 6:
					frameInfo->yuvType = Media::ColorProfile::YUVT_SMPTE170M;
					break;
				case 7:
					frameInfo->yuvType = Media::ColorProfile::YUVT_SMPTE240M;
					break;
				}
				switch (frame[ofst + 5]) //colour_primaries
				{
				default:
				case 1: //BT.709
					frameInfo->color->GetPrimaries()->SetColorType(Media::ColorProfile::CT_BT709);
					break;
				case 2:
					frameInfo->color->GetPrimaries()->SetColorType(Media::ColorProfile::CT_VUNKNOWN);
					break;
				case 4: //ITU-R BT.470-2 System M
					frameInfo->color->GetPrimaries()->SetColorType(Media::ColorProfile::CT_BT470M);
					break;
				case 5: //ITU-R BT.470-2 System B, G
					frameInfo->color->GetPrimaries()->SetColorType(Media::ColorProfile::CT_BT470BG);
					break;
				case 6: //SMPTE 170M
					frameInfo->color->GetPrimaries()->SetColorType(Media::ColorProfile::CT_SMPTE170M);
					break;
				case 7: //SMPTE 240M (1987)
					frameInfo->color->GetPrimaries()->SetColorType(Media::ColorProfile::CT_SMPTE240M);
					break;
				}
				Media::CS::TransferType tranType;
				Double tranGamma;
				switch (frame[ofst + 6]) //transfer_characteristics
				{
				default:
				case 1:
					tranType = Media::CS::TRANT_BT709;
					tranGamma = 2.2;
					break;
				case 4:
					tranType = Media::CS::TRANT_GAMMA;
					tranGamma = 2.2;
					break;
				case 5:
					tranType = Media::CS::TRANT_GAMMA;
					tranGamma = 2.8;
					break;
				case 6:
					tranType = Media::CS::TRANT_BT709;
					tranGamma = 2.2;
					break;
				case 7:
					tranType = Media::CS::TRANT_SMPTE240;
					tranGamma = 2.2;
					break;
				case 8:
					tranType = Media::CS::TRANT_LINEAR;
					tranGamma = 1.0;
					break;
				}
				frameInfo->color->GetRTranParam()->Set(tranType, tranGamma);
				frameInfo->color->GetGTranParam()->Set(tranType, tranGamma);
				frameInfo->color->GetBTranParam()->Set(tranType, tranGamma);

				frameInfo->dispWidth = (UInt32)((frame[ofst + 8] << 6) | (frame[ofst + 9] >> 2));
				frameInfo->dispHeight = (UInt32)(((frame[ofst + 9] & 1) << 13) | (frame[ofst + 10] << 5) | (frame[ofst + 11] >> 3));
				if (decoderFix && (frameInfo->dispWidth != horizontal_size || frameInfo->dispHeight != vertical_size))
				{
					frame[ofst + 8] = (UInt8)(horizontal_size >> 6);
					frame[ofst + 9] = (UInt8)(((horizontal_size << 2) | 2 | (vertical_size >> 13)) & 0xff);
					frame[ofst + 10] = (UInt8)((vertical_size >> 5) & 0xff);
					frame[ofst + 11] = (UInt8)((vertical_size << 3) & 0xff);
				}
				ofst += 12;
			}
			else
			{
				frameInfo->dispWidth = (UInt32)((frame[ofst + 5] << 6) | (frame[ofst + 6] >> 2));
				frameInfo->dispHeight = (UInt32)(((frame[ofst + 6] & 1) << 13) | (frame[ofst + 7] << 5) | (frame[ofst + 8] >> 3));
				if (decoderFix && (frameInfo->dispWidth != horizontal_size || frameInfo->dispHeight != vertical_size))
				{
					frame[ofst + 5] = (UInt8)(horizontal_size >> 6);
					frame[ofst + 6] = (UInt8)(((horizontal_size << 2) | 2 | (vertical_size >> 13)) & 0xff);
					frame[ofst + 7] = (UInt8)((vertical_size >> 5) & 0xff);
					frame[ofst + 8] = (UInt8)((vertical_size << 3) & 0xff);
				}
				ofst += 9;
			}
		}
		else
		{
			break;
		}
	}

	if (mpg2)
	{
		switch (aspect_ratio_information)
		{
		case 1:
		default:
			frameInfo->par2 = 1;
			break;
		case 2:
			frameInfo->par2 = 0.75 * horizontal_size / vertical_size;
			break;
		case 3:
			frameInfo->par2 = 0.5625 * horizontal_size / vertical_size;
			break;
		case 4:
			frameInfo->par2 = 0.45248868778280542986425339366516 * horizontal_size / vertical_size;
			break;
		}
	}
	else
	{
		switch (aspect_ratio_information)
		{
		case 1:
		default:
			frameInfo->par2 = 1;
			break;
		case 2:
			frameInfo->par2 = 0.6735;
			break;
		case 3: //16:9 625 line
			frameInfo->par2 = 0.7031;
			break;
		case 4:
			frameInfo->par2 = 0.7615;
			break;
		case 5:
			frameInfo->par2 = 0.8055;
			break;
		case 6: //16:9 525line
			frameInfo->par2 = 0.8437;
			break;
		case 7:
			frameInfo->par2 = 0.8935;
			break;
		case 8: //CCIR601 625line
			frameInfo->par2 = 0.9375;
			break;
		case 9:
			frameInfo->par2 = 0.9815;
			break;
		case 10:
			frameInfo->par2 = 1.0255;
			break;
		case 11:
			frameInfo->par2 = 1.0695;
			break;
		case 12: //CCIR601 525line
			frameInfo->par2 = 1.1250;
			break;
		case 13:
			frameInfo->par2 = 1.1575;
			break;
		case 14:
			frameInfo->par2 = 1.2015;
			break;
		}
	}
	switch (aspect_ratio_information)
	{
	case 1:
		frameInfo->par2 = 1;
		break;
	default:
		if (horizontal_size == 352 && vertical_size == 240)
		{
			frameInfo->par2 = 0.75 * horizontal_size / vertical_size;
		}
		else if (horizontal_size == 352 && vertical_size == 288)
		{
			frameInfo->par2 = 0.75 * horizontal_size / vertical_size;
		}
		else
		{
			frameInfo->par2 = 1;
		}
		break;
	case 2:
		frameInfo->par2 = 0.75 * horizontal_size / vertical_size;
		break;
	case 3:
		frameInfo->par2 = 0.5625 * horizontal_size / vertical_size;
		break;
	case 4:
		frameInfo->par2 = 0.45248868778280542986425339366516 * horizontal_size / vertical_size;
		break;
	}
	*fRateNorm = frameRateNorm;
	*fRateDenorm = frameRateDenorm;
	if (bitRate)
	{
		*bitRate = bit_rate * 400ULL;
	}
	return true;
}

Bool Media::MPEGVideoParser::GetFrameProp(const UInt8 *frame, UOSInt frameSize, MPEGFrameProp *prop)
{
	if (ReadMInt32(frame) != 0x00000100)
		return false;
	UInt32 temporal_reference;
	UInt32 picture_coding_type;
	UInt32 vbv_delay;
	UInt32 v;
	IO::BitReaderMSB reader(&frame[4], frameSize - 4);
	reader.ReadBits(&temporal_reference, 10);
	reader.ReadBits(&picture_coding_type, 3);
	reader.ReadBits(&vbv_delay, 16);
	if (picture_coding_type == 2)
	{
		reader.ReadBits(&v, 4);
	}
	else if (picture_coding_type == 3)
	{
		reader.ReadBits(&v, 8);
	}
	while (true)
	{
		if (!reader.ReadBits(&v, 1))
			break;
		if (v == 0)
			break;
		reader.ReadBits(&v, 8);
	}
	reader.ByteAlign();

	reader.ReadBits(&v, 32);
	while (v == 0 || v == 1)
	{
		if (v == 1)
		{
			reader.ReadBits(&v, 8);
			v = v | 0x100;
			break;
		}
		if (!reader.ReadBits(&v, 8))
		{
			break;
		}
	}
	if (v != 0x000001b5)
	{
		switch (picture_coding_type)
		{
		case 1:
			prop->pictureCodingType = 'I';
			break;
		case 2:
			prop->pictureCodingType = 'P';
			break;
		case 3:
			prop->pictureCodingType = 'B';
			break;
		default:
			prop->pictureCodingType = 0;
			break;
		}
		prop->dcBits = 8;
		prop->tff = true;
		prop->progressive = true;
		prop->rff = false;
		prop->pictureStruct = PS_FRAME;
		return true;
	}
	reader.ReadBits(&v, 4);
	if (v != 8)
	{
		return false;
	}
	reader.ReadBits(&v, 16);
	UInt32 intra_dc_precision;
	UInt32 picture_structure;
	UInt32 flags;
	reader.ReadBits(&intra_dc_precision, 2);
	reader.ReadBits(&picture_structure, 2);
	reader.ReadBits(&flags, 10);

	switch (picture_coding_type)
	{
	case 1:
		prop->pictureCodingType = 'I';
		break;
	case 2:
		prop->pictureCodingType = 'P';
		break;
	case 3:
		prop->pictureCodingType = 'B';
		break;
	default:
		prop->pictureCodingType = 0;
		break;
	}
	prop->dcBits = (UInt8)(intra_dc_precision + 8);
	prop->tff = (flags & 0x200) != 0;
	prop->progressive = (flags & 2) != 0;
	prop->rff = (flags & 8) != 0;
	if (picture_structure == 1)
	{
		prop->pictureStruct = PS_TOPFIELD;
	}
	else if (picture_structure == 2)
	{
		prop->pictureStruct = PS_BOTTOMFIELD;
	}
	else
	{
		prop->pictureStruct = PS_FRAME;
	}
	return true;
}
