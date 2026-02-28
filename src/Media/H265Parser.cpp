#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Media/H264Parser.h"
#include "Media/H265Parser.h"
#include "Text/MyString.h"

Bool Media::H265Parser::GetFrameInfoSPS(const UInt8 *sps, UIntOS spsSize, NN<Media::FrameInfo> frameInfo)
{
	UInt8 *tmpBuff;
	UIntOS tmpSize;
	UIntOS i;
	UIntOS j;
	UInt32 tmp;
	UInt32 sps_max_sub_layers_minus1;
	UInt32 chroma_format_idc;
	UInt32 num_short_term_ref_pic_sets;

	if ((sps[0] & 0x7e) != 0x42)
		return false;

	tmpBuff = MemAlloc(UInt8, spsSize);
	tmpSize = 0;
	i = 2;
	while (i < spsSize)
	{
		if (sps[i - 2] == 0 && sps[i - 1] == 0 && sps[i] == 3)
		{
			tmpBuff[tmpSize] = sps[i - 2];
			tmpBuff[tmpSize + 1] = sps[i - 1];
			i += 3;
			tmpSize += 2;
		}
		else
		{
			tmpBuff[tmpSize] = sps[i - 2];
			i++;
			tmpSize++;
		}
	}
	i -= 2;
	while (i < spsSize)
	{
		tmpBuff[tmpSize++] = sps[i++];
	}
	{
		IO::BitReaderMSB reader(&tmpBuff[2], tmpSize - 2);
		reader.ReadBits(tmp, 4); //sps_video_parameter_set_id
		reader.ReadBits(sps_max_sub_layers_minus1, 3); //sps_max_sub_layers_minus1
		reader.ReadBits(tmp, 1); //sps_video_parameter_set_id
		{	//profile_tier_level( maxNumSubLayersMinus1 )
			reader.ReadBits(tmp, 2); //general_profile_space
			reader.ReadBits(tmp, 1); //general_tier_flag
			reader.ReadBits(tmp, 5); //general_profile_idc
			reader.ReadBits(tmp, 32); //general_profile_compatibility_flag
			reader.ReadBits(tmp, 1); //general_progressive_source_flag
			reader.ReadBits(tmp, 1); //general_interlaced_source_flag
			reader.ReadBits(tmp, 1); //general_non_packed_constraint_flag
			reader.ReadBits(tmp, 1); //general_frame_only_constraint_flag
			reader.ReadBits(tmp, 12); //general_reserved_zero_44bits
			reader.ReadBits(tmp, 32); //general_reserved_zero_44bits
			reader.ReadBits(tmp, 8); //general_level_idc
			if (sps_max_sub_layers_minus1 > 0)
			{
				UInt32 subLyrFlags[8];
				i = 0;
				while (i < 8)
				{
					reader.ReadBits(subLyrFlags[i], 2); //sub_layer_profile_present_flag, sub_layer_level_present_flag
					i++;
				}
				i = 0;
				while (i < sps_max_sub_layers_minus1)
				{
					if (subLyrFlags[i] & 2)
					{
						reader.ReadBits(tmp, 2); //sub_layer_profile_space
						reader.ReadBits(tmp, 1); //sub_layer_tier_flag
						reader.ReadBits(tmp, 5); //sub_layer_profile_idc
						reader.ReadBits(tmp, 32); //sub_layer_profile_compatibility_flag
						reader.ReadBits(tmp, 1); //sub_layer_progressive_source_flag
						reader.ReadBits(tmp, 1); //sub_layer_interlaced_source_flag
						reader.ReadBits(tmp, 1); //sub_layer_non_packed_constraint_flag
						reader.ReadBits(tmp, 1); //sub_layer_frame_only_constraint_flag
						reader.ReadBits(tmp, 12); //sub_layer_reserved_zero_44bits
						reader.ReadBits(tmp, 32); //sub_layer_reserved_zero_44bits
					}
					if (subLyrFlags[i] & 1)
					{
						reader.ReadBits(tmp, 8); //sub_layer_level_idc
					}
					i++;
				}
			}
		}
		Media::H264Parser::ParseVari(reader, tmp); //sps_seq_parameter_set_id
		Media::H264Parser::ParseVari(reader, chroma_format_idc); //chroma_format_idc
		if (chroma_format_idc == 3)
		{
			reader.ReadBits(tmp, 1); //separate_colour_plane_flag
		}
		UInt32 pic_width_in_luma_samples;
		UInt32 pic_height_in_luma_samples;
		Media::H264Parser::ParseVari(reader, pic_width_in_luma_samples); //pic_width_in_luma_samples
		Media::H264Parser::ParseVari(reader, pic_height_in_luma_samples); //pic_height_in_luma_samples
		if (pic_width_in_luma_samples != 0 && pic_height_in_luma_samples != 0)
		{
			frameInfo->storeSize.x = pic_width_in_luma_samples;
			frameInfo->storeSize.y = pic_height_in_luma_samples;
			frameInfo->dispSize = frameInfo->storeSize;
		}
		reader.ReadBits(tmp, 1); //conformance_window_flag
		if (tmp)
		{
			Media::H264Parser::ParseVari(reader, tmp); //conf_win_left_offset
			Media::H264Parser::ParseVari(reader, tmp); //conf_win_right_offset
			Media::H264Parser::ParseVari(reader, tmp); //conf_win_top_offset
			Media::H264Parser::ParseVari(reader, tmp); //conf_win_bottom_offset
		}
		Media::H264Parser::ParseVari(reader, tmp); //bit_depth_luma_minus8
		Media::H264Parser::ParseVari(reader, tmp); //bit_depth_chroma_minus8
		Media::H264Parser::ParseVari(reader, tmp); //log2_max_pic_order_cnt_lsb_minus4
		reader.ReadBits(tmp, 1); //sps_sub_layer_ordering_info_present_flag
		if (tmp)
		{
			i = 0;
			while (i <= sps_max_sub_layers_minus1)
			{
				Media::H264Parser::ParseVari(reader, tmp); //sps_max_dec_pic_buffering_minus1
				Media::H264Parser::ParseVari(reader, tmp); //sps_max_num_reorder_pics
				Media::H264Parser::ParseVari(reader, tmp); //sps_max_latency_increase_plus1
				i++;
			}
		}
		else
		{
			Media::H264Parser::ParseVari(reader, tmp); //sps_max_dec_pic_buffering_minus1
			Media::H264Parser::ParseVari(reader, tmp); //sps_max_num_reorder_pics
			Media::H264Parser::ParseVari(reader, tmp); //sps_max_latency_increase_plus1
		}
		Media::H264Parser::ParseVari(reader, tmp); //log2_min_luma_coding_block_size_minus3
		Media::H264Parser::ParseVari(reader, tmp); //log2_diff_max_min_luma_coding_block_size
		Media::H264Parser::ParseVari(reader, tmp); //log2_min_transform_block_size_minus2
		Media::H264Parser::ParseVari(reader, tmp); //log2_diff_max_min_transform_block_size
		Media::H264Parser::ParseVari(reader, tmp); //max_transform_hierarchy_depth_inter
		Media::H264Parser::ParseVari(reader, tmp); //max_transform_hierarchy_depth_intra
		reader.ReadBits(tmp, 1); //scaling_list_enabled_flag
		if (tmp)
		{
			reader.ReadBits(tmp, 1); //sps_scaling_list_data_present_flag
			if (tmp) //scaling_list_data()
			{
				IntOS coefNum;
				i = 3;
				while (i-- > 0)
				{
					j = 6;
					while (j-- > 0)
					{
						reader.ReadBits(tmp, 1); //scaling_list_pred_mode_flag
						if (tmp)
						{
							coefNum = ((IntOS)1) << (4 + ((2 - (Int32)i) << 1));
							if (coefNum > 64)
								coefNum = 64;
							if (i == 0)
							{
								Media::H264Parser::ParseVari(reader, tmp); //scaling_list_dc_coef_minus8
							}
							while (coefNum-- > 0)
							{
								Media::H264Parser::ParseVari(reader, tmp); //scaling_list_delta_coef
							}
						}
						else
						{
							Media::H264Parser::ParseVari(reader, tmp); //scaling_list_pred_matrix_id_delta
						}
					}
				}

				j = 2;
				while (j-- > 0)
				{
					reader.ReadBits(tmp, 1); //scaling_list_pred_mode_flag
					if (tmp)
					{
						coefNum = 64;
						Media::H264Parser::ParseVari(reader, tmp); //scaling_list_dc_coef_minus8
						while (coefNum-- > 0)
						{
							Media::H264Parser::ParseVari(reader, tmp); //scaling_list_delta_coef
						}
					}
					else
					{
						Media::H264Parser::ParseVari(reader, tmp); //scaling_list_pred_matrix_id_delta
					}
				}
			}
		}
		reader.ReadBits(tmp, 1); //amp_enabled_flag
		reader.ReadBits(tmp, 1); //sample_adaptive_offset_enabled_flag
		reader.ReadBits(tmp, 1); //pcm_enabled_flag
		if (tmp)
		{
			reader.ReadBits(tmp, 4); //pcm_sample_bit_depth_luma_minus1
			reader.ReadBits(tmp, 4); //pcm_sample_bit_depth_chroma_minus1
			Media::H264Parser::ParseVari(reader, tmp); //log2_min_pcm_luma_coding_block_size_minus3
			Media::H264Parser::ParseVari(reader, tmp); //log2_diff_max_min_pcm_luma_coding_block_size
			reader.ReadBits(tmp, 1); //pcm_loop_filter_disabled_flag
		}
		Media::H264Parser::ParseVari(reader, num_short_term_ref_pic_sets); //num_short_term_ref_pic_sets
		if (num_short_term_ref_pic_sets > 0)
		{
			i = 0;
			while (i < num_short_term_ref_pic_sets) //short_term_ref_pic_set(i)
			{

				/////////////////////////////////
				i++;
			}
		}
		reader.ReadBits(tmp, 1); //long_term_ref_pics_present_flag
		if (tmp)
		{
			i = 0;
			/////////////////////////////////
		}
		reader.ReadBits(tmp, 1); //sps_temporal_mvp_enabled_flag
		reader.ReadBits(tmp, 1); //strong_intra_smoothing_enabled_flag
		reader.ReadBits(tmp, 1); //vui_parameters_present_flag
		if (tmp)
		{
			reader.ReadBits(tmp, 1); //aspect_ratio_info_present_flag
			if (tmp)
			{
				UInt32 sarWidth;
				UInt32 sarHeight;

				reader.ReadBits(tmp, 8); //aspect_ratio_idc
				switch (tmp)
				{
				case 0: //Unspecified
					break;
				case 1: //1:1
					frameInfo->SetPAR(1);
					break;
				case 2: //12:11
					frameInfo->SetPAR(0.91666666666666666666666666666667);
					break;
				case 3: //10:11
					frameInfo->SetPAR(1.1);
					break;
				case 4: //16:11
					frameInfo->SetPAR(0.6875);
					break;
				case 5: //40:33
					frameInfo->SetPAR(0.825);
					break;
				case 6: //24:11
					frameInfo->SetPAR(0.45833333333333333333333333333333);
					break;
				case 7: //20:11
					frameInfo->SetPAR(0.55);
					break;
				case 8: //32:11
					frameInfo->SetPAR(0.34375);
					break;
				case 9: //80:33
					frameInfo->SetPAR(0.4125);
					break;
				case 10: //18:11
					frameInfo->SetPAR(0.61111111111111111111111111111111);
					break;
				case 11: //15:11
					frameInfo->SetPAR(0.73333333333333333333333333333333);
					break;
				case 12: //64:33
					frameInfo->SetPAR(0.515625);
					break;
				case 13: //160:99
					frameInfo->SetPAR(0.61875);
					break;
				case 14: //4:3
					frameInfo->SetPAR(0.75);
					break;
				case 15: //3:2
					frameInfo->SetPAR(0.66666666666666666666666666666667);
					break;
				case 16: //2:1
					frameInfo->SetPAR(0.5);
					break;
				case 255:
					sarWidth = 0;
					sarHeight = 0;
					reader.ReadBits(sarWidth, 16);
					reader.ReadBits(sarHeight, 16);
					if (sarWidth != 0 && sarHeight != 0)
					{
						frameInfo->SetPAR((Double)sarHeight / (Double)sarWidth);
					}
					break;
				}
			}
			reader.ReadBits(tmp, 1); //overscan_info_present_flag
			if (tmp)
			{
				reader.ReadBits(tmp, 1); //overscan_appropriate_flag
			}
			reader.ReadBits(tmp, 1); //video_signal_type_present_flag
			if (tmp)
			{
				reader.ReadBits(tmp, 3); //video_format
				reader.ReadBits(tmp, 1); //video_full_range_flag
				reader.ReadBits(tmp, 1); //colour_description_present_flag
				if (tmp)
				{
					reader.ReadBits(tmp, 8); //colour_primaries
					reader.ReadBits(tmp, 8); //transfer_characteristics
					reader.ReadBits(tmp, 8); //matrix_coeffs
				}
			}
		}
		frameInfo->rotateType = Media::RotateType::None;
	}
	MemFree(tmpBuff);
	return true;
}
