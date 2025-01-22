#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/Compress/Inflater.h"
#include "Math/Math.h"
#include <assert.h>

#include <stddef.h>
#include <stdint.h>

typedef unsigned char mz_uint8;
typedef int16_t mz_int16;
typedef uint16_t mz_uint16;
typedef uint32_t mz_uint32;
typedef uint32_t mz_uint;
typedef int64_t mz_int64;
typedef uint64_t mz_uint64;
typedef int mz_bool;


#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__i386) || defined(__i486__) || defined(__i486) || defined(i386) || defined(__ia64__) || defined(__x86_64__)
/* MINIZ_X86_OR_X64_CPU is only used to help set the below macros. */
#define MINIZ_X86_OR_X64_CPU 1
#else
#define MINIZ_X86_OR_X64_CPU 0
#endif

/* Set MINIZ_LITTLE_ENDIAN only if not set */
#if !defined(MINIZ_LITTLE_ENDIAN)
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)

#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
/* Set MINIZ_LITTLE_ENDIAN to 1 if the processor is little endian. */
#define MINIZ_LITTLE_ENDIAN 1
#else
#define MINIZ_LITTLE_ENDIAN 0
#endif

#else

#if MINIZ_X86_OR_X64_CPU
#define MINIZ_LITTLE_ENDIAN 1
#else
#define MINIZ_LITTLE_ENDIAN 0
#endif

#endif
#endif

/* Using unaligned loads and stores causes errors when using UBSan */
#if defined(__has_feature)
#if __has_feature(undefined_behavior_sanitizer)
#define MINIZ_USE_UNALIGNED_LOADS_AND_STORES 0
#endif
#endif

/* Set MINIZ_USE_UNALIGNED_LOADS_AND_STORES only if not set */
#if !defined(MINIZ_USE_UNALIGNED_LOADS_AND_STORES)
#if MINIZ_X86_OR_X64_CPU
/* Set MINIZ_USE_UNALIGNED_LOADS_AND_STORES to 1 on CPU's that permit efficient integer loads and stores from unaligned addresses. */
#define MINIZ_USE_UNALIGNED_LOADS_AND_STORES 0
#define MINIZ_UNALIGNED_USE_MEMCPY
#else
#define MINIZ_USE_UNALIGNED_LOADS_AND_STORES 0
#endif
#endif

#if defined(_M_X64) || defined(_WIN64) || defined(__MINGW64__) || defined(_LP64) || defined(__LP64__) || defined(__ia64__) || defined(__x86_64__)
/* Set MINIZ_HAS_64BIT_REGISTERS to 1 if operations on 64-bit integers are reasonably fast (and don't involve compiler generated calls to helper functions). */
#define MINIZ_HAS_64BIT_REGISTERS 1
#else
#define MINIZ_HAS_64BIT_REGISTERS 0
#endif

#define MZ_ASSERT(x) assert(x)

#define MZ_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MZ_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MZ_CLEAR_ARR(obj) memset((obj), 0, sizeof(obj))

#if MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN
#define MZ_READ_LE16(p) *((const mz_uint16 *)(p))
#define MZ_READ_LE32(p) *((const mz_uint32 *)(p))
#else
#define MZ_READ_LE16(p) ((mz_uint32)(((const mz_uint8 *)(p))[0]) | ((mz_uint32)(((const mz_uint8 *)(p))[1]) << 8U))
#define MZ_READ_LE32(p) ((mz_uint32)(((const mz_uint8 *)(p))[0]) | ((mz_uint32)(((const mz_uint8 *)(p))[1]) << 8U) | ((mz_uint32)(((const mz_uint8 *)(p))[2]) << 16U) | ((mz_uint32)(((const mz_uint8 *)(p))[3]) << 24U))
#endif

#ifdef _MSC_VER
#define MZ_MACRO_END while (0, 0)
#else
#define MZ_MACRO_END while (0)
#endif


enum
{
	TINFL_MAX_HUFF_TABLES = 3,
	TINFL_MAX_HUFF_SYMBOLS_0 = 288,
	TINFL_MAX_HUFF_SYMBOLS_1 = 32,
	TINFL_MAX_HUFF_SYMBOLS_2 = 19,
	TINFL_FAST_LOOKUP_BITS = 10,
	TINFL_FAST_LOOKUP_SIZE = 1 << TINFL_FAST_LOOKUP_BITS
};

#if MINIZ_HAS_64BIT_REGISTERS
#define TINFL_USE_64BIT_BITBUF 1
#else
#define TINFL_USE_64BIT_BITBUF 0
#endif

#if TINFL_USE_64BIT_BITBUF
    typedef mz_uint64 tinfl_bit_buf_t;
#define TINFL_BITBUF_SIZE (64)
#else
typedef mz_uint32 tinfl_bit_buf_t;
#define TINFL_BITBUF_SIZE (32)
#endif


#define TINFL_MEMCPY(d, s, l) memcpy(d, s, l)
#define TINFL_MEMSET(p, c, l) memset(p, c, l)

#define TINFL_CR_BEGIN  \
    switch (r->m_state) \
    {                   \
        case 0:
#define TINFL_CR_RETURN(state_index, result) \
    do                                       \
    {                                        \
        status = result;                     \
        r->m_state = state_index;            \
        goto common_exit;                    \
        case state_index:;                   \
    }                                        \
    MZ_MACRO_END
#define TINFL_CR_RETURN_FOREVER(state_index, result) \
    do                                               \
    {                                                \
        for (;;)                                     \
        {                                            \
            TINFL_CR_RETURN(state_index, result);    \
        }                                            \
    }                                                \
    MZ_MACRO_END
#define TINFL_CR_FINISH }

#define TINFL_GET_BYTE(state_index, c)                                                                                                                           \
    do                                                                                                                                                           \
    {                                                                                                                                                            \
        while (pIn_buf_cur >= pIn_buf_end)                                                                                                                       \
        {                                                                                                                                                        \
            TINFL_CR_RETURN(state_index, (decomp_flags & TINFL_FLAG_HAS_MORE_INPUT) ? TINFL_STATUS_NEEDS_MORE_INPUT : TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS); \
        }                                                                                                                                                        \
        c = *pIn_buf_cur++;                                                                                                                                      \
    }                                                                                                                                                            \
    MZ_MACRO_END

#define TINFL_NEED_BITS(state_index, n)                \
    do                                                 \
    {                                                  \
        mz_uint c;                                     \
        TINFL_GET_BYTE(state_index, c);                \
        bit_buf |= (((tinfl_bit_buf_t)c) << num_bits); \
        num_bits += 8;                                 \
    } while (num_bits < (mz_uint)(n))
#define TINFL_SKIP_BITS(state_index, n)      \
    do                                       \
    {                                        \
        if (num_bits < (mz_uint)(n))         \
        {                                    \
            TINFL_NEED_BITS(state_index, n); \
        }                                    \
        bit_buf >>= (n);                     \
        num_bits -= (n);                     \
    }                                        \
    MZ_MACRO_END
#define TINFL_GET_BITS(state_index, b, n)    \
    do                                       \
    {                                        \
        if (num_bits < (mz_uint)(n))         \
        {                                    \
            TINFL_NEED_BITS(state_index, n); \
        }                                    \
        b = bit_buf & ((1 << (n)) - 1);      \
        bit_buf >>= (n);                     \
        num_bits -= (n);                     \
    }                                        \
    MZ_MACRO_END

/* TINFL_HUFF_BITBUF_FILL() is only used rarely, when the number of bytes remaining in the input buffer falls below 2. */
/* It reads just enough bytes from the input stream that are needed to decode the next Huffman code (and absolutely no more). It works by trying to fully decode a */
/* Huffman code by using whatever bits are currently present in the bit buffer. If this fails, it reads another byte, and tries again until it succeeds or until the */
/* bit buffer contains >=15 bits (deflate's max. Huffman code size). */
#define TINFL_HUFF_BITBUF_FILL(state_index, pLookUp, pTree)          \
    do                                                               \
    {                                                                \
        temp = pLookUp[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)];      \
        if (temp >= 0)                                               \
        {                                                            \
            code_len = temp >> 9;                                    \
            if ((code_len) && (num_bits >= code_len))                \
                break;                                               \
        }                                                            \
        else if (num_bits > TINFL_FAST_LOOKUP_BITS)                  \
        {                                                            \
            code_len = TINFL_FAST_LOOKUP_BITS;                       \
            do                                                       \
            {                                                        \
                temp = pTree[~temp + ((bit_buf >> code_len++) & 1)]; \
            } while ((temp < 0) && (num_bits >= (code_len + 1)));    \
            if (temp >= 0)                                           \
                break;                                               \
        }                                                            \
        TINFL_GET_BYTE(state_index, c);                              \
        bit_buf |= (((tinfl_bit_buf_t)c) << num_bits);               \
        num_bits += 8;                                               \
    } while (num_bits < 15);

/* TINFL_HUFF_DECODE() decodes the next Huffman coded symbol. It's more complex than you would initially expect because the zlib API expects the decompressor to never read */
/* beyond the final byte of the deflate stream. (In other words, when this macro wants to read another byte from the input, it REALLY needs another byte in order to fully */
/* decode the next Huffman code.) Handling this properly is particularly important on raw deflate (non-zlib) streams, which aren't followed by a byte aligned adler-32. */
/* The slow path is only executed at the very end of the input buffer. */
/* v1.16: The original macro handled the case at the very end of the passed-in input buffer, but we also need to handle the case where the user passes in 1+zillion bytes */
/* following the deflate data and our non-conservative read-ahead path won't kick in here on this code. This is much trickier. */
#define TINFL_HUFF_DECODE(state_index, sym, pLookUp, pTree)                                                                         \
    do                                                                                                                              \
    {                                                                                                                               \
        int temp;                                                                                                                   \
        mz_uint code_len, c;                                                                                                        \
        if (num_bits < 15)                                                                                                          \
        {                                                                                                                           \
            if ((pIn_buf_end - pIn_buf_cur) < 2)                                                                                    \
            {                                                                                                                       \
                TINFL_HUFF_BITBUF_FILL(state_index, pLookUp, pTree);                                                                \
            }                                                                                                                       \
            else                                                                                                                    \
            {                                                                                                                       \
                bit_buf |= (((tinfl_bit_buf_t)pIn_buf_cur[0]) << num_bits) | (((tinfl_bit_buf_t)pIn_buf_cur[1]) << (num_bits + 8)); \
                pIn_buf_cur += 2;                                                                                                   \
                num_bits += 16;                                                                                                     \
            }                                                                                                                       \
        }                                                                                                                           \
        if ((temp = pLookUp[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]) >= 0)                                                          \
            code_len = temp >> 9, temp &= 511;                                                                                      \
        else                                                                                                                        \
        {                                                                                                                           \
            code_len = TINFL_FAST_LOOKUP_BITS;                                                                                      \
            do                                                                                                                      \
            {                                                                                                                       \
                temp = pTree[~temp + ((bit_buf >> code_len++) & 1)];                                                                \
            } while (temp < 0);                                                                                                     \
        }                                                                                                                           \
        sym = temp;                                                                                                                 \
        bit_buf >>= code_len;                                                                                                       \
        num_bits -= code_len;                                                                                                       \
    }                                                                                                                               \
    MZ_MACRO_END

enum
{
	TINFL_FLAG_PARSE_ZLIB_HEADER = 1,
	TINFL_FLAG_HAS_MORE_INPUT = 2,
	TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF = 4,
	TINFL_FLAG_COMPUTE_ADLER32 = 8
};

typedef enum
{
	/* This flags indicates the inflator needs 1 or more input bytes to make forward progress, but the caller is indicating that no more are available. The compressed data */
	/* is probably corrupted. If you call the inflator again with more bytes it'll try to continue processing the input but this is a BAD sign (either the data is corrupted or you called it incorrectly). */
	/* If you call it again with no input you'll just get TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS again. */
	TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS = -4,

	/* This flag indicates that one or more of the input parameters was obviously bogus. (You can try calling it again, but if you get this error the calling code is wrong.) */
	TINFL_STATUS_BAD_PARAM = -3,

	/* This flags indicate the inflator is finished but the adler32 check of the uncompressed data didn't match. If you call it again it'll return TINFL_STATUS_DONE. */
	TINFL_STATUS_ADLER32_MISMATCH = -2,

	/* This flags indicate the inflator has somehow failed (bad code, corrupted input, etc.). If you call it again without resetting via tinfl_init() it it'll just keep on returning the same status failure code. */
	TINFL_STATUS_FAILED = -1,

	/* Any status code less than TINFL_STATUS_DONE must indicate a failure. */

	/* This flag indicates the inflator has returned every byte of uncompressed data that it can, has consumed every byte that it needed, has successfully reached the end of the deflate stream, and */
	/* if zlib headers and adler32 checking enabled that it has successfully checked the uncompressed data's adler32. If you call it again you'll just get TINFL_STATUS_DONE over and over again. */
	TINFL_STATUS_DONE = 0,

	/* This flag indicates the inflator MUST have more input data (even 1 byte) before it can make any more forward progress, or you need to clear the TINFL_FLAG_HAS_MORE_INPUT */
	/* flag on the next call if you don't have any more source data. If the source data was somehow corrupted it's also possible (but unlikely) for the inflator to keep on demanding input to */
	/* proceed, so be sure to properly set the TINFL_FLAG_HAS_MORE_INPUT flag. */
	TINFL_STATUS_NEEDS_MORE_INPUT = 1,

	/* This flag indicates the inflator definitely has 1 or more bytes of uncompressed data available, but it cannot write this data into the output buffer. */
	/* Note if the source compressed data was corrupted it's possible for the inflator to return a lot of uncompressed data to the caller. I've been assuming you know how much uncompressed data to expect */
	/* (either exact or worst case) and will stop calling the inflator and fail after receiving too much. In pure streaming scenarios where you have no idea how many bytes to expect this may not be possible */
	/* so I may need to add some code to address this. */
	TINFL_STATUS_HAS_MORE_OUTPUT = 2
} tinfl_status;

struct tinfl_decompressor_tag
{
	mz_uint32 m_state, m_num_bits, m_zhdr0, m_zhdr1, m_z_adler32, m_final, m_type, m_check_adler32, m_dist, m_counter, m_num_extra, m_table_sizes[TINFL_MAX_HUFF_TABLES];
	tinfl_bit_buf_t m_bit_buf;
	size_t m_dist_from_out_buf_start;
	mz_int16 m_look_up[TINFL_MAX_HUFF_TABLES][TINFL_FAST_LOOKUP_SIZE];
	mz_int16 m_tree_0[TINFL_MAX_HUFF_SYMBOLS_0 * 2];
	mz_int16 m_tree_1[TINFL_MAX_HUFF_SYMBOLS_1 * 2];
	mz_int16 m_tree_2[TINFL_MAX_HUFF_SYMBOLS_2 * 2];
	mz_uint8 m_code_size_0[TINFL_MAX_HUFF_SYMBOLS_0];
	mz_uint8 m_code_size_1[TINFL_MAX_HUFF_SYMBOLS_1];
	mz_uint8 m_code_size_2[TINFL_MAX_HUFF_SYMBOLS_2];
	mz_uint8 m_raw_header[4], m_len_codes[TINFL_MAX_HUFF_SYMBOLS_0 + TINFL_MAX_HUFF_SYMBOLS_1 + 137];
};

struct tinfl_decompressor_tag;
typedef struct tinfl_decompressor_tag tinfl_decompressor;

static void test_clear_tree(tinfl_decompressor *r)
{
	if (r->m_type == 0)
		MZ_CLEAR_ARR(r->m_tree_0);
	else if (r->m_type == 1)
		MZ_CLEAR_ARR(r->m_tree_1);
	else
		MZ_CLEAR_ARR(r->m_tree_2);
}

tinfl_status test_decompress(tinfl_decompressor *r, const mz_uint8 *pIn_buf_next, size_t *pIn_buf_size, mz_uint8 *pOut_buf_start, mz_uint8 *pOut_buf_next, size_t *pOut_buf_size, const mz_uint32 decomp_flags)
{
	static const mz_uint16 s_length_base[31] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0 };
	static const mz_uint8 s_length_extra[31] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0 };
	static const mz_uint16 s_dist_base[32] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0, 0 };
	static const mz_uint8 s_dist_extra[32] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };
	static const mz_uint8 s_length_dezigzag[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
	static const mz_uint16 s_min_table_sizes[3] = { 257, 1, 4 };

	mz_int16 *pTrees[3];
	mz_uint8 *pCode_sizes[3];

	tinfl_status status = TINFL_STATUS_FAILED;
	mz_uint32 num_bits, dist, counter, num_extra;
	tinfl_bit_buf_t bit_buf;
	const mz_uint8 *pIn_buf_cur = pIn_buf_next, *const pIn_buf_end = pIn_buf_next + *pIn_buf_size;
	mz_uint8 *pOut_buf_cur = pOut_buf_next, *const pOut_buf_end = pOut_buf_next ? pOut_buf_next + *pOut_buf_size : NULL;
	size_t out_buf_size_mask = (decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF) ? (size_t)-1 : ((pOut_buf_next - pOut_buf_start) + *pOut_buf_size) - 1, dist_from_out_buf_start;

	/* Ensure the output buffer's size is a power of 2, unless the output buffer is large enough to hold the entire output file (in which case it doesn't matter). */
	if (((out_buf_size_mask + 1) & out_buf_size_mask) || (pOut_buf_next < pOut_buf_start))
	{
		*pIn_buf_size = *pOut_buf_size = 0;
		return TINFL_STATUS_BAD_PARAM;
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
	TINFL_CR_BEGIN

	bit_buf = num_bits = dist = counter = num_extra = r->m_zhdr0 = r->m_zhdr1 = 0;
	r->m_z_adler32 = r->m_check_adler32 = 1;
	if (decomp_flags & TINFL_FLAG_PARSE_ZLIB_HEADER)
	{
		TINFL_GET_BYTE(1, r->m_zhdr0);
		TINFL_GET_BYTE(2, r->m_zhdr1);
		counter = (((r->m_zhdr0 * 256 + r->m_zhdr1) % 31 != 0) || (r->m_zhdr1 & 32) || ((r->m_zhdr0 & 15) != 8));
		if (!(decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF))
			counter |= (((1U << (8U + (r->m_zhdr0 >> 4))) > 32768U) || ((out_buf_size_mask + 1) < (size_t)((size_t)1 << (8U + (r->m_zhdr0 >> 4)))));
		if (counter)
		{
			TINFL_CR_RETURN_FOREVER(36, TINFL_STATUS_FAILED);
		}
	}

	do
	{
		TINFL_GET_BITS(3, r->m_final, 3);
		r->m_type = r->m_final >> 1;
		if (r->m_type == 0)
		{
			TINFL_SKIP_BITS(5, num_bits & 7);
			for (counter = 0; counter < 4; ++counter)
			{
				if (num_bits)
					TINFL_GET_BITS(6, r->m_raw_header[counter], 8);
				else
					TINFL_GET_BYTE(7, r->m_raw_header[counter]);
			}
			if ((counter = (r->m_raw_header[0] | (r->m_raw_header[1] << 8))) != (mz_uint)(0xFFFF ^ (r->m_raw_header[2] | (r->m_raw_header[3] << 8))))
			{
				TINFL_CR_RETURN_FOREVER(39, TINFL_STATUS_FAILED);
			}
			while ((counter) && (num_bits))
			{
				TINFL_GET_BITS(51, dist, 8);
				while (pOut_buf_cur >= pOut_buf_end)
				{
					TINFL_CR_RETURN(52, TINFL_STATUS_HAS_MORE_OUTPUT);
				}
				*pOut_buf_cur++ = (mz_uint8)dist;
				counter--;
			}
			while (counter)
			{
				size_t n;
				while (pOut_buf_cur >= pOut_buf_end)
				{
					TINFL_CR_RETURN(9, TINFL_STATUS_HAS_MORE_OUTPUT);
				}
				while (pIn_buf_cur >= pIn_buf_end)
				{
					TINFL_CR_RETURN(38, (decomp_flags & TINFL_FLAG_HAS_MORE_INPUT) ? TINFL_STATUS_NEEDS_MORE_INPUT : TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS);
				}
				n = MZ_MIN(MZ_MIN((size_t)(pOut_buf_end - pOut_buf_cur), (size_t)(pIn_buf_end - pIn_buf_cur)), counter);
				TINFL_MEMCPY(pOut_buf_cur, pIn_buf_cur, n);
				pIn_buf_cur += n;
				pOut_buf_cur += n;
				counter -= (mz_uint)n;
			}
		}
		else if (r->m_type == 3)
		{
			TINFL_CR_RETURN_FOREVER(10, TINFL_STATUS_FAILED);
		}
		else
		{
			if (r->m_type == 1)
			{
				mz_uint8 *p = r->m_code_size_0;
				mz_uint i;
				r->m_table_sizes[0] = 288;
				r->m_table_sizes[1] = 32;
				TINFL_MEMSET(r->m_code_size_1, 5, 32);
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
					TINFL_GET_BITS(11, r->m_table_sizes[counter], "\05\05\04"[counter]);
					r->m_table_sizes[counter] += s_min_table_sizes[counter];
				}
				MZ_CLEAR_ARR(r->m_code_size_2);
				for (counter = 0; counter < r->m_table_sizes[2]; counter++)
				{
					mz_uint s;
					TINFL_GET_BITS(14, s, 3);
					r->m_code_size_2[s_length_dezigzag[counter]] = (mz_uint8)s;
				}
				r->m_table_sizes[2] = 19;
			}
			for (; (int)r->m_type >= 0; r->m_type--)
			{
				int tree_next, tree_cur;
				mz_int16 *pLookUp;
				mz_int16 *pTree;
				mz_uint8 *pCode_size;
				mz_uint i, j, used_syms, total, sym_index, next_code[17], total_syms[16];
				pLookUp = r->m_look_up[r->m_type];
				pTree = pTrees[r->m_type];
				pCode_size = pCode_sizes[r->m_type];
				MZ_CLEAR_ARR(total_syms);
				TINFL_MEMSET(pLookUp, 0, sizeof(r->m_look_up[0]));
				test_clear_tree(r);
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
					TINFL_CR_RETURN_FOREVER(35, TINFL_STATUS_FAILED);
				}
				for (tree_next = -1, sym_index = 0; sym_index < r->m_table_sizes[r->m_type]; ++sym_index)
				{
					mz_uint rev_code = 0, l, cur_code, code_size = pCode_size[sym_index];
					if (!code_size)
						continue;
					cur_code = next_code[code_size]++;
					for (l = code_size; l > 0; l--, cur_code >>= 1)
						rev_code = (rev_code << 1) | (cur_code & 1);
					if (code_size <= TINFL_FAST_LOOKUP_BITS)
					{
						mz_int16 k = (mz_int16)((code_size << 9) | sym_index);
						while (rev_code < TINFL_FAST_LOOKUP_SIZE)
						{
							pLookUp[rev_code] = k;
							rev_code += (1 << code_size);
						}
						continue;
					}
					if (0 == (tree_cur = pLookUp[rev_code & (TINFL_FAST_LOOKUP_SIZE - 1)]))
					{
						pLookUp[rev_code & (TINFL_FAST_LOOKUP_SIZE - 1)] = (mz_int16)tree_next;
						tree_cur = tree_next;
						tree_next -= 2;
					}
					rev_code >>= (TINFL_FAST_LOOKUP_BITS - 1);
					for (j = code_size; j > (TINFL_FAST_LOOKUP_BITS + 1); j--)
					{
						tree_cur -= ((rev_code >>= 1) & 1);
						if (!pTree[-tree_cur - 1])
						{
							pTree[-tree_cur - 1] = (mz_int16)tree_next;
							tree_cur = tree_next;
							tree_next -= 2;
						}
						else
							tree_cur = pTree[-tree_cur - 1];
					}
					tree_cur -= ((rev_code >>= 1) & 1);
					pTree[-tree_cur - 1] = (mz_int16)sym_index;
				}
				if (r->m_type == 2)
				{
					for (counter = 0; counter < (r->m_table_sizes[0] + r->m_table_sizes[1]);)
					{
						mz_uint s;
						TINFL_HUFF_DECODE(16, dist, r->m_look_up[2], r->m_tree_2);
						if (dist < 16)
						{
							r->m_len_codes[counter++] = (mz_uint8)dist;
							continue;
						}
						if ((dist == 16) && (!counter))
						{
							TINFL_CR_RETURN_FOREVER(17, TINFL_STATUS_FAILED);
						}
						num_extra = "\02\03\07"[dist - 16];
						TINFL_GET_BITS(18, s, num_extra);
						s += "\03\03\013"[dist - 16];
						TINFL_MEMSET(r->m_len_codes + counter, (dist == 16) ? r->m_len_codes[counter - 1] : 0, s);
						counter += s;
					}
					if ((r->m_table_sizes[0] + r->m_table_sizes[1]) != counter)
					{
						TINFL_CR_RETURN_FOREVER(21, TINFL_STATUS_FAILED);
					}
					TINFL_MEMCPY(r->m_code_size_0, r->m_len_codes, r->m_table_sizes[0]);
					TINFL_MEMCPY(r->m_code_size_1, r->m_len_codes + r->m_table_sizes[0], r->m_table_sizes[1]);
				}
			}
			for (;;)
			{
				mz_uint8 *pSrc;
				for (;;)
				{
					if (((pIn_buf_end - pIn_buf_cur) < 4) || ((pOut_buf_end - pOut_buf_cur) < 2))
					{
						TINFL_HUFF_DECODE(23, counter, r->m_look_up[0], r->m_tree_0);
						if (counter >= 256)
							break;
						while (pOut_buf_cur >= pOut_buf_end)
						{
							TINFL_CR_RETURN(24, TINFL_STATUS_HAS_MORE_OUTPUT);
						}
						*pOut_buf_cur++ = (mz_uint8)counter;
					}
					else
					{
						int sym2;
						mz_uint code_len;
#if TINFL_USE_64BIT_BITBUF
						if (num_bits < 30)
						{
							bit_buf |= (((tinfl_bit_buf_t)MZ_READ_LE32(pIn_buf_cur)) << num_bits);
							pIn_buf_cur += 4;
							num_bits += 32;
						}
#else
					if (num_bits < 15)
					{
						bit_buf |= (((tinfl_bit_buf_t)MZ_READ_LE16(pIn_buf_cur)) << num_bits);
						pIn_buf_cur += 2;
						num_bits += 16;
					}
#endif
						if ((sym2 = r->m_look_up[0][bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]) >= 0)
							code_len = sym2 >> 9;
						else
						{
							code_len = TINFL_FAST_LOOKUP_BITS;
							do
							{
								sym2 = r->m_tree_0[~sym2 + ((bit_buf >> code_len++) & 1)];
							} while (sym2 < 0);
						}
						counter = sym2;
						bit_buf >>= code_len;
						num_bits -= code_len;
						if (counter & 256)
							break;

#if !TINFL_USE_64BIT_BITBUF
						if (num_bits < 15)
						{
							bit_buf |= (((tinfl_bit_buf_t)MZ_READ_LE16(pIn_buf_cur)) << num_bits);
							pIn_buf_cur += 2;
							num_bits += 16;
						}
#endif
						if ((sym2 = r->m_look_up[0][bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]) >= 0)
							code_len = sym2 >> 9;
						else
						{
							code_len = TINFL_FAST_LOOKUP_BITS;
							do
							{
								sym2 = r->m_tree_0[~sym2 + ((bit_buf >> code_len++) & 1)];
							} while (sym2 < 0);
						}
						bit_buf >>= code_len;
						num_bits -= code_len;

						pOut_buf_cur[0] = (mz_uint8)counter;
						if (sym2 & 256)
						{
							pOut_buf_cur++;
							counter = sym2;
							break;
						}
						pOut_buf_cur[1] = (mz_uint8)sym2;
						pOut_buf_cur += 2;
					}
				}
				if ((counter &= 511) == 256)
					break;

				num_extra = s_length_extra[counter - 257];
				counter = s_length_base[counter - 257];
				if (num_extra)
				{
					mz_uint extra_bits;
					TINFL_GET_BITS(25, extra_bits, num_extra);
					counter += extra_bits;
				}

				TINFL_HUFF_DECODE(26, dist, r->m_look_up[1], r->m_tree_1);
				num_extra = s_dist_extra[dist];
				dist = s_dist_base[dist];
				if (num_extra)
				{
					mz_uint extra_bits;
					TINFL_GET_BITS(27, extra_bits, num_extra);
					dist += extra_bits;
				}

				dist_from_out_buf_start = pOut_buf_cur - pOut_buf_start;
				if ((dist == 0 || dist > dist_from_out_buf_start || dist_from_out_buf_start == 0) && (decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF))
				{
					TINFL_CR_RETURN_FOREVER(37, TINFL_STATUS_FAILED);
				}

				pSrc = pOut_buf_start + ((dist_from_out_buf_start - dist) & out_buf_size_mask);

				if ((MZ_MAX(pOut_buf_cur, pSrc) + counter) > pOut_buf_end)
				{
					while (counter--)
					{
						while (pOut_buf_cur >= pOut_buf_end)
						{
							TINFL_CR_RETURN(53, TINFL_STATUS_HAS_MORE_OUTPUT);
						}
						*pOut_buf_cur++ = pOut_buf_start[(dist_from_out_buf_start++ - dist) & out_buf_size_mask];
					}
					continue;
				}
#if MINIZ_USE_UNALIGNED_LOADS_AND_STORES
				else if ((counter >= 9) && (counter <= dist))
				{
					const mz_uint8 *pSrc_end = pSrc + (counter & ~7);
					do
					{
#ifdef MINIZ_UNALIGNED_USE_MEMCPY
						memcpy(pOut_buf_cur, pSrc, sizeof(mz_uint32) * 2);
#else
						((mz_uint32 *)pOut_buf_cur)[0] = ((const mz_uint32 *)pSrc)[0];
						((mz_uint32 *)pOut_buf_cur)[1] = ((const mz_uint32 *)pSrc)[1];
#endif
						pOut_buf_cur += 8;
					} while ((pSrc += 8) < pSrc_end);
					if ((counter &= 7) < 3)
					{
						if (counter)
						{
							pOut_buf_cur[0] = pSrc[0];
							if (counter > 1)
								pOut_buf_cur[1] = pSrc[1];
							pOut_buf_cur += counter;
						}
						continue;
					}
				}
#endif
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
	TINFL_SKIP_BITS(32, num_bits & 7);
	while ((pIn_buf_cur > pIn_buf_next) && (num_bits >= 8))
	{
		--pIn_buf_cur;
		num_bits -= 8;
	}
	bit_buf &= ~(~(tinfl_bit_buf_t)0 << num_bits);
	MZ_ASSERT(!num_bits); /* if this assert fires then we've read beyond the end of non-deflate/zlib streams with following data (such as gzip streams). */

	if (decomp_flags & TINFL_FLAG_PARSE_ZLIB_HEADER)
	{
		for (counter = 0; counter < 4; ++counter)
		{
			mz_uint s;
			if (num_bits)
				TINFL_GET_BITS(41, s, 8);
			else
				TINFL_GET_BYTE(42, s);
			r->m_z_adler32 = (r->m_z_adler32 << 8) | s;
		}
	}
	TINFL_CR_RETURN_FOREVER(34, TINFL_STATUS_DONE);

	TINFL_CR_FINISH

common_exit:
	/* As long as we aren't telling the caller that we NEED more input to make forward progress: */
	/* Put back any bytes from the bitbuf in case we've looked ahead too far on gzip, or other Deflate streams followed by arbitrary data. */
	/* We need to be very careful here to NOT push back any bytes we definitely know we need to make forward progress, though, or we'll lock the caller up into an inf loop. */
	if ((status != TINFL_STATUS_NEEDS_MORE_INPUT) && (status != TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS))
	{
		while ((pIn_buf_cur > pIn_buf_next) && (num_bits >= 8))
		{
			--pIn_buf_cur;
			num_bits -= 8;
		}
	}
	r->m_num_bits = num_bits;
	r->m_bit_buf = bit_buf & ~(~(tinfl_bit_buf_t)0 << num_bits);
	r->m_dist = dist;
	r->m_counter = counter;
	r->m_num_extra = num_extra;
	r->m_dist_from_out_buf_start = dist_from_out_buf_start;
	*pIn_buf_size = pIn_buf_cur - pIn_buf_next;
	*pOut_buf_size = pOut_buf_cur - pOut_buf_next;
	if ((decomp_flags & (TINFL_FLAG_PARSE_ZLIB_HEADER | TINFL_FLAG_COMPUTE_ADLER32)) && (status >= 0))
	{
		const mz_uint8 *ptr = pOut_buf_next;
		size_t buf_len = *pOut_buf_size;
		mz_uint32 i, s1 = r->m_check_adler32 & 0xffff, s2 = r->m_check_adler32 >> 16;
		size_t block_len = buf_len % 5552;
		while (buf_len)
		{
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
			block_len = 5552;
		}
		r->m_check_adler32 = (s2 << 16) + s1;
		if ((status == TINFL_STATUS_DONE) && (decomp_flags & TINFL_FLAG_PARSE_ZLIB_HEADER) && (r->m_check_adler32 != r->m_z_adler32))
			status = TINFL_STATUS_ADLER32_MISMATCH;
	}
	return status;
}

#define INFLATER_BUFFSIZE 1048576

#define INFLATER_CLEAR_ARR(arr) MemClear(arr, sizeof(arr))
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
            INFLATER_CR_RETURN(state_index, (decomp_flags & INFLATER_FLAG_HAS_MORE_INPUT) ? InflateStatus::NeedsMoreInput : InflateStatus::FailedCannotMakeProgress); \
        }                                                                                                                                                        \
        c = *pIn_buf_cur++;                                                                                                                                      \
    }

#define INFLATER_NEED_BITS(state_index, n)                \
    do                                                 \
    {                                                  \
        UOSInt c;                                     \
        INFLATER_GET_BYTE(state_index, c);                \
        bit_buf |= (((UOSInt)c) << num_bits); \
        num_bits += 8;                                 \
    } while (num_bits < (UOSInt)(n))

#define INFLATER_SKIP_BITS(state_index, n)      \
    {                                        \
        if (num_bits < (UOSInt)(n))         \
        {                                    \
            INFLATER_NEED_BITS(state_index, n); \
        }                                    \
        bit_buf >>= (n);                     \
        num_bits -= (n);                     \
    }

#define INFLATER_GET_BITS(state_index, b, n)    \
    {                                        \
        if (num_bits < (UOSInt)(n))         \
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
            code_len = temp >> 9;                                    \
            if ((code_len) && (num_bits >= code_len))                \
                break;                                               \
        }                                                            \
        else if (num_bits > INFLATER_FAST_LOOKUP_BITS)                  \
        {                                                            \
            code_len = INFLATER_FAST_LOOKUP_BITS;                       \
            do                                                       \
            {                                                        \
                temp = pTree[~temp + ((bit_buf >> code_len++) & 1)]; \
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
        UOSInt temp;                                                                                                                   \
        UOSInt code_len, c;                                                                                                        \
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
            code_len = temp >> 9, temp &= 511;                                                                                      \
        else                                                                                                                        \
        {                                                                                                                           \
            code_len = INFLATER_FAST_LOOKUP_BITS;                                                                                      \
            do                                                                                                                      \
            {                                                                                                                       \
                temp = pTree[~temp + ((bit_buf >> code_len++) & 1)];                                                                \
            } while (temp < 0);                                                                                                     \
        }                                                                                                                           \
        sym = (UInt32)temp;                                                                                                                 \
        bit_buf >>= code_len;                                                                                                       \
        num_bits -= (UInt32)code_len;                                                                                                       \
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
			OSInt m_dist_from_out_buf_start;
			UInt16 m_look_up[INFLATER_MAX_HUFF_TABLES][INFLATER_FAST_LOOKUP_SIZE];
			UInt16 m_tree_0[INFLATER_MAX_HUFF_SYMBOLS_0 * 2];
			UInt16 m_tree_1[INFLATER_MAX_HUFF_SYMBOLS_1 * 2];
			UInt16 m_tree_2[INFLATER_MAX_HUFF_SYMBOLS_2 * 2];
			UInt8 m_code_size_0[INFLATER_MAX_HUFF_SYMBOLS_0];
			UInt8 m_code_size_1[INFLATER_MAX_HUFF_SYMBOLS_1];
			UInt8 m_code_size_2[INFLATER_MAX_HUFF_SYMBOLS_2];
			UInt8 m_raw_header[4];
			UInt8 m_len_codes[INFLATER_MAX_HUFF_SYMBOLS_0 + INFLATER_MAX_HUFF_SYMBOLS_1 + 137];
		};

		struct InflateState
		{
//			InflateDecompressor m_decomp;
//			InflateStatus m_last_status;
			tinfl_decompressor m_decomp;
			tinfl_status m_last_status;
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

	UInt16 *pTrees[3];
	UInt8 *pCode_sizes[3];

	InflateStatus status = InflateStatus::Failed;
	UInt32 num_bits, dist, counter, num_extra;
	UOSInt bit_buf;
	UnsafeArray<const UInt8> pIn_buf_cur = pIn_buf_next;
	UnsafeArray<const UInt8> pIn_buf_end = pIn_buf_next + pIn_buf_size.Get();
	UnsafeArray<UInt8> pOut_buf_cur = pOut_buf_next;
	UnsafeArray<const UInt8> pOut_buf_end = pOut_buf_next + pOut_buf_size.Get();
	OSInt out_buf_size_mask = (decomp_flags & INFLATER_FLAG_USING_NON_WRAPPING_OUTPUT_BUF) ? (OSInt)-1 : ((pOut_buf_next - pOut_buf_start) + (OSInt)pOut_buf_size.Get()) - 1, dist_from_out_buf_start;

	if (((out_buf_size_mask + 1) & out_buf_size_mask) || (pOut_buf_next < pOut_buf_start))
	{
		pOut_buf_size.Set(0);
		pIn_buf_size.Set(0);
		return InflateStatus::BadParam;
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
			counter |= (((1U << (8U + (r->m_zhdr0 >> 4))) > 32768U) || ((out_buf_size_mask + 1) < (OSInt)((OSInt)1 << (8U + (r->m_zhdr0 >> 4)))));
		if (counter)
		{
			INFLATER_CR_RETURN_FOREVER(36, InflateStatus::Failed);
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
			if ((counter = (r->m_raw_header[0] | (r->m_raw_header[1] << 8))) != (UOSInt)(0xFFFF ^ (r->m_raw_header[2] | (r->m_raw_header[3] << 8))))
			{
				INFLATER_CR_RETURN_FOREVER(39, InflateStatus::Failed);
			}
			while ((counter) && (num_bits))
			{
				INFLATER_GET_BITS(51, dist, 8);
				while (UnsafeArray<const UInt8>(pOut_buf_cur) >= pOut_buf_end)
				{
					INFLATER_CR_RETURN(52, InflateStatus::HasMoreOutput);
				}
				*pOut_buf_cur++ = (UInt8)dist;
				counter--;
			}
			while (counter)
			{
				UOSInt n;
				while (UnsafeArray<const UInt8>(pOut_buf_cur) >= pOut_buf_end)
				{
					INFLATER_CR_RETURN(9, InflateStatus::HasMoreOutput);
				}
				while (pIn_buf_cur >= pIn_buf_end)
				{
					INFLATER_CR_RETURN(38, (decomp_flags & INFLATER_FLAG_HAS_MORE_INPUT) ? InflateStatus::NeedsMoreInput : InflateStatus::FailedCannotMakeProgress);
				}
				n = Math_Min(Math_Min((UOSInt)(pOut_buf_end - pOut_buf_cur), (UOSInt)(pIn_buf_end - pIn_buf_cur)), counter);
				MemCopyNO(pOut_buf_cur.Ptr(), pIn_buf_cur.Ptr(), n);
				pIn_buf_cur += n;
				pOut_buf_cur += n;
				counter -= (UInt32)n;
			}
		}
		else if (r->m_type == 3)
		{
			INFLATER_CR_RETURN_FOREVER(10, InflateStatus::Failed);
		}
		else
		{
			if (r->m_type == 1)
			{
				UInt8 *p = r->m_code_size_0;
				UOSInt i;
				r->m_table_sizes[0] = 288;
				r->m_table_sizes[1] = 32;
				MemFillB(r->m_code_size_1, 32, 5);
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
					UOSInt s;
					INFLATER_GET_BITS(14, s, 3);
					r->m_code_size_2[s_length_dezigzag[counter]] = (UInt8)s;
				}
				r->m_table_sizes[2] = 19;
			}
			for (; (int)r->m_type >= 0; r->m_type--)
			{
				OSInt tree_next, tree_cur;
				UInt16 *pLookUp;
				UInt16 *pTree;
				UInt8 *pCode_size;
				UOSInt i, j, used_syms, total, sym_index, next_code[17], total_syms[16];
				pLookUp = r->m_look_up[r->m_type];
				pTree = pTrees[r->m_type];
				pCode_size = pCode_sizes[r->m_type];
				INFLATER_CLEAR_ARR(total_syms);
				MemClear(pLookUp, sizeof(r->m_look_up[0]));
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
					INFLATER_CR_RETURN_FOREVER(35, InflateStatus::Failed);
				}
				for (tree_next = -1, sym_index = 0; sym_index < r->m_table_sizes[r->m_type]; ++sym_index)
				{
					UOSInt rev_code = 0, l, cur_code, code_size = pCode_size[sym_index];
					if (!code_size)
						continue;
					cur_code = next_code[code_size]++;
					for (l = code_size; l > 0; l--, cur_code >>= 1)
						rev_code = (rev_code << 1) | (cur_code & 1);
					if (code_size <= INFLATER_FAST_LOOKUP_BITS)
					{
						UInt16 k = (UInt16)((code_size << 9) | sym_index);
						while (rev_code < INFLATER_FAST_LOOKUP_SIZE)
						{
							pLookUp[rev_code] = k;
							rev_code += (1 << code_size);
						}
						continue;
					}
					if (0 == (tree_cur = pLookUp[rev_code & (INFLATER_FAST_LOOKUP_SIZE - 1)]))
					{
						pLookUp[rev_code & (INFLATER_FAST_LOOKUP_SIZE - 1)] = (UInt16)tree_next;
						tree_cur = tree_next;
						tree_next -= 2;
					}
					rev_code >>= (INFLATER_FAST_LOOKUP_BITS - 1);
					for (j = code_size; j > (INFLATER_FAST_LOOKUP_BITS + 1); j--)
					{
						tree_cur -= (OSInt)((rev_code >>= 1) & 1);
						if (!pTree[-tree_cur - 1])
						{
							pTree[-tree_cur - 1] = (UInt16)tree_next;
							tree_cur = tree_next;
							tree_next -= 2;
						}
						else
							tree_cur = pTree[-tree_cur - 1];
					}
					tree_cur -= (OSInt)((rev_code >>= 1) & 1);
					pTree[-tree_cur - 1] = (UInt16)sym_index;
				}
				if (r->m_type == 2)
				{
					for (counter = 0; counter < (r->m_table_sizes[0] + r->m_table_sizes[1]);)
					{
						UOSInt s;
						INFLATER_HUFF_DECODE(16, dist, r->m_look_up[2], r->m_tree_2);
						if (dist < 16)
						{
							r->m_len_codes[counter++] = (UInt8)dist;
							continue;
						}
						if ((dist == 16) && (!counter))
						{
							INFLATER_CR_RETURN_FOREVER(17, InflateStatus::Failed);
						}
						num_extra = (UInt32)("\02\03\07"[dist - 16]);
						INFLATER_GET_BITS(18, s, num_extra);
						s += (UOSInt)("\03\03\013"[dist - 16]);
						MemFillB(r->m_len_codes + counter, s, (dist == 16) ? r->m_len_codes[counter - 1] : 0);
						counter += (UInt32)s;
					}
					if ((r->m_table_sizes[0] + r->m_table_sizes[1]) != counter)
					{
						INFLATER_CR_RETURN_FOREVER(21, InflateStatus::Failed);
					}
					MemCopyNO(r->m_code_size_0, r->m_len_codes, r->m_table_sizes[0]);
					MemCopyNO(r->m_code_size_1, r->m_len_codes + r->m_table_sizes[0], r->m_table_sizes[1]);
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
							INFLATER_CR_RETURN(24, InflateStatus::HasMoreOutput);
						}
						*pOut_buf_cur++ = (UInt8)counter;
					}
					else
					{
						Int16 sym2;
						UOSInt code_len;
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
						bit_buf |= (((UOSInt)ReadUInt16(pIn_buf_cur)) << num_bits);
						pIn_buf_cur += 2;
						num_bits += 16;
					}
#endif
						if ((sym2 = (Int16)r->m_look_up[0][bit_buf & (INFLATER_FAST_LOOKUP_SIZE - 1)]) >= 0)
							code_len = (UOSInt)sym2 >> 9;
						else
						{
							code_len = INFLATER_FAST_LOOKUP_BITS;
							do
							{
								sym2 = (Int16)r->m_tree_0[~sym2 + (OSInt)((bit_buf >> code_len++) & 1)];
							} while (sym2 < 0);
						}
						counter = (UInt16)sym2;
						bit_buf >>= code_len;
						num_bits -= (UInt32)code_len;
						if (counter & 256)
							break;

#if _OSINT_SIZE != 64
						if (num_bits < 15)
						{
							bit_buf |= (((UOSInt)ReadUInt16(pIn_buf_cur)) << num_bits);
							pIn_buf_cur += 2;
							num_bits += 16;
						}
#endif
						if ((sym2 = (Int16)r->m_look_up[0][bit_buf & (INFLATER_FAST_LOOKUP_SIZE - 1)]) >= 0)
							code_len = (UOSInt)sym2 >> 9;
						else
						{
							code_len = INFLATER_FAST_LOOKUP_BITS;
							do
							{
								sym2 = (Int16)r->m_tree_0[~sym2 + (OSInt)((bit_buf >> code_len++) & 1)];
							} while (sym2 < 0);
						}
						bit_buf >>= code_len;
						num_bits -= (UInt32)code_len;

						pOut_buf_cur[0] = (UInt8)counter;
						if (sym2 & 256)
						{
							pOut_buf_cur++;
							counter = (UInt16)sym2;
							break;
						}
						pOut_buf_cur[1] = (UInt8)sym2;
						pOut_buf_cur += 2;
					}
				}
				if ((counter &= 511) == 256)
					break;

//				printf("cnt: %d\r\n", counter);
				num_extra = s_length_extra[counter - 257];
				counter = s_length_base[counter - 257];
				if (num_extra)
				{
					UInt32 extra_bits;
					INFLATER_GET_BITS(25, extra_bits, num_extra);
//					printf("a: %d, %d, %d\r\n", extra_bits, num_extra, counter);
					counter += (UInt32)extra_bits;
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

				dist_from_out_buf_start = pOut_buf_cur - pOut_buf_start;
				if ((dist == 0 || dist > dist_from_out_buf_start || dist_from_out_buf_start == 0) && (decomp_flags & INFLATER_FLAG_USING_NON_WRAPPING_OUTPUT_BUF))
				{
					INFLATER_CR_RETURN_FOREVER(37, InflateStatus::Failed);
				}

				pSrc = pOut_buf_start + ((dist_from_out_buf_start - dist) & out_buf_size_mask);

				if (UnsafeArray<const UInt8>(Math_Max(pOut_buf_cur, pSrc) + counter) > pOut_buf_end)
				{
					while (counter--)
					{
						while (UnsafeArray<const UInt8>(pOut_buf_cur) >= pOut_buf_end)
						{
							INFLATER_CR_RETURN(53, InflateStatus::HasMoreOutput);
						}
						*pOut_buf_cur++ = pOut_buf_start[(dist_from_out_buf_start++ - dist) & out_buf_size_mask];
					}
					continue;
				}
#if MINIZ_USE_UNALIGNED_LOADS_AND_STORES
				else if ((counter >= 9) && (counter <= dist))
				{
					const mz_uint8 *pSrc_end = pSrc + (counter & ~7);
					do
					{
#ifdef MINIZ_UNALIGNED_USE_MEMCPY
						memcpy(pOut_buf_cur, pSrc, sizeof(mz_uint32) * 2);
#else
						((mz_uint32 *)pOut_buf_cur)[0] = ((const mz_uint32 *)pSrc)[0];
						((mz_uint32 *)pOut_buf_cur)[1] = ((const mz_uint32 *)pSrc)[1];
#endif
						pOut_buf_cur += 8;
					} while ((pSrc += 8) < pSrc_end);
					if ((counter &= 7) < 3)
					{
						if (counter)
						{
							pOut_buf_cur[0] = pSrc[0];
							if (counter > 1)
								pOut_buf_cur[1] = pSrc[1];
							pOut_buf_cur += counter;
						}
						continue;
					}
				}
#endif
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
	INFLATER_CR_RETURN_FOREVER(34, InflateStatus::Done);

	INFLATER_CR_FINISH

common_exit:
	if ((status != InflateStatus::NeedsMoreInput) && (status != InflateStatus::FailedCannotMakeProgress))
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
		UOSInt block_len = buf_len % 5552;
		while (buf_len)
		{
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
			block_len = 5552;
		}
		r->m_check_adler32 = (s2 << 16) + s1;
		if ((status == InflateStatus::Done) && (decomp_flags & INFLATER_FLAG_PARSE_ZLIB_HEADER) && (r->m_check_adler32 != r->m_z_adler32))
			status = InflateStatus::Adler32Mismatch;
	}
	return status;
}

//#define STATUS_NEEDSMOREINPUT InflateStatus::NeedsMoreInput
//#define STATUS_DONE InflateStatus::Done
#define STATUS_NEEDSMOREINPUT TINFL_STATUS_NEEDS_MORE_INPUT
#define STATUS_DONE TINFL_STATUS_DONE

Data::Compress::InflateResult Data::Compress::Inflater::Inflate(Bool isEnd)
{
	NN<InflateState> pState;
	UInt32 n;
	UInt32 decomp_flags = INFLATER_FLAG_COMPUTE_ADLER32;
//	UOSInt in_bytes;
//	UOSInt out_bytes;
	size_t in_bytes;
	size_t out_bytes;
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

//		InflateStatus status = Decompress(pState->m_decomp, this->next_in, in_bytes, pState->m_dict, pState->m_dict + pState->m_dict_ofs, out_bytes, decomp_flags);
		tinfl_status status = test_decompress(&pState->m_decomp, this->next_in.Ptr(), &in_bytes, pState->m_dict, pState->m_dict + pState->m_dict_ofs, &out_bytes, decomp_flags);
		pState->m_last_status = status;

		this->next_in += (UInt32)in_bytes;
		this->avail_in -= in_bytes;

		n = (UInt32)out_bytes;
		this->decStm->Write(Data::ByteArrayR(pState->m_dict + pState->m_dict_ofs, n));
		pState->m_dict_ofs = (pState->m_dict_ofs + n) & (INFLATER_LZ_DICT_SIZE - 1);

		if ((OSInt)status < 0)
			return InflateResult::DataError;
		else if ((status == STATUS_NEEDSMOREINPUT) && (!orig_avail_in))
			return InflateResult::BufError;
		else if (isEnd)
		{
			if (status == STATUS_DONE)
				return InflateResult::StreamEnd;
		}
		else if ((status == STATUS_DONE) || (!this->avail_in))
			return (status == STATUS_DONE) ? InflateResult::StreamEnd : InflateResult::Ok;
	}
}

Data::Compress::Inflater::Inflater(NN<IO::Stream> decStm, Bool hasHeader) : Stream(CSTR("Inflater"))
{
	this->decStm = decStm;

	NN<InflateState> pDecomp = MemAllocNN(InflateState);
	this->state = pDecomp;

	pDecomp->m_decomp.m_state = 0;
	pDecomp->m_dict_ofs = 0;
	pDecomp->m_last_status = STATUS_NEEDSMOREINPUT;
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
		InflateResult res = this->Inflate(false);
		if (this->avail_in == 0 || (OSInt)res < 0)
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

Bool Data::Compress::Inflater::DecompressDirect(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff, Bool hasHeader)
{
	UInt32 decomp_flags = INFLATER_FLAG_COMPUTE_ADLER32;
//	UOSInt in_bytes;
//	UOSInt out_bytes;
	size_t in_bytes;
	size_t out_bytes;

	if (hasHeader)
		decomp_flags |= INFLATER_FLAG_PARSE_ZLIB_HEADER;
	in_bytes = srcBuff.GetSize();
	out_bytes = destBuff.GetSize();
//	InflateDecompressor decomp;
	tinfl_decompressor decomp;
	decomp.m_state = 0;
	decomp_flags |= INFLATER_FLAG_USING_NON_WRAPPING_OUTPUT_BUF;
//	InflateStatus status = Decompress(decomp, srcBuff.Arr(), in_bytes, destBuff.Arr(), destBuff.Arr(), out_bytes, decomp_flags);
	tinfl_status status = test_decompress(&decomp, srcBuff.Ptr(), &in_bytes, destBuff.Ptr(), destBuff.Ptr(), &out_bytes, decomp_flags);
	if ((OSInt)status < 0)
		return false;
	else if (status != STATUS_DONE)
	{
		return false;
	}
	outDestBuffSize.Set(out_bytes);
	return true;
}
