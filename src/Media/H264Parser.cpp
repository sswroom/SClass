#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Media/H264Parser.h"
#include "Text/MyString.h"

Bool Media::H264Parser::ParseHRDParameters(IO::BitReaderMSB *reader, H264Flags *flags)
{
	UInt32 cpb_cnt_minus1;
	UInt32 bit_rate_scale;
	UInt32 cpb_size_scale;
	UInt32 initial_cpb_removal_delay_length_minus1;
	UInt32 cpb_removal_delay_length_minus1;
	UInt32 dpb_output_delay_length_minus1;
	UInt32 time_offset_length;
	UInt32 v;
	UInt32 i;
	ParseVari(reader, &cpb_cnt_minus1);
	reader->ReadBits(&bit_rate_scale, 4);
	reader->ReadBits(&cpb_size_scale, 4);
	i = 0;
	while (i <= cpb_cnt_minus1)
	{
		ParseVari(reader, &v); //bit_rate_value_minus1[ SchedSelIdx ]
		ParseVari(reader, &v); //cpb_size_value_minus1[ SchedSelIdx ]
		reader->ReadBits(&v, 1); //cbr_flag[ SchedSelIdx ]
		i++;
	}
	reader->ReadBits(&initial_cpb_removal_delay_length_minus1, 5);
	reader->ReadBits(&cpb_removal_delay_length_minus1, 5);
	reader->ReadBits(&dpb_output_delay_length_minus1, 5);
	reader->ReadBits(&time_offset_length, 5);
	if (flags)
	{
		flags->initial_cpb_removal_delay_length_minus1 = initial_cpb_removal_delay_length_minus1;
		flags->cpb_removal_delay_length_minus1 = cpb_removal_delay_length_minus1;
		flags->dpb_output_delay_length_minus1 = dpb_output_delay_length_minus1;
	}
	return true;
}

Bool Media::H264Parser::ParseVUIParameters(IO::BitReaderMSB *reader, Media::FrameInfo *info, H264Flags *flags)
{
	UInt32 aspect_ratio_info_present_flag = 0;
	UInt32 overscan_info_present_flag = 0;
	UInt32 video_signal_type_present_flag = 0;
	UInt32 chroma_loc_info_present_flag = 0;
	UInt32 timing_info_present_flag = 0;
	UInt32 nal_hrd_parameters_present_flag = 0;
	UInt32 vcl_hrd_parameters_present_flag = 0;
	UInt32 low_delay_hrd_flag = 0;
	UInt32 pic_struct_present_flag = 0;
	UInt32 bitstream_restriction_flag = 0;
	UInt32 temp;
	reader->ReadBits(&aspect_ratio_info_present_flag, 1);
	if (aspect_ratio_info_present_flag != 0)
	{
		UInt32 aspect_ratio_idc = 0;
		UInt32 sarWidth;
		UInt32 sarHeight;
		reader->ReadBits(&aspect_ratio_idc, 8);
		switch (aspect_ratio_idc)
		{
		case 0: //Unspecified
			break;
		case 1: //1:1
			info->par2 = 1;
			break;
		case 2: //12:11
			info->par2 = 0.91666666666666666666666666666667;
			break;
		case 3: //10:11
			info->par2 = 1.1;
			break;
		case 4: //16:11
			info->par2 = 0.6875;
			break;
		case 5: //40:33
			info->par2 = 0.825;
			break;
		case 6: //24:11
			info->par2 = 0.45833333333333333333333333333333;
			break;
		case 7: //20:11
			info->par2 = 0.55;
			break;
		case 8: //32:11
			info->par2 = 0.34375;
			break;
		case 9: //80:33
			info->par2 = 0.4125;
			break;
		case 10: //18:11
			info->par2 = 0.61111111111111111111111111111111;
			break;
		case 11: //15:11
			info->par2 = 0.73333333333333333333333333333333;
			break;
		case 12: //64:33
			info->par2 = 0.515625;
			break;
		case 13: //160:99
			info->par2 = 0.61875;
			break;
		case 14: //4:3
			info->par2 = 0.75;
			break;
		case 15: //3:2
			info->par2 = 0.66666666666666666666666666666667;
			break;
		case 16: //2:1
			info->par2 = 0.5;
			break;
		case 255:
			sarWidth = 0;
			sarHeight = 0;
			reader->ReadBits(&sarWidth, 16);
			reader->ReadBits(&sarHeight, 16);
			if (sarWidth != 0 && sarHeight != 0)
			{
				info->par2 = sarHeight / (Double)sarWidth;
			}
			break;
		}
	}
	reader->ReadBits(&overscan_info_present_flag, 1);
	if (overscan_info_present_flag != 0)
	{
		reader->ReadBits(&temp, 1); //overscan_appropriate_flag
	}
	reader->ReadBits(&video_signal_type_present_flag, 1);
	if (video_signal_type_present_flag != 0)
	{
		UInt32 video_format;
		UInt32 video_full_range_flag;
		UInt32 colour_description_present_flag = 0;
		reader->ReadBits(&video_format, 3);
		reader->ReadBits(&video_full_range_flag, 1);
		reader->ReadBits(&colour_description_present_flag, 1);
		if (colour_description_present_flag != 0)
		{
			UInt32 colour_primaries = 0;
			UInt32 transfer_characteristics = 0;
			UInt32 matrix_coefficients = 0;
			reader->ReadBits(&colour_primaries, 8);
			reader->ReadBits(&transfer_characteristics, 8);
			reader->ReadBits(&matrix_coefficients, 8);
			switch (matrix_coefficients)
			{
			case 0: //GBR
			case 2: //Unspecified
			case 8: //YCgCo
				break;
			case 1:
				info->yuvType = Media::ColorProfile::YUVT_BT709;
				break;
			case 4:
				info->yuvType = Media::ColorProfile::YUVT_FCC;
				break;
			case 5:
				info->yuvType = Media::ColorProfile::YUVT_BT601;
				break;
			case 6:
				info->yuvType = Media::ColorProfile::YUVT_SMPTE170M;
				break;
			case 7:
				info->yuvType = Media::ColorProfile::YUVT_SMPTE240M;
				break;
			}
			if (video_full_range_flag)
			{
				info->yuvType = (Media::ColorProfile::YUVType)(info->yuvType | Media::ColorProfile::YUVT_FLAG_YUV_0_255);
			}
			switch (colour_primaries)
			{
			default:
			case 2:
				info->color->GetPrimaries()->SetColorType(Media::ColorProfile::CT_VUNKNOWN);
				break;
			case 1:
				info->color->GetPrimaries()->SetColorType(Media::ColorProfile::CT_SRGB);
				break;
			case 4:
				info->color->GetPrimaries()->SetColorType(Media::ColorProfile::CT_BT470M);
				break;
			case 5:
				info->color->GetPrimaries()->SetColorType(Media::ColorProfile::CT_BT470BG);
				break;
			case 6:
				info->color->GetPrimaries()->SetColorType(Media::ColorProfile::CT_SMPTE170M);
				break;
			case 7:
				info->color->GetPrimaries()->SetColorType(Media::ColorProfile::CT_SMPTE240M);
				break;
			case 8:
				info->color->GetPrimaries()->SetColorType(Media::ColorProfile::CT_GENERIC_FILM);
				break;
			}
			Bool skip = false;
			Media::CS::TransferType tranType = Media::CS::TRANT_VUNKNOWN;
			Double tranGamma = 1.0;
			switch (transfer_characteristics)
			{
			case 1: //BT709
			case 6: //BT601
				tranType = Media::CS::TRANT_BT709;
				tranGamma = 2.2;
				break;
			case 2: //Unspecified
				skip = true;
				break;
			case 4: // Gamma 2.2 (BT470M)
				tranType = Media::CS::TRANT_GAMMA;
				tranGamma = 2.2;
				break;
			case 5: // Gamma 2.8 (BT470BG)
				tranType = Media::CS::TRANT_GAMMA;
				tranGamma = 2.8;
				break;
			case 7: // SMPTE240M
				tranType = Media::CS::TRANT_SMPTE240;
				tranGamma = 2.2;
				break;
			case 8: // Linear
				tranType = Media::CS::TRANT_LINEAR;
				tranGamma = 1.0;
				break;
			case 9: //Log100
				tranType = Media::CS::TRANT_LOG100;
				tranGamma = 2.2;
				break;
			case 10: //LogSqrt10
				tranType = Media::CS::TRANT_LOGSQRT10;
				tranGamma = 2.2;
				break;
			case 11: //IEC 61966-2-4
				tranType = Media::CS::TRANT_BT709;
				tranGamma = 2.2;
				break;
			case 12: //BT.1361
				tranType = Media::CS::TRANT_BT1361;
				tranGamma = 2.2;
				break;
			}
			if (!skip)
			{
				info->color->GetRTranParam()->Set(tranType, tranGamma);
				info->color->GetGTranParam()->Set(tranType, tranGamma);
				info->color->GetBTranParam()->Set(tranType, tranGamma);
			}
		}
	}
	reader->ReadBits(&chroma_loc_info_present_flag, 1);
	if (chroma_loc_info_present_flag != 0)
	{
		UInt32 chroma_sample_loc_type_top_field;
		UInt32 chroma_sample_loc_type_bottom_field;
		ParseVari(reader, &chroma_sample_loc_type_top_field);
		ParseVari(reader, &chroma_sample_loc_type_bottom_field);
	}
	reader->ReadBits(&timing_info_present_flag, 1);
	if (timing_info_present_flag != 0)
	{
		if (flags)
		{
			reader->ReadBits(&flags->frameRateDenorm, 32); //num_units_in_tick
			reader->ReadBits(&flags->frameRateNorm, 32); //time_scale
		}
		else
		{
			reader->ReadBits(&temp, 32); //num_units_in_tick
			reader->ReadBits(&temp, 32); //time_scale
		}
		reader->ReadBits(&temp, 1); //fixed_frame_rate_flag
	}
	reader->ReadBits(&nal_hrd_parameters_present_flag, 1);
	if (nal_hrd_parameters_present_flag != 0)
	{
		ParseHRDParameters(reader, flags);
	}
	reader->ReadBits(&vcl_hrd_parameters_present_flag, 1);
	if (vcl_hrd_parameters_present_flag != 0)
	{
		ParseHRDParameters(reader, flags);
	}
	if (nal_hrd_parameters_present_flag != 0 || vcl_hrd_parameters_present_flag != 0)
	{
		reader->ReadBits(&low_delay_hrd_flag, 1);
	}
	reader->ReadBits(&pic_struct_present_flag, 1);
	reader->ReadBits(&bitstream_restriction_flag, 1);

	if (flags)
	{
		flags->pic_struct_present_flag = pic_struct_present_flag != 0;
		flags->nal_hrd_parameters_present_flag = nal_hrd_parameters_present_flag != 0;
		flags->vcl_hrd_parameters_present_flag = vcl_hrd_parameters_present_flag != 0;
	}
	info->ycOfst = Media::YCOFST_C_CENTER_LEFT;
	info->rotateType = Media::RotateType::None;
	return true;
}

Bool Media::H264Parser::GetFrameInfo(const UInt8 *frame, UOSInt frameSize, Media::FrameInfo *frameInfo, H264Flags *flags) //Bool *frameOnly, Bool *mbaff, Bool *separateColourPlane, Int32 *maxFrameNum_4, 
{
	IO::BitReaderMSB *reader;
	Bool succ = false;
	UInt8 *tmpBuff;
	UInt8 *tmpPtr;
	UInt32 profile_idc;
	UInt32 level_idc;
	UInt32 pic_order_cnt_type;
	UInt32 seq_parameter_set_id;
	UInt32 temp;
	Int32 stemp;
	UInt32 pic_width_in_mbs_minus1;
	UInt32 pic_height_in_map_units_minus1;
	UInt32 frame_mbs_only_flag;
	UInt32 mb_adaptive_frame_field_flag;
	UOSInt i;
	UOSInt j;
	OSInt k;

	if (ReadMInt32(frame) != 1)
		return false;
	if ((frame[4] & 0x1f) != 7)
		return false;

	tmpBuff = MemAlloc(UInt8, frameSize);
	tmpPtr = tmpBuff;
	i = 0;
	j = frameSize;
	while (i < j)
	{
		if (frame[i] == 0)
		{
			if (i + 3 < j && frame[i + 1] == 0 && frame[i + 2] == 3)
			{
				*tmpPtr++ = 0;
				*tmpPtr++ = 0;
				frameSize--;
				i += 2;
			}
			else
			{
				*tmpPtr++ = frame[i];
			}
		}
		else
		{
			*tmpPtr++ = frame[i];
		}
		i++;
	}


	NEW_CLASS(reader, IO::BitReaderMSB(&tmpBuff[5], frameSize - 5));
	reader->ReadBits(&profile_idc, 8);
	reader->ReadBits(&temp, 8);
	reader->ReadBits(&level_idc, 8);
	ParseVari(reader, &seq_parameter_set_id);
	UInt32 separate_colour_plane_flag = 0;

	if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 244 || profile_idc == 44 || profile_idc == 83 || profile_idc == 86 || profile_idc == 118 || profile_idc == 128)
	{
		UInt32 chroma_format_idc = 0;
		UInt32 seq_scaling_matrix_present_flag = 0;
		j = 8;

		ParseVari(reader, &chroma_format_idc);
		if (chroma_format_idc == 3)
		{
			reader->ReadBits(&separate_colour_plane_flag, 1);
			j = 12;
		}
		ParseVari(reader, &temp); //bit_depth_luma_minus8
		ParseVari(reader, &temp); //bit_depth_chroma_minus8
		reader->ReadBits(&temp, 1); //qpprime_y_zero_transform_bypass_flag
		reader->ReadBits(&seq_scaling_matrix_present_flag, 1);
		if (seq_scaling_matrix_present_flag == 1)
		{
			i = 0;
			while (i < j)
			{
				temp = 0;
				reader->ReadBits(&temp, 1); //seq_scaling_list_present_flag[i]
				if (temp == 1)
				{
					if (i < 6)
					{
						//scaling_list( ScalingList4x4[ i ], 16, UseDefaultScalingMatrix4x4Flag[ i ]);
						UInt32 lastScale = 8;
						UInt32 nextScale = 8;
						k = 16;
						while (k-- > 0)
						{
							if (nextScale != 0 && ParseSVari(reader, &stemp))
							{
								nextScale = (lastScale + (UInt32)stemp + 256) & 255;
							}
							if (nextScale)
							{
								lastScale = nextScale;
							}
						}
					}
					else
					{
						//scaling_list( ScalingList8x8[ i − 6 ], 64, UseDefaultScalingMatrix8x8Flag[ i − 6 ]);
						UInt32 lastScale = 8;
						UInt32 nextScale = 8;
						k = 64;
						while (k-- > 0)
						{
							if (nextScale != 0 && ParseSVari(reader, &stemp))
							{
								nextScale = (lastScale + (UInt32)stemp + 256) & 255;
							}
							if (nextScale)
							{
								lastScale = nextScale;
							}
						}
					}
				}
				i++;
			}
		}
	}
	ParseVari(reader, &temp); //log2_max_frame_num_minus4
	if (flags)
	{
		flags->separateColourPlane = separate_colour_plane_flag != 0;
		flags->maxFrameNum_4 = temp;
	}
	ParseVari(reader, &pic_order_cnt_type);
	if (pic_order_cnt_type == 0)
	{
		ParseVari(reader, &temp); //log2_max_pic_order_cnt_lsb_minus4
	}
	else if (pic_order_cnt_type == 1)
	{
		reader->ReadBits(&temp, 1); //delta_pic_order_always_zero_flag
		ParseSVari(reader, &stemp); //offset_for_non_ref_pic
		ParseSVari(reader, &stemp); //offset_for_top_to_bottom_field
		ParseVari(reader, &temp); //num_ref_frames_in_pic_order_cnt_cycle
		j = temp;
		i = 0;
		while (i < j)
		{
			ParseSVari(reader, &stemp); //offset_for_ref_frame[i]
			i++;
		}
	}
	ParseVari(reader, &temp); //max_num_ref_frames
	reader->ReadBits(&temp, 1); //gaps_in_frame_num_value_allowed_flag
	if (ParseVari(reader, &pic_width_in_mbs_minus1))
	{
		frameInfo->storeSize.x = ((UOSInt)pic_width_in_mbs_minus1 + 1) << 4;
	}
	ParseVari(reader, &pic_height_in_map_units_minus1);
	if (reader->ReadBits(&frame_mbs_only_flag, 1))
	{
		frameInfo->storeSize.y = (2 - frame_mbs_only_flag) * ((UOSInt)pic_height_in_map_units_minus1 + 1) << 4;
	}
	mb_adaptive_frame_field_flag = 0;
	if (frame_mbs_only_flag == 0)
	{
		reader->ReadBits(&mb_adaptive_frame_field_flag, 1);
	}
	reader->ReadBits(&temp, 1); //direct_8x8_inference_flag
	reader->ReadBits(&temp, 1); //frame_cropping_flag
	frameInfo->dispSize = frameInfo->storeSize;
	if (temp)
	{
		if (ParseVari(reader, &temp))
			frameInfo->dispSize.x -= temp;
		if (ParseVari(reader, &temp))
			frameInfo->dispSize.x -= temp;
		if (ParseVari(reader, &temp))
			frameInfo->dispSize.y -= temp;
		if (ParseVari(reader, &temp))
			frameInfo->dispSize.y -= temp;
	}

	if (flags)
	{
		flags->frameOnly = frame_mbs_only_flag != 0;
		flags->mbaff = mb_adaptive_frame_field_flag != 0;
		flags->pic_struct_present_flag = false;
		flags->nal_hrd_parameters_present_flag = false;
		flags->vcl_hrd_parameters_present_flag = false;
	}

	reader->ReadBits(&temp, 1);
	if (temp)
	{
		ParseVUIParameters(reader, frameInfo, flags);
	}
	
	DEL_CLASS(reader);
	MemFree(tmpBuff);
	return succ;
}

Bool Media::H264Parser::ParseVari(IO::BitReaderMSB *reader, UInt32 *val)
{
	UInt32 v;
	UInt32 bitCnt = 0;
	while (true)
	{
		if (!reader->ReadBits(&v, 1))
			return false;
		if (v == 1)
			break;
		bitCnt++;
	}
	if (bitCnt)
	{
		if (!reader->ReadBits(&v, bitCnt))
		{
			return false;
		}
	}
	else
	{
		v = 0;
	}
	*val = (UInt32)(1 << bitCnt) - 1 + v;
	return true;
}

Bool Media::H264Parser::ParseSVari(IO::BitReaderMSB *reader, Int32 *val)
{
	UInt32 v;
	Bool ret = ParseVari(reader, &v);
	if (ret)
	{
		if (v & 1)
		{
			*val = (Int32)(v >> 1);
		}
		else
		{
			*val = -(Int32)(v >> 1);
		}
	}
	return ret;
}

Bool Media::H264Parser::FindSPS(const UInt8 *frame, UOSInt frameSize, const UInt8 **sps, UOSInt *spsSize)
{
	if (frame[0] != 0 || frame[1] != 0 || frame[2] != 0 || frame[3] != 1)
		return false;
	UOSInt i;
	Data::ArrayListUInt32 nalList;
	UInt8 t;
	FindNALs(frame, frameSize, &nalList);

	UOSInt startOfst;
	UOSInt endOfst = frameSize;
	i = nalList.GetCount();
	while (i-- > 0)
	{
		startOfst = nalList.GetItem(i);
		t = frame[startOfst + 4] & 0x1f;
		if (t == 7)
		{
			*sps = &frame[startOfst + 4];
			*spsSize = endOfst - startOfst - 4;
			return true;
		}
		endOfst = startOfst;
	}
	return false;
}


Bool Media::H264Parser::FindPPS(const UInt8 *frame, UOSInt frameSize, const UInt8 **pps, UOSInt *ppsSize)
{
	if (frame[0] != 0 || frame[1] != 0 || frame[2] != 0 || frame[3] != 1)
		return false;
	UOSInt i;
	Data::ArrayListUInt32 nalList;
	FindNALs(frame, frameSize, &nalList);

	UOSInt startOfst;
	UOSInt endOfst = frameSize;
	i = nalList.GetCount();
	while (i-- > 0)
	{
		startOfst = nalList.GetItem(i);
		if ((frame[startOfst + 4] & 0x1f) == 8)
		{
			*pps = &frame[startOfst + 4];
			*ppsSize = endOfst - startOfst - 4;
			return true;
		}
		endOfst = startOfst;
	}
	return false;
}

Bool Media::H264Parser::FindNALs(const UInt8 *frame, UOSInt frameSize, Data::ArrayListUInt32 *nalList)
{
	if (frame[0] != 0 || frame[1] != 0 || frame[2] != 0 || frame[3] != 1)
		return false;
	UOSInt i;
	UOSInt j = frameSize - 4;
	UInt8 t;
	i = 0;
	while (i <= j)
	{
		if (frame[i] != 0 || frame[i + 1] != 0 || frame[i + 2] != 0 || frame[i + 3] != 1)
		{
		}
		else
		{
			nalList->Add((UInt32)i);
			t = frame[i + 4] & 0x1f;
			if (t == 1 || t == 5)
				break;
		}
		i++;
	}
	return true;
}

UTF8Char *Media::H264Parser::GetFrameType(UTF8Char *sbuff, const UInt8 *frame, UOSInt frameSize)
{
	if (frame[0] != 0 || frame[1] != 0 || frame[2] != 0 || frame[3] != 1)
		return 0;

	UOSInt i;
	Data::ArrayListUInt32 nalList;
	FindNALs(frame, frameSize, &nalList);

	UOSInt startOfst;
	UOSInt endOfst = frameSize;
	i = nalList.GetCount();
	while (i-- > 0)
	{
		startOfst = nalList.GetItem(i);
		if ((frame[startOfst + 4] & 0x1f) == 1 || (frame[startOfst + 4] & 0x1f) == 5)
		{
			IO::BitReaderMSB *reader;
			UInt32 v;
			NEW_CLASS(reader, IO::BitReaderMSB(&frame[startOfst + 5], endOfst - startOfst - 5));
			ParseVari(reader, &v);
			v = (UInt32)-1;
			ParseVari(reader, &v);
			DEL_CLASS(reader);
			switch (v)
			{
			case 0:
				return Text::StrConcatC(sbuff, UTF8STRC("P"));
			case 1:
				return Text::StrConcatC(sbuff, UTF8STRC("B"));
			case 2:
				return Text::StrConcatC(sbuff, UTF8STRC("I"));
			case 3:
				return Text::StrConcatC(sbuff, UTF8STRC("SP"));
			case 4:
				return Text::StrConcatC(sbuff, UTF8STRC("SI"));
			case 5:
				return Text::StrConcatC(sbuff, UTF8STRC("P"));
			case 6:
				return Text::StrConcatC(sbuff, UTF8STRC("B"));
			case 7:
				return Text::StrConcatC(sbuff, UTF8STRC("I"));
			case 8:
				return Text::StrConcatC(sbuff, UTF8STRC("SP"));
			case 9:
				return Text::StrConcatC(sbuff, UTF8STRC("SI"));
			default:
				return 0;
			}
		}
		endOfst = startOfst;
	}
	return 0;
}
