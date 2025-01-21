#include "Stdafx.h"
#include "Data/Compress/Inflater.h"

#define INFLATER_BUFFSIZE 1048576

#define TINFL_LZ_DICT_SIZE 32768
#define TINFL_MAX_HUFF_TABLES 3
#define TINFL_MAX_HUFF_SYMBOLS_0 288
#define TINFL_MAX_HUFF_SYMBOLS_1 32
#define TINFL_MAX_HUFF_SYMBOLS_2 19
#define TINFL_FAST_LOOKUP_BITS 10
#define TINFL_FAST_LOOKUP_SIZE (1 << TINFL_FAST_LOOKUP_BITS)

namespace Data
{
	namespace Compress
	{
		struct InflateDecompressor
		{
			UInt32 m_state;
			UInt32 m_num_bits;
			UInt32 m_zhdr0;
			UInt32 m_zhdr1;
			UInt32 m_z_adler32;
			UInt32 m_final;
			UInt32 m_type;
			UInt32 m_check_adler32;
			UInt32 m_dist;
			UInt32 m_counter;
			UInt32 m_num_extra;
			UInt32 m_table_sizes[TINFL_MAX_HUFF_TABLES];
			UOSInt m_bit_buf;
			OSInt m_dist_from_out_buf_start;
			Int16 m_look_up[TINFL_MAX_HUFF_TABLES][TINFL_FAST_LOOKUP_SIZE];
			Int16 m_tree_0[TINFL_MAX_HUFF_SYMBOLS_0 * 2];
			Int16 m_tree_1[TINFL_MAX_HUFF_SYMBOLS_1 * 2];
			Int16 m_tree_2[TINFL_MAX_HUFF_SYMBOLS_2 * 2];
			UInt8 m_code_size_0[TINFL_MAX_HUFF_SYMBOLS_0];
			UInt8 m_code_size_1[TINFL_MAX_HUFF_SYMBOLS_1];
			UInt8 m_code_size_2[TINFL_MAX_HUFF_SYMBOLS_2];
			UInt8 m_raw_header[4];
			UInt8 m_len_codes[TINFL_MAX_HUFF_SYMBOLS_0 + TINFL_MAX_HUFF_SYMBOLS_1 + 137];
		};

		struct InflateState
		{
			InflateDecompressor m_decomp;
			UInt32 m_dict_ofs;
			UInt32 m_dict_avail;
			UInt32 m_first_call;
			UInt32 m_has_flushed;
			Int32 m_window_bits;
			UInt8 m_dict[TINFL_LZ_DICT_SIZE];
			InflateStatus m_last_status;
			UInt8 writeBuffer[INFLATER_BUFFSIZE];
		};
	}
}

Data::Compress::InflateResult Data::Compress::Inflater::Inflate(Bool isEnd)
{
	NN<InflateState> pState;
	UInt32 n;
	UInt32 first_call;
	UInt32 decomp_flags = TINFL_FLAG_COMPUTE_ADLER32;
	OSInt in_bytes;
	OSInt out_bytes;
	OSInt orig_avail_in;
	InflateStatus status;

	pState = this->state;
	if (pState->m_window_bits > 0)
		decomp_flags |= TINFL_FLAG_PARSE_ZLIB_HEADER;
	orig_avail_in = this->avail_in;

	first_call = pState->m_first_call;
	pState->m_first_call = 0;
	if (((OSInt)pState->m_last_status) < 0)
		return InflateResult::DataError;

	if (pState->m_has_flushed && (!isEnd))
		return InflateResult::StreamError;
	pState->m_has_flushed |= isEnd;

	if (isEnd && first_call)
	{
		/* MZ_FINISH on the first call implies that the input and output buffers are large enough to hold the entire compressed/decompressed file. */
		decomp_flags |= TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF;
		in_bytes = this->avail_in;
		out_bytes = this->avail_out;
		status = tinfl_decompress(&pState->m_decomp, this->next_in, &in_bytes, this->next_out, this->next_out, &out_bytes, decomp_flags);
		pState->m_last_status = status;
		this->next_in += (UInt32)in_bytes;
		this->avail_in -= (UInt32)in_bytes;
		this->total_in += (UInt32)in_bytes;
		this->adler = pState->m_decomp.m_check_adler32;
		this->next_out += (UInt32)out_bytes;
		this->avail_out -= (UInt32)out_bytes;
		this->total_out += (UInt32)out_bytes;

		if ((OSInt)status < 0)
			return InflateResult::DataError;
		else if (status != InflateStatus::Done)
		{
			pState->m_last_status = InflateStatus::Failed;
			return InflateResult::BufError;
		}
		return InflateResult::StreamEnd;
	}
	/* flush != MZ_FINISH then we must assume there's more input. */
	if (!isEnd)
		decomp_flags |= TINFL_FLAG_HAS_MORE_INPUT;

	if (pState->m_dict_avail)
	{
		n = MZ_MIN(pState->m_dict_avail, this->avail_out);
		MemCopyNO(this->next_out, pState->m_dict + pState->m_dict_ofs, n);
		this->next_out += n;
		this->avail_out -= n;
		this->total_out += n;
		pState->m_dict_avail -= n;
		pState->m_dict_ofs = (pState->m_dict_ofs + n) & (TINFL_LZ_DICT_SIZE - 1);
		return ((pState->m_last_status == InflateStatus::Done) && (!pState->m_dict_avail)) ? InflateResult::StreamEnd : InflateResult::Ok;
	}

	for (;;)
	{
		in_bytes = this->avail_in;
		out_bytes = TINFL_LZ_DICT_SIZE - pState->m_dict_ofs;

		status = tinfl_decompress(&pState->m_decomp, this->next_in, &in_bytes, pState->m_dict, pState->m_dict + pState->m_dict_ofs, &out_bytes, decomp_flags);
		pState->m_last_status = status;

		this->next_in += (UInt32)in_bytes;
		this->avail_in -= (UInt32)in_bytes;
		this->total_in += (UInt32)in_bytes;
		this->adler = pState->m_decomp.m_check_adler32;

		pState->m_dict_avail = (UInt32)out_bytes;

		n = MZ_MIN(pState->m_dict_avail, this->avail_out);
		MemCopyNO(this->next_out, pState->m_dict + pState->m_dict_ofs, n);
		this->next_out += n;
		this->avail_out -= n;
		this->total_out += n;
		pState->m_dict_avail -= n;
		pState->m_dict_ofs = (pState->m_dict_ofs + n) & (TINFL_LZ_DICT_SIZE - 1);

		if ((OSInt)status < 0)
			return InflateResult::DataError; /* Stream is corrupted (there could be some uncompressed data left in the output dictionary - oh well). */
		else if ((status == InflateStatus::NeedsMoreInput) && (!orig_avail_in))
			return InflateResult::BufError; /* Signal caller that we can't make forward progress without supplying more input or by setting flush to MZ_FINISH. */
		else if (isEnd)
		{
			/* The output buffer MUST be large to hold the remaining uncompressed data when flush==MZ_FINISH. */
			if (status == InflateStatus::Done)
				return pState->m_dict_avail ? InflateResult::BufError : InflateResult::StreamEnd;
			/* status here must be TINFL_STATUS_HAS_MORE_OUTPUT, which means there's at least 1 more byte on the way. If there's no more room left in the output buffer then something is wrong. */
			else if (!this->avail_out)
				return InflateResult::BufError;
		}
		else if ((status == InflateStatus::Done) || (!this->avail_in) || (!this->avail_out) || (pState->m_dict_avail))
			break;
	}

	return ((status == InflateStatus::Done) && (!pState->m_dict_avail)) ? InflateResult::StreamEnd : InflateResult::Ok;
}

Data::Compress::Inflater::Inflater(NN<IO::Stream> decStm, Bool hasHeader)
{
	this->decStm = decStm;
	this->data_type = 0;
	this->adler = 0;
	this->msg = 0;
	this->total_in = 0;
	this->total_out = 0;
	this->reserved = 0;

	NN<InflateState> pDecomp = MemAllocNN(InflateState);
	this->state = pDecomp;

	pDecomp->m_decomp.m_state = 0;
	pDecomp->m_dict_ofs = 0;
	pDecomp->m_dict_avail = 0;
	pDecomp->m_last_status = InflateStatus::NeedsMoreInput;
	pDecomp->m_first_call = 1;
	pDecomp->m_has_flushed = 0;
	pDecomp->m_window_bits = hasHeader?15:-15;
	this->next_out = pDecomp->writeBuffer;
	this->avail_out = INFLATER_BUFFSIZE;
}

Data::Compress::Inflater::~Inflater()
{
	MemFreeNN(this->state);
}

Bool Data::Compress::Inflater::IsDown() const
{
	return this->decStm->IsDown();
}

UOSInt Data::Compress::Inflater::Read(const Data::ByteArray &buff)
{
	return 0;
}

UOSInt Data::Compress::Inflater::Write(Data::ByteArrayR buff)
{
	this->next_in = buff.Ptr();
	this->avail_in = (UInt32)buff.GetSize();
	while (true || this->avail_in > 0)
	{
		this->Inflate(false);
		if (this->avail_out == INFLATER_BUFFSIZE)
		{
//			error = true;
			break;
		}
		else //	if (ret == MZ_STREAM_END || mzstm->avail_in != lastSize)
		{
			this->decStm->Write(Data::ByteArrayR(this->state->writeBuffer, INFLATER_BUFFSIZE - this->avail_out));
			this->avail_out = INFLATER_BUFFSIZE;
			this->next_out = this->state->writeBuffer;
		}
	}
	return buff.GetSize() - this->avail_in;
}
