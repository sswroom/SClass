#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/Compress/Inflater.h"
#include "Math/Math.h"
#include <assert.h>

#define INFLATER_BUFFSIZE 1048576

#define INFLATER_CLEAR_ARR(arr) MemClear(arr, sizeof(arr))
#define INFLATER_MEMCPY(d, s, c) MemCopyNOShort(d, s, c)
#define INFLATER_MEMSET(p, c, l) MemFillB((UInt8*)p, l, c)
#define INFLATER_MEMSETW(p, c, l) MemFillW((UInt8*)p, (l) >> 1, c)
//#define INFLATER_MEMSET(p, c, l) memset(p, c, l)
#define INFLATER_ASSERT(x) assert(x)

#define INFLATER_LZ_DICT_SIZE 32768
#define INFLATER_MAX_HUFF_TABLES 3
#define INFLATER_MAX_HUFF_SYMBOLS_0 288
#define INFLATER_MAX_HUFF_SYMBOLS_1 32
#define INFLATER_MAX_HUFF_SYMBOLS_2 19
#define INFLATER_FAST_LOOKUP_BITS 10
#define INFLATER_FAST_LOOKUP_SIZE (1 << INFLATER_FAST_LOOKUP_BITS)

#define INFLATER_FLAG_PARSE_ZLIB_HEADER 1
#define INFLATER_FLAG_HAS_MORE_INPUT 2
#define INFLATER_FLAG_USING_NON_WRAPPING_OUTPUT_BUF 4
#define INFLATER_FLAG_COMPUTE_ADLER32 8

#define INFLATER_CR_BEGIN  \
    switch (r->m_state) \
    {                   \
        case 0:

#define INFLATER_CR_RETURN(state_index, result) \
    {                                        \
        status = result;                     \
        r->m_state = state_index;            \
        goto common_exit;                    \
        case state_index:;                   \
    }

#define INFLATER_CR_RETURN_FOREVER(state_index, result) \
    {                                                \
        for (;;)                                     \
        {                                            \
            INFLATER_CR_RETURN(state_index, result);    \
        }                                            \
    }

#define INFLATER_GET_BYTE(state_index, c)                                                                                                                           \
    {                                                                                                                                                            \
        while (pIn_buf_cur >= pIn_buf_end)                                                                                                                       \
        {                                                                                                                                                        \
            INFLATER_CR_RETURN(state_index, (decomp_flags & INFLATER_FLAG_HAS_MORE_INPUT) ? Data::Compress::InflateStatus::NeedsMoreInput : Data::Compress::InflateStatus::FailedCannotMakeProgress); \
        }                                                                                                                                                        \
        c = *pIn_buf_cur++;                                                                                                                                      \
    }

#define INFLATER_NEED_BITS(state_index, n)                \
    do                                                 \
    {                                                  \
        UInt32 c;                                     \
        INFLATER_GET_BYTE(state_index, c);                \
        bit_buf |= (((UOSInt)c) << num_bits); \
        num_bits += 8;                                 \
    } while (num_bits < (UInt32)(n))

#define INFLATER_SKIP_BITS(state_index, n)      \
    {                                        \
        if (num_bits < (UInt32)(n))         \
        {                                    \
            INFLATER_NEED_BITS(state_index, n); \
        }                                    \
        bit_buf >>= (n);                     \
        num_bits -= (n);                     \
    }

#define INFLATER_GET_BITS(state_index, b, n)    \
    {                                        \
        if (num_bits < (UInt32)(n))         \
        {                                    \
            INFLATER_NEED_BITS(state_index, n); \
        }                                    \
        b = (UInt32)(bit_buf & (((UOSInt)1 << (n)) - 1));      \
        bit_buf >>= (n);                     \
        num_bits -= (UInt32)(n);                     \
    }

#define INFLATER_CR_FINISH }

#define INFLATER_HUFF_BITBUF_FILL(state_index, pLookUp, pTree)          \
    do                                                               \
    {                                                                \
        temp = pLookUp[bit_buf & (INFLATER_FAST_LOOKUP_SIZE - 1)];      \
        if (temp >= 0)                                               \
        {                                                            \
            code_len = (UInt32)(temp >> 9);                                    \
            if ((code_len) && (num_bits >= code_len))                \
                break;                                               \
        }                                                            \
        else if (num_bits > INFLATER_FAST_LOOKUP_BITS)                  \
        {                                                            \
            code_len = INFLATER_FAST_LOOKUP_BITS;                       \
            do                                                       \
            {                                                        \
                temp = pTree[(UInt32)~temp + ((bit_buf >> code_len++) & 1)]; \
            } while ((temp < 0) && (num_bits >= (code_len + 1)));    \
            if (temp >= 0)                                           \
                break;                                               \
        }                                                            \
        INFLATER_GET_BYTE(state_index, c);                              \
        bit_buf |= (((UOSInt)c) << num_bits);               \
        num_bits += 8;                                               \
    } while (num_bits < 15);

#define INFLATER_HUFF_DECODE(state_index, sym, pLookUp, pTree)                                                                         \
    {                                                                                                                               \
        Int32 temp;                                                                                                                   \
        UInt32 code_len, c;                                                                                                        \
        if (num_bits < 15)                                                                                                          \
        {                                                                                                                           \
            if ((pIn_buf_end - pIn_buf_cur) < 2)                                                                                    \
            {                                                                                                                       \
                INFLATER_HUFF_BITBUF_FILL(state_index, pLookUp, pTree);                                                                \
            }                                                                                                                       \
            else                                                                                                                    \
            {                                                                                                                       \
                bit_buf |= (((UOSInt)pIn_buf_cur[0]) << num_bits) | (((UOSInt)pIn_buf_cur[1]) << (num_bits + 8)); \
                pIn_buf_cur += 2;                                                                                                   \
                num_bits += 16;                                                                                                     \
            }                                                                                                                       \
        }                                                                                                                           \
        if ((temp = pLookUp[bit_buf & (INFLATER_FAST_LOOKUP_SIZE - 1)]) >= 0)                                                          \
            code_len = (UInt32)(temp >> 9), temp &= 511;                                                                                      \
        else                                                                                                                        \
        {                                                                                                                           \
            code_len = INFLATER_FAST_LOOKUP_BITS;                                                                                      \
            do                                                                                                                      \
            {                                                                                                                       \
                temp = pTree[(UInt32)~temp + ((bit_buf >> code_len++) & 1)];                                                                \
            } while (temp < 0);                                                                                                     \
        }                                                                                                                           \
        sym = (UInt32)temp;                                                                                                                 \
        bit_buf >>= code_len;                                                                                                       \
        num_bits -= code_len;                                                                                                       \
    }

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
			UInt32 m_table_sizes[INFLATER_MAX_HUFF_TABLES];
			UOSInt m_bit_buf;
			UOSInt m_dist_from_out_buf_start;
			Int16 m_look_up[INFLATER_MAX_HUFF_TABLES][INFLATER_FAST_LOOKUP_SIZE];
			Int16 m_tree_0[INFLATER_MAX_HUFF_SYMBOLS_0 * 2];
			Int16 m_tree_1[INFLATER_MAX_HUFF_SYMBOLS_1 * 2];
			Int16 m_tree_2[INFLATER_MAX_HUFF_SYMBOLS_2 * 2];
			UInt8 m_code_size_0[INFLATER_MAX_HUFF_SYMBOLS_0];
			UInt8 m_code_size_1[INFLATER_MAX_HUFF_SYMBOLS_1];
			UInt8 m_code_size_2[INFLATER_MAX_HUFF_SYMBOLS_2];
			UInt8 m_raw_header[4];
			UInt8 m_len_codes[INFLATER_MAX_HUFF_SYMBOLS_0 + INFLATER_MAX_HUFF_SYMBOLS_1 + 137];
		};

		struct InflateState
		{
			InflateDecompressor m_decomp;
			InflateStatus m_last_status;
//			tinfl_decompressor m_decomp;
			UInt8 m_dict[INFLATER_LZ_DICT_SIZE];
			UInt32 m_dict_ofs;
			Bool hasFlushed;
			Bool hasHeader;
		};
	}
}

void Data::Compress::Inflater::ClearTree(NN<InflateDecompressor> r)
{
	if (r->m_type == 0)
		INFLATER_CLEAR_ARR(r->m_tree_0)
	else if (r->m_type == 1)
		INFLATER_CLEAR_ARR(r->m_tree_1)
	else
		INFLATER_CLEAR_ARR(r->m_tree_2);
}

Data::Compress::InflateStatus Data::Compress::Inflater::Decompress(NN<InflateDecompressor> r, UnsafeArray<const UInt8> pIn_buf_next, InOutParam<UOSInt> pIn_buf_size, UnsafeArray<UInt8> pOut_buf_start, UnsafeArray<UInt8> pOut_buf_next, InOutParam<UOSInt> pOut_buf_size, const UInt32 decomp_flags)
{
	static const UInt16 s_length_base[31] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0 };
	static const UInt8 s_length_extra[31] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0 };
	static const UInt16 s_dist_base[32] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0, 0 };
	static const UInt8 s_dist_extra[32] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };
	static const UInt8 s_length_dezigzag[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
	static const UInt16 s_min_table_sizes[3] = { 257, 1, 4 };

	Int16 *pTrees[3];
	UInt8 *pCode_sizes[3];

	Data::Compress::InflateStatus status = Data::Compress::InflateStatus::Failed;
	UInt32 num_bits, dist, counter, num_extra;
	UOSInt bit_buf;
	UnsafeArray<const UInt8> pIn_buf_cur = pIn_buf_next;
	UnsafeArray<const UInt8> pIn_buf_end = pIn_buf_next + pIn_buf_size.Get();
	UnsafeArray<UInt8> pOut_buf_cur = pOut_buf_next;
	UnsafeArray<const UInt8> pOut_buf_end = pOut_buf_next + pOut_buf_size.Get();
	UOSInt out_buf_size_mask = (decomp_flags & INFLATER_FLAG_USING_NON_WRAPPING_OUTPUT_BUF) ? (UOSInt)-1 : ((UOSInt)(pOut_buf_next - pOut_buf_start) + pOut_buf_size.Get()) - 1, dist_from_out_buf_start;

	/* Ensure the output buffer's size is a power of 2, unless the output buffer is large enough to hold the entire output file (in which case it doesn't matter). */
	if (((out_buf_size_mask + 1) & out_buf_size_mask) || (pOut_buf_next < pOut_buf_start))
	{
		pIn_buf_size.Set(0);
		pOut_buf_size.Set(0);
		return Data::Compress::InflateStatus::BadParam;
	}

	pTrees[0] = r->m_tree_0;
	pTrees[1] = r->m_tree_1;
	pTrees[2] = r->m_tree_2;
	pCode_sizes[0] = r->m_code_size_0;
	pCode_sizes[1] = r->m_code_size_1;
	pCode_sizes[2] = r->m_code_size_2;

	num_bits = r->m_num_bits;
	bit_buf = r->m_bit_buf;
	dist = r->m_dist;
	counter = r->m_counter;
	num_extra = r->m_num_extra;
	dist_from_out_buf_start = r->m_dist_from_out_buf_start;
	INFLATER_CR_BEGIN

	bit_buf = num_bits = dist = counter = num_extra = r->m_zhdr0 = r->m_zhdr1 = 0;
	r->m_z_adler32 = r->m_check_adler32 = 1;
	if (decomp_flags & INFLATER_FLAG_PARSE_ZLIB_HEADER)
	{
		INFLATER_GET_BYTE(1, r->m_zhdr0);
		INFLATER_GET_BYTE(2, r->m_zhdr1);
		counter = (((r->m_zhdr0 * 256 + r->m_zhdr1) % 31 != 0) || (r->m_zhdr1 & 32) || ((r->m_zhdr0 & 15) != 8));
		if (!(decomp_flags & INFLATER_FLAG_USING_NON_WRAPPING_OUTPUT_BUF))
			counter |= (((1U << (8U + (r->m_zhdr0 >> 4))) > 32768U) || ((out_buf_size_mask + 1) < (UOSInt)((UOSInt)1 << (8U + (r->m_zhdr0 >> 4)))));
		if (counter)
		{
			INFLATER_CR_RETURN_FOREVER(36, Data::Compress::InflateStatus::Failed);
		}
	}

	do
	{
		INFLATER_GET_BITS(3, r->m_final, 3);
		r->m_type = r->m_final >> 1;
		if (r->m_type == 0)
		{
			INFLATER_SKIP_BITS(5, num_bits & 7);
			for (counter = 0; counter < 4; ++counter)
			{
				if (num_bits)
					INFLATER_GET_BITS(6, r->m_raw_header[counter], 8)
				else
					INFLATER_GET_BYTE(7, r->m_raw_header[counter]);
			}
			if ((counter = (UInt32)(r->m_raw_header[0] | (r->m_raw_header[1] << 8))) != (UInt32)(0xFFFF ^ (r->m_raw_header[2] | (r->m_raw_header[3] << 8))))
			{
				INFLATER_CR_RETURN_FOREVER(39, Data::Compress::InflateStatus::Failed);
			}
			while ((counter) && (num_bits))
			{
				INFLATER_GET_BITS(51, dist, 8);
				while (UnsafeArray<const UInt8>(pOut_buf_cur) >= pOut_buf_end)
				{
					INFLATER_CR_RETURN(52, Data::Compress::InflateStatus::HasMoreOutput);
				}
				*pOut_buf_cur++ = (UInt8)dist;
				counter--;
			}
			while (counter)
			{
				UOSInt n;
				while (UnsafeArray<const UInt8>(pOut_buf_cur) >= pOut_buf_end)
				{
					INFLATER_CR_RETURN(9, Data::Compress::InflateStatus::HasMoreOutput);
				}
				while (pIn_buf_cur >= pIn_buf_end)
				{
					INFLATER_CR_RETURN(38, (decomp_flags & INFLATER_FLAG_HAS_MORE_INPUT) ? Data::Compress::InflateStatus::NeedsMoreInput : Data::Compress::InflateStatus::FailedCannotMakeProgress);
				}
				n = Math_Min(Math_Min((UOSInt)(pOut_buf_end - UnsafeArray<const UInt8>(pOut_buf_cur)), (UOSInt)(pIn_buf_end - pIn_buf_cur)), counter);
				INFLATER_MEMCPY(pOut_buf_cur.Ptr(), pIn_buf_cur.Ptr(), n);
				pIn_buf_cur += n;
				pOut_buf_cur += n;
				counter -= (UInt32)n;
			}
		}
		else if (r->m_type == 3)
		{
			INFLATER_CR_RETURN_FOREVER(10, Data::Compress::InflateStatus::Failed);
		}
		else
		{
			if (r->m_type == 1)
			{
				UInt8 *p = r->m_code_size_0;
				UInt32 i;
				r->m_table_sizes[0] = 288;
				r->m_table_sizes[1] = 32;
				INFLATER_MEMSETW(r->m_code_size_1, 0x505, 32);
				for (i = 0; i <= 143; ++i)
					*p++ = 8;
				for (; i <= 255; ++i)
					*p++ = 9;
				for (; i <= 279; ++i)
					*p++ = 7;
				for (; i <= 287; ++i)
					*p++ = 8;
			}
			else
			{
				for (counter = 0; counter < 3; counter++)
				{
					INFLATER_GET_BITS(11, r->m_table_sizes[counter], "\05\05\04"[counter]);
					r->m_table_sizes[counter] += s_min_table_sizes[counter];
				}
				INFLATER_CLEAR_ARR(r->m_code_size_2);
				for (counter = 0; counter < r->m_table_sizes[2]; counter++)
				{
					UInt32 s;
					INFLATER_GET_BITS(14, s, 3);
					r->m_code_size_2[s_length_dezigzag[counter]] = (UInt8)s;
				}
				r->m_table_sizes[2] = 19;
			}
			for (; (Int32)r->m_type >= 0; r->m_type--)
			{
				Int32 tree_next, tree_cur;
				Int16 *pLookUp;
				Int16 *pTree;
				UInt8 *pCode_size;
				UInt32 i, j, used_syms, total, sym_index, next_code[17], total_syms[16];
				pLookUp = r->m_look_up[r->m_type];
				pTree = pTrees[r->m_type];
				pCode_size = pCode_sizes[r->m_type];
				INFLATER_CLEAR_ARR(total_syms);
				INFLATER_MEMSETW(pLookUp, 0, sizeof(r->m_look_up[0]));
				ClearTree(r);
				for (i = 0; i < r->m_table_sizes[r->m_type]; ++i)
					total_syms[pCode_size[i]]++;
				used_syms = 0, total = 0;
				next_code[0] = next_code[1] = 0;
				for (i = 1; i <= 15; ++i)
				{
					used_syms += total_syms[i];
					next_code[i + 1] = (total = ((total + total_syms[i]) << 1));
				}
				if ((65536 != total) && (used_syms > 1))
				{
					INFLATER_CR_RETURN_FOREVER(35, Data::Compress::InflateStatus::Failed);
				}
				for (tree_next = -1, sym_index = 0; sym_index < r->m_table_sizes[r->m_type]; ++sym_index)
				{
					UInt32 rev_code = 0, l, cur_code, code_size = pCode_size[sym_index];
					if (!code_size)
						continue;
					cur_code = next_code[code_size]++;
					for (l = code_size; l > 0; l--, cur_code >>= 1)
						rev_code = (rev_code << 1) | (cur_code & 1);
					if (code_size <= INFLATER_FAST_LOOKUP_BITS)
					{
						Int16 k = (Int16)((code_size << 9) | sym_index);
						while (rev_code < INFLATER_FAST_LOOKUP_SIZE)
						{
							pLookUp[rev_code] = k;
							rev_code += (1 << code_size);
						}
						continue;
					}
					if (0 == (tree_cur = pLookUp[rev_code & (INFLATER_FAST_LOOKUP_SIZE - 1)]))
					{
						pLookUp[rev_code & (INFLATER_FAST_LOOKUP_SIZE - 1)] = (Int16)tree_next;
						tree_cur = tree_next;
						tree_next -= 2;
					}
					rev_code >>= (INFLATER_FAST_LOOKUP_BITS - 1);
					for (j = code_size; j > (INFLATER_FAST_LOOKUP_BITS + 1); j--)
					{
						tree_cur -= (Int32)((rev_code >>= 1) & 1);
						if (!pTree[-tree_cur - 1])
						{
							pTree[-tree_cur - 1] = (Int16)tree_next;
							tree_cur = tree_next;
							tree_next -= 2;
						}
						else
							tree_cur = pTree[-tree_cur - 1];
					}
					tree_cur -= (Int32)((rev_code >>= 1) & 1);
					pTree[-tree_cur - 1] = (Int16)sym_index;
				}
				if (r->m_type == 2)
				{
					for (counter = 0; counter < (r->m_table_sizes[0] + r->m_table_sizes[1]);)
					{
						UInt32 s;
						INFLATER_HUFF_DECODE(16, dist, r->m_look_up[2], r->m_tree_2);
						if (dist < 16)
						{
							r->m_len_codes[counter++] = (UInt8)dist;
							continue;
						}
						if ((dist == 16) && (!counter))
						{
							INFLATER_CR_RETURN_FOREVER(17, Data::Compress::InflateStatus::Failed);
						}
						num_extra = (UInt32)"\02\03\07"[dist - 16];
						INFLATER_GET_BITS(18, s, num_extra);
						s += (UInt32)"\03\03\013"[dist - 16];
						INFLATER_MEMSET(r->m_len_codes + counter, (dist == 16) ? r->m_len_codes[counter - 1] : 0, s);
						counter += s;
					}
					if ((r->m_table_sizes[0] + r->m_table_sizes[1]) != counter)
					{
						INFLATER_CR_RETURN_FOREVER(21, Data::Compress::InflateStatus::Failed);
					}
					INFLATER_MEMCPY(r->m_code_size_0, r->m_len_codes, r->m_table_sizes[0]);
					INFLATER_MEMCPY(r->m_code_size_1, r->m_len_codes + r->m_table_sizes[0], r->m_table_sizes[1]);
				}
			}
			for (;;)
			{
				UnsafeArray<UInt8> pSrc;
				for (;;)
				{
					if (((pIn_buf_end - pIn_buf_cur) < 4) || ((pOut_buf_end - pOut_buf_cur) < 2))
					{
						INFLATER_HUFF_DECODE(23, counter, r->m_look_up[0], r->m_tree_0);
						if (counter >= 256)
							break;
						while (UnsafeArray<const UInt8>(pOut_buf_cur) >= pOut_buf_end)
						{
							INFLATER_CR_RETURN(24, Data::Compress::InflateStatus::HasMoreOutput);
						}
						*pOut_buf_cur++ = (UInt8)counter;
					}
					else
					{
						Int32 sym2;
						UInt32 code_len;
#if _OSINT_SIZE == 64
						if (num_bits < 30)
						{
							bit_buf |= (((UOSInt)ReadUInt32(&pIn_buf_cur[0])) << num_bits);
							pIn_buf_cur += 4;
							num_bits += 32;
						}
#else
					if (num_bits < 15)
					{
						bit_buf |= (((UOSInt)ReadUInt16(&pIn_buf_cur[0])) << num_bits);
						pIn_buf_cur += 2;
						num_bits += 16;
					}
#endif
						if ((sym2 = r->m_look_up[0][bit_buf & (INFLATER_FAST_LOOKUP_SIZE - 1)]) >= 0)
							code_len = (UInt32)(sym2 >> 9);
						else
						{
							code_len = INFLATER_FAST_LOOKUP_BITS;
							do
							{
								sym2 = r->m_tree_0[~sym2 + (OSInt)((bit_buf >> code_len++) & 1)];
							} while (sym2 < 0);
						}
						counter = (UInt32)sym2;
						bit_buf >>= code_len;
						num_bits -= code_len;
						if (counter & 256)
							break;

#if _OSINT_SIZE != 64
						if (num_bits < 15)
						{
							bit_buf |= (((UOSInt)ReadUInt16(&pIn_buf_cur[0])) << num_bits);
							pIn_buf_cur += 2;
							num_bits += 16;
						}
#endif
						if ((sym2 = r->m_look_up[0][bit_buf & (INFLATER_FAST_LOOKUP_SIZE - 1)]) >= 0)
							code_len = (UInt32)(sym2 >> 9);
						else
						{
							code_len = INFLATER_FAST_LOOKUP_BITS;
							do
							{
								sym2 = r->m_tree_0[~sym2 + (OSInt)((bit_buf >> code_len++) & 1)];
							} while (sym2 < 0);
						}
						bit_buf >>= code_len;
						num_bits -= code_len;

						pOut_buf_cur[0] = (UInt8)counter;
						if (sym2 & 256)
						{
							pOut_buf_cur++;
							counter = (UInt32)sym2;
							break;
						}
						pOut_buf_cur[1] = (UInt8)sym2;
						pOut_buf_cur += 2;
					}
				}
				if ((counter &= 511) == 256)
					break;

				num_extra = s_length_extra[counter - 257];
				counter = s_length_base[counter - 257];
				if (num_extra)
				{
					UInt32 extra_bits;
					INFLATER_GET_BITS(25, extra_bits, num_extra);
					counter += extra_bits;
				}

				INFLATER_HUFF_DECODE(26, dist, r->m_look_up[1], r->m_tree_1);
				num_extra = s_dist_extra[dist];
				dist = s_dist_base[dist];
				if (num_extra)
				{
					UInt32 extra_bits;
					INFLATER_GET_BITS(27, extra_bits, num_extra);
					dist += extra_bits;
				}

				dist_from_out_buf_start = (UOSInt)(pOut_buf_cur - pOut_buf_start);
				if ((dist == 0 || dist > dist_from_out_buf_start || dist_from_out_buf_start == 0) && (decomp_flags & INFLATER_FLAG_USING_NON_WRAPPING_OUTPUT_BUF))
				{
					INFLATER_CR_RETURN_FOREVER(37, Data::Compress::InflateStatus::Failed);
				}

				pSrc = pOut_buf_start + ((dist_from_out_buf_start - dist) & out_buf_size_mask);

				if (UnsafeArray<const UInt8>(Math_Max(pOut_buf_cur, pSrc) + counter) > pOut_buf_end)
				{
					while (counter--)
					{
						while (UnsafeArray<const UInt8>(pOut_buf_cur) >= pOut_buf_end)
						{
							INFLATER_CR_RETURN(53, Data::Compress::InflateStatus::HasMoreOutput);
						}
						*pOut_buf_cur++ = pOut_buf_start[(dist_from_out_buf_start++ - dist) & out_buf_size_mask];
					}
					continue;
				}
				else if ((counter >= 9) && (counter <= dist))
				{
					INFLATER_MEMCPY(pOut_buf_cur.Ptr(), pSrc.Ptr(), counter);
					pOut_buf_cur += counter;
					continue;
				}
				while (counter > 2)
				{
					pOut_buf_cur[0] = pSrc[0];
					pOut_buf_cur[1] = pSrc[1];
					pOut_buf_cur[2] = pSrc[2];
					pOut_buf_cur += 3;
					pSrc += 3;
					counter -= 3;
				}
				if (counter > 0)
				{
					pOut_buf_cur[0] = pSrc[0];
					if (counter > 1)
						pOut_buf_cur[1] = pSrc[1];
					pOut_buf_cur += counter;
				}
			}
		}
	} while (!(r->m_final & 1));

	/* Ensure byte alignment and put back any bytes from the bitbuf if we've looked ahead too far on gzip, or other Deflate streams followed by arbitrary data. */
	/* I'm being super conservative here. A number of simplifications can be made to the byte alignment part, and the Adler32 check shouldn't ever need to worry about reading from the bitbuf now. */
	INFLATER_SKIP_BITS(32, num_bits & 7);
	while ((pIn_buf_cur > pIn_buf_next) && (num_bits >= 8))
	{
		--pIn_buf_cur;
		num_bits -= 8;
	}
	bit_buf &= ~(~(UOSInt)0 << num_bits);
	INFLATER_ASSERT(!num_bits); /* if this assert fires then we've read beyond the end of non-deflate/zlib streams with following data (such as gzip streams). */

	if (decomp_flags & INFLATER_FLAG_PARSE_ZLIB_HEADER)
	{
		for (counter = 0; counter < 4; ++counter)
		{
			UInt32 s;
			if (num_bits)
				INFLATER_GET_BITS(41, s, 8)
			else
				INFLATER_GET_BYTE(42, s);
			r->m_z_adler32 = (r->m_z_adler32 << 8) | s;
		}
	}
	INFLATER_CR_RETURN_FOREVER(34, Data::Compress::InflateStatus::Done);

	INFLATER_CR_FINISH

common_exit:
	/* As long as we aren't telling the caller that we NEED more input to make forward progress: */
	/* Put back any bytes from the bitbuf in case we've looked ahead too far on gzip, or other Deflate streams followed by arbitrary data. */
	/* We need to be very careful here to NOT push back any bytes we definitely know we need to make forward progress, though, or we'll lock the caller up into an inf loop. */
	if ((status != Data::Compress::InflateStatus::NeedsMoreInput) && (status != Data::Compress::InflateStatus::FailedCannotMakeProgress))
	{
		while ((pIn_buf_cur > pIn_buf_next) && (num_bits >= 8))
		{
			--pIn_buf_cur;
			num_bits -= 8;
		}
	}
	r->m_num_bits = num_bits;
	r->m_bit_buf = bit_buf & ~(~(UOSInt)0 << num_bits);
	r->m_dist = dist;
	r->m_counter = counter;
	r->m_num_extra = num_extra;
	r->m_dist_from_out_buf_start = dist_from_out_buf_start;
	pIn_buf_size.Set((UOSInt)(pIn_buf_cur - pIn_buf_next));
	pOut_buf_size.Set((UOSInt)(pOut_buf_cur - pOut_buf_next));
	if ((decomp_flags & (INFLATER_FLAG_PARSE_ZLIB_HEADER | INFLATER_FLAG_COMPUTE_ADLER32)) && ((OSInt)status >= 0))
	{
		UnsafeArray<const UInt8> ptr = pOut_buf_next;
		UOSInt buf_len = (UOSInt)(pOut_buf_cur - pOut_buf_next);
		UInt32 i, s1 = r->m_check_adler32 & 0xffff, s2 = r->m_check_adler32 >> 16;
		if (buf_len)
		{
			UOSInt block_len = buf_len % 5552;
			for (i = 0; i + 7 < block_len; i += 8, ptr += 8)
			{
				s1 += ptr[0], s2 += s1;
				s1 += ptr[1], s2 += s1;
				s1 += ptr[2], s2 += s1;
				s1 += ptr[3], s2 += s1;
				s1 += ptr[4], s2 += s1;
				s1 += ptr[5], s2 += s1;
				s1 += ptr[6], s2 += s1;
				s1 += ptr[7], s2 += s1;
			}
			for (; i < block_len; ++i)
				s1 += *ptr++, s2 += s1;
			s1 %= 65521U, s2 %= 65521U;
			buf_len -= block_len;
			while (buf_len)
			{
				block_len = 5552;
				for (i = 0; i + 7 < block_len; i += 8, ptr += 8)
				{
					s1 += ptr[0], s2 += s1;
					s1 += ptr[1], s2 += s1;
					s1 += ptr[2], s2 += s1;
					s1 += ptr[3], s2 += s1;
					s1 += ptr[4], s2 += s1;
					s1 += ptr[5], s2 += s1;
					s1 += ptr[6], s2 += s1;
					s1 += ptr[7], s2 += s1;
				}
				s1 %= 65521U, s2 %= 65521U;
				buf_len -= block_len;
			}
		}
		r->m_check_adler32 = (s2 << 16) + s1;
		if ((status == Data::Compress::InflateStatus::Done) && (decomp_flags & INFLATER_FLAG_PARSE_ZLIB_HEADER) && (r->m_check_adler32 != r->m_z_adler32))
			status = Data::Compress::InflateStatus::Adler32Mismatch;
	}
	return status;
}

Data::Compress::InflateResult Data::Compress::Inflater::Inflate(Bool isEnd)
{
	NN<InflateState> pState;
	UInt32 n;
	UInt32 decomp_flags = INFLATER_FLAG_COMPUTE_ADLER32;
	UOSInt in_bytes;
	UOSInt out_bytes;
	UOSInt orig_avail_in;

	pState = this->state;
	if (pState->hasHeader)
		decomp_flags |= INFLATER_FLAG_PARSE_ZLIB_HEADER;
	orig_avail_in = this->avail_in;

	if (((OSInt)pState->m_last_status) < 0)
		return InflateResult::DataError;

	if (pState->hasFlushed && (!isEnd))
		return InflateResult::StreamError;
	pState->hasFlushed |= isEnd;

	if (!isEnd)
		decomp_flags |= INFLATER_FLAG_HAS_MORE_INPUT;

	for (;;)
	{
		in_bytes = this->avail_in;
		out_bytes = INFLATER_LZ_DICT_SIZE - pState->m_dict_ofs;

		InflateStatus status = Decompress(pState->m_decomp, this->next_in, in_bytes, pState->m_dict, pState->m_dict + pState->m_dict_ofs, out_bytes, decomp_flags);
		pState->m_last_status = status;

		this->next_in += (UInt32)in_bytes;
		this->avail_in -= in_bytes;

		n = (UInt32)out_bytes;
		this->decStm->Write(Data::ByteArrayR(pState->m_dict + pState->m_dict_ofs, n));
		pState->m_dict_ofs = (pState->m_dict_ofs + n) & (INFLATER_LZ_DICT_SIZE - 1);

		if ((OSInt)status < 0)
			return InflateResult::DataError;
		else if ((status == InflateStatus::NeedsMoreInput) && (!orig_avail_in))
			return InflateResult::BufError;
		else if (isEnd)
		{
			if (status == InflateStatus::Done)
				return InflateResult::StreamEnd;
		}
		else if ((status == InflateStatus::Done) || (!this->avail_in))
			return (status == InflateStatus::Done) ? InflateResult::StreamEnd : InflateResult::Ok;
	}
}

Data::Compress::Inflater::Inflater(NN<IO::Stream> decStm, Bool hasHeader) : Stream(CSTR("Inflater"))
{
	this->decStm = decStm;
	this->lastRes = InflateResult::Ok;

	NN<InflateState> pDecomp = MemAllocNN(InflateState);
	this->state = pDecomp;

	pDecomp->m_decomp.m_state = 0;
	pDecomp->m_dict_ofs = 0;
	pDecomp->m_last_status = InflateStatus::NeedsMoreInput;
	pDecomp->hasFlushed = false;
	pDecomp->hasHeader = hasHeader;
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
	this->avail_in = buff.GetSize();
	while (true || this->avail_in > 0)
	{
		InflateResult res = this->lastRes = this->Inflate(false);
		if (res == InflateResult::StreamEnd || this->avail_in == 0 || (OSInt)res < 0)
		{
//			error = true;
			break;
		}
	}
	return buff.GetSize() - this->avail_in;
}


Int32 Data::Compress::Inflater::Flush()
{
	return 0;
}

void Data::Compress::Inflater::Close()
{
}

Bool Data::Compress::Inflater::Recover()
{
	return this->decStm->Recover();
}

IO::StreamType Data::Compress::Inflater::GetStreamType() const
{
	return IO::StreamType::Inflater;
}

Bool Data::Compress::Inflater::IsEnd() const
{
	return this->lastRes == InflateResult::StreamEnd;
}

Bool Data::Compress::Inflater::DecompressDirect(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff, Bool hasHeader)
{
	UInt32 decomp_flags = INFLATER_FLAG_COMPUTE_ADLER32;
	UOSInt in_bytes;
	UOSInt out_bytes;

	if (hasHeader)
		decomp_flags |= INFLATER_FLAG_PARSE_ZLIB_HEADER;
	in_bytes = srcBuff.GetSize();
	out_bytes = destBuff.GetSize();
	InflateDecompressor decomp;
	decomp.m_state = 0;
	decomp_flags |= INFLATER_FLAG_USING_NON_WRAPPING_OUTPUT_BUF;
	InflateStatus status = Decompress(decomp, srcBuff.Arr(), in_bytes, destBuff.Arr(), destBuff.Arr(), out_bytes, decomp_flags);
	if ((OSInt)status < 0)
		return false;
	else if (status != InflateStatus::Done)
	{
		return false;
	}
	outDestBuffSize.Set(out_bytes);
	return true;
}
