#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/TextBinEnc/Punycode.h"

#define BASE 36
#define TMIN 1
#define TMAX 26
#define SKEW 38
#define DAMP 700

UOSInt Text::TextBinEnc::Punycode::Adapt(UOSInt delta, UOSInt numPoints, Bool firstTime)
{
	UOSInt k;

	delta = firstTime ? (delta / DAMP) : (delta >> 1);
	delta += delta / numPoints;

	k = 0;
	while (delta > ((BASE - TMIN) * TMAX) / 2)
	{
		delta /= BASE - 1;
		k += BASE;
	}

	return k + (BASE - TMIN + 1) * delta / (delta + SKEW);
}

UnsafeArray<UTF8Char> Text::TextBinEnc::Punycode::Encode(UnsafeArray<UTF8Char> buff, Text::CStringNN strToEnc)
{
	return strToEnc.ConcatTo(buff);
}

UnsafeArray<UTF8Char> Text::TextBinEnc::Punycode::Encode(UnsafeArray<UTF8Char> buff, const WChar *strToEnc)
{
/*	const WChar *srcPtr;
	WChar c;
	srcPtr = strToEnc;
	while (true)
	{
		c = *strToEnc++;
		if (c == 0 || c == '.')
		{

		}
	}*/

	return Text::StrWChar_UTF8(buff, strToEnc);
/*
	punycode_uint n, delta, h, b, out, max_out, bias, j, m, q, k, t;

  n = initial_n;
  delta = out = 0;
  max_out = *output_length;
  bias = initial_bias;

  for (j = 0;  j < input_length;  ++j) {
    if (basic(input[j])) {
      if (max_out - out < 2) return punycode_big_output;
      output[out++] =
        case_flags ?  encode_basic(input[j], case_flags[j]) : input[j];
    }
  }

  h = b = out;

  if (b > 0) output[out++] = delimiter;

  while (h < input_length) {
    for (m = maxint, j = 0;  j < input_length;  ++j) {
      if (input[j] >= n && input[j] < m) m = input[j];
    }

    if (m - n > (maxint - delta) / (h + 1)) return punycode_overflow;
    delta += (m - n) * (h + 1);
    n = m;

    for (j = 0;  j < input_length;  ++j) {
      if (input[j] < n  ) {
        if (++delta == 0) return punycode_overflow;
      }

      if (input[j] == n) {

        for (q = delta, k = base;  ;  k += base) {
          if (out >= max_out) return punycode_big_output;
          t = k <= bias  ? tmin :     
              k >= bias + tmax ? tmax : k - bias;
          if (q < t) break;
          output[out++] = encode_digit(t + (q - t) % (base - t), 0);
          q = (q - t) / (base - t);
        }

        output[out++] = encode_digit(q, case_flags && case_flags[j]);
        bias = adapt(delta, h + 1, h == b);
        delta = 0;
        ++h;
      }
    }

    ++delta, ++n;
  }

  *output_length = out;
  return punycode_success;
  */
	return Text::StrWChar_UTF8(buff, strToEnc);
}

UnsafeArray<WChar> Text::TextBinEnc::Punycode::Encode(UnsafeArray<WChar> buff, UnsafeArray<const WChar> strToEnc)
{
/*	const WChar *srcPtr;
	WChar c;
	srcPtr = strToEnc;
	while (true)
	{
		c = *strToEnc++;
		if (c == 0 || c == '.')
		{

		}
	}*/

	return Text::StrConcat(buff, strToEnc);
/*
	punycode_uint n, delta, h, b, out, max_out, bias, j, m, q, k, t;

  n = initial_n;
  delta = out = 0;
  max_out = *output_length;
  bias = initial_bias;

  for (j = 0;  j < input_length;  ++j) {
    if (basic(input[j])) {
      if (max_out - out < 2) return punycode_big_output;
      output[out++] =
        case_flags ?  encode_basic(input[j], case_flags[j]) : input[j];
    }
  }

  h = b = out;

  if (b > 0) output[out++] = delimiter;

  while (h < input_length) {
    for (m = maxint, j = 0;  j < input_length;  ++j) {
      if (input[j] >= n && input[j] < m) m = input[j];
    }

    if (m - n > (maxint - delta) / (h + 1)) return punycode_overflow;
    delta += (m - n) * (h + 1);
    n = m;

    for (j = 0;  j < input_length;  ++j) {
      if (input[j] < n  ) {
        if (++delta == 0) return punycode_overflow;
      }

      if (input[j] == n) {

        for (q = delta, k = base;  ;  k += base) {
          if (out >= max_out) return punycode_big_output;
          t = k <= bias  ? tmin :     
              k >= bias + tmax ? tmax : k - bias;
          if (q < t) break;
          output[out++] = encode_digit(t + (q - t) % (base - t), 0);
          q = (q - t) / (base - t);
        }

        output[out++] = encode_digit(q, case_flags && case_flags[j]);
        bias = adapt(delta, h + 1, h == b);
        delta = 0;
        ++h;
      }
    }

    ++delta, ++n;
  }

  *output_length = out;
  return punycode_success;
  */
	return Text::StrConcat(buff, strToEnc);
}

UnsafeArray<UTF8Char> Text::TextBinEnc::Punycode::Decode(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> strToDec, UOSInt strLen)
{
	UTF8Char c;
	UTF32Char tmpBuff[128];
	while (strLen > 0)
	{
		if ((strToDec[0] == 'X' || strToDec[0] == 'x') && (strToDec[1] == 'n' || strToDec[1] == 'N') && strToDec[2] == '-' && strToDec[3] == '-')
		{
			strToDec += 4;
			strLen -= 4;
			UnsafeArray<const UTF8Char> sptr;
			UnsafeArray<const UTF8Char> sptrEnd;

			UOSInt n = 128;//initial_n
			UOSInt i = 0;
			UOSInt bias = 72;//initial_bias

			UOSInt destSize = 0;
			UOSInt lastMinus = 0;
			sptr = strToDec;
			sptrEnd = strToDec + strLen;
			while (sptr < sptrEnd)
			{
				c = *sptr;
				if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
				{
					sptr++;
				}
				else if (c == '-')
				{
					lastMinus = (UOSInt)(sptr - strToDec);
					sptr++;
				}
				else
					break;
			}

			if (lastMinus > 0)
			{
				sptr = &strToDec[lastMinus];
				while (strToDec < sptr)
				{
					tmpBuff[destSize] = *strToDec++;
					destSize++;
				}
				strToDec = sptr + 1;
			}
			UOSInt digit;
			UOSInt t;
			UOSInt out = destSize;
			while (sptrEnd > strToDec)
			{
				c = *strToDec;
				if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9'))
				{
					break;
				}
				UOSInt oldi = i;
				UOSInt w = 1;
				UOSInt k = BASE; //base;
				while (true)
				{
					c = *strToDec;
					if (c >= 'a' && c <= 'z')
					{
						digit = (UOSInt)(c - 'a');
					}
					else if (c >= 'A' && c <= 'Z')
					{
						digit = (UOSInt)(c - 'A');
					}
					else if (c >= '0' && c <= '9')
					{
						digit = (UOSInt)(c - '0' + 26);
					}
					else
					{
						break;
					}
					strToDec++;

					i += digit * w;
					t = (k <= bias)?TMIN:((k >= bias + TMAX)?TMAX:k - bias);
					if (digit < t)
						break;
					w = w * (BASE - t);
					k += BASE; //base;
				}

				bias = Adapt(i - oldi, out + 1, oldi == 0);
				n += i / (out + 1);
				i %= (out + 1);

				MemCopyO(&tmpBuff[i + 1], &tmpBuff[i], (destSize - i) * sizeof(UTF32Char));
				tmpBuff[i] = (UTF32Char)n;
				destSize++;
				i++;
				out++;
			}
			buff = Text::StrUTF32_UTF8C(buff, tmpBuff, destSize);
			strLen = (UOSInt)(sptrEnd - strToDec);
		}
		else
		{
			while (strLen > 0)
			{
				c = *strToDec++;
				*buff++ = c;
				strLen--;

				if (c == '.')
				{
					break;
				}
			}
		}
	}
	*buff = 0;
	return buff;
}

WChar *Text::TextBinEnc::Punycode::Decode(WChar *buff, UnsafeArray<const UTF8Char> strToDec)
{
	while (*strToDec)
	{
		if ((strToDec[0] == 'X' || strToDec[0] == 'x') && (strToDec[1] == 'n' || strToDec[1] == 'N') && strToDec[2] == '-' && strToDec[3] == '-')
		{
			strToDec += 4;
			UnsafeArray<const UTF8Char> sptr;
			UTF8Char c;

			UOSInt n = 128;//initial_n
			UOSInt i = 0;
			UOSInt bias = 72;//initial_bias

			UOSInt destSize = 0;
			UOSInt lastMinus = 0;
			sptr = strToDec;
			while (true)
			{
				c = *sptr;
				if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
				{
					sptr++;
				}
				else if (c == '-')
				{
					lastMinus = (UOSInt)(sptr - strToDec);
					sptr++;
				}
				else
					break;
			}

			if (lastMinus > 0)
			{
				sptr = &strToDec[lastMinus];
				while (strToDec < sptr)
				{
					buff[destSize] = *strToDec++;
					destSize++;
				}
				strToDec = sptr + 1;
			}
			UOSInt digit;
			UOSInt t;
			UOSInt out = destSize;
			while (true)
			{
				c = *strToDec;
				if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9'))
				{
					break;
				}
				UOSInt oldi = i;
				UOSInt w = 1;
				UOSInt k = BASE; //base;
				while (true)
				{
					c = *strToDec;
					if (c >= 'a' && c <= 'z')
					{
						digit = (UOSInt)(c - 'a');
					}
					else if (c >= 'A' && c <= 'Z')
					{
						digit = (UOSInt)(c - 'A');
					}
					else if (c >= '0' && c <= '9')
					{
						digit = (UOSInt)(c - '0' + 26);
					}
					else
					{
						break;
					}
					strToDec++;

					i += digit * w;
					t = (k <= bias)?TMIN:((k >= bias + TMAX)?TMAX:k - bias);
					if (digit < t)
						break;
					w = w * (BASE - t);
					k += BASE; //base;
				}

				bias = Adapt(i - oldi, out + 1, oldi == 0);
				n += i / (out + 1);
				i %= (out + 1);

				MemCopyO(&buff[i + 1], &buff[i], (destSize - i) * sizeof(WChar));
				buff[i] = (WChar)n;
				destSize++;
				i++;
				out++;
			}
			buff += destSize;
		}
		else
		{
			*buff++ = *strToDec++;
		}
	}
	*buff = 0;
	return buff;
}

WChar *Text::TextBinEnc::Punycode::Decode(WChar *buff, const WChar *strToDec)
{
	while (*strToDec)
	{
		if ((strToDec[0] == 'X' || strToDec[0] == 'x') && (strToDec[1] == 'n' || strToDec[1] == 'N') && strToDec[2] == '-' && strToDec[3] == '-')
		{
			strToDec += 4;
			const WChar *wptr;
			WChar c;

			UOSInt n = 128;//initial_n
			UOSInt i = 0;
			UOSInt bias = 72;//initial_bias

			UOSInt destSize = 0;
			UOSInt lastMinus = 0;
			wptr = strToDec;
			while (true)
			{
				c = *wptr;
				if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
				{
					wptr++;
				}
				else if (c == '-')
				{
					lastMinus = (UOSInt)(wptr - strToDec);
					wptr++;
				}
				else
					break;
			}

			if (lastMinus > 0)
			{
				wptr = &strToDec[lastMinus];
				while (strToDec < wptr)
				{
					buff[destSize] = *strToDec++;
					destSize++;
				}
				strToDec = wptr + 1;
			}
			UOSInt digit;
			UOSInt t;
			UOSInt out = destSize;
			while (true)
			{
				c = *strToDec;
				if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9'))
				{
					break;
				}
				UOSInt oldi = i;
				UOSInt w = 1;
				UOSInt k = BASE; //base;
				while (true)
				{
					c = *strToDec;
					if (c >= 'a' && c <= 'z')
					{
						digit = (UOSInt)(c - 'a');
					}
					else if (c >= 'A' && c <= 'Z')
					{
						digit = (UOSInt)(c - 'A');
					}
					else if (c >= '0' && c <= '9')
					{
						digit = (UOSInt)(c - '0' + 26);
					}
					else
					{
						break;
					}
					strToDec++;

					i += digit * w;
					t = (k <= bias)?TMIN:((k >= bias + TMAX)?TMAX:k - bias);
					if (digit < t)
						break;
					w = w * (BASE - t);
					k += BASE; //base;
				}

				bias = Adapt(i - oldi, out + 1, oldi == 0);
				n += i / (out + 1);
				i %= (out + 1);

				MemCopyO(&buff[i + 1], &buff[i], (destSize - i) * sizeof(WChar));
				buff[i] = (WChar)n;
				destSize++;
				i++;
				out++;
			}
			buff += destSize;
		}
		else
		{
			*buff++ = *strToDec++;
		}
	}
	*buff = 0;
	return buff;
}

Text::TextBinEnc::Punycode::Punycode()
{
}

Text::TextBinEnc::Punycode::~Punycode()
{
}

UnsafeArray<UTF8Char> Text::TextBinEnc::Punycode::EncodeString(UnsafeArray<UTF8Char> buff, const WChar *strToEnc) const
{
	return Encode(buff, strToEnc);
}

UOSInt Text::TextBinEnc::Punycode::EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const
{
	sb->AppendC(dataBuff, buffSize);
	return buffSize;
}

UOSInt Text::TextBinEnc::Punycode::CalcBinSize(Text::CStringNN str) const
{
	UTF8Char buff[256];
	return (UOSInt)(Decode(buff, str.v, str.leng) - buff);
}

UOSInt Text::TextBinEnc::Punycode::DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const
{
	return (UOSInt)(Decode(dataBuff, str.v, str.leng) - dataBuff);
}

Text::CStringNN Text::TextBinEnc::Punycode::GetName() const
{
	return CSTR("Punycode");
}
