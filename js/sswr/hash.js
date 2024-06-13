import * as data from "./data.js";
import * as text from "./text.js";

export const HashType = {
	Unknown: 0,
	// Primary Algorithm
	Adler32: 1,
	CRC16: 2,
	CRC16R: 3,
	CRC32: 4,
	CRC32R_IEEE: 5,
	CRC32C: 6,
	DJB2: 7,
	DJB2a: 8,
	FNV1: 9,
	FNV1a: 10,
	MD5: 11,
	RIPEMD128: 12,
	RIPEMD160: 13,
	SDBM: 14,
	SHA1: 15,
	Excel: 16,
	SHA224: 17,
	SHA256: 18,
	SHA384: 19,
	SHA512: 20,
	MD4: 21,

	// Compound Algorithm
	SHA1_SHA1: 22
}

export class Hash
{
}

export class SHA1 extends Hash
{
	constructor()
	{
		super();
		this.messageLength        = 0n;
		this.messageBlockIndex    = 0;
		this.intermediateHash = [0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0];
		this.messageBlock = new Array(64);
	}

	getName()
	{
		return "SHA-1";
	}

	clone()
	{
		let o = new SHA1();
		o.messageLength = this.messageLength;
		o.messageBlockIndex = this.messageBlockIndex;

		o.intermediateHash[0] = this.intermediateHash[0];
		o.intermediateHash[1] = this.intermediateHash[1];
		o.intermediateHash[2] = this.intermediateHash[2];
		o.intermediateHash[3] = this.intermediateHash[3];
		o.intermediateHash[4] = this.intermediateHash[4];
	
		let i = 0;
		while (i < this.messageBlockIndex)
		{
			o.messageBlock[i] = this.messageBlock[i];
			i++;
		}
		return o;
	}

	clear()
	{
		this.messageLength        = 0n;
		this.messageBlockIndex    = 0;
		
		this.intermediateHash = [0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0];
		this.messageBlock = new Array(64);
	}

	calc(buff)
	{
		if (!(buff instanceof ArrayBuffer))
			return;
		let i;
		let arr;
		this.messageLength += BigInt(buff.byteLength << 3);
		if ((buff.byteLength + this.messageBlockIndex) < 64)
		{
			arr = new Uint8Array(buff);
			i = 0;
			while (i < buff.byteLength)
			{
				this.messageBlock[this.messageBlockIndex + i] = arr[i];
				i++;
			}
			this.messageBlockIndex += buff.byteLength;
			return;
		}
		i = 0;
		if (this.messageBlockIndex > 0)
		{
			arr = new Uint8Array(buff);
			while (this.messageBlockIndex < 64)
			{
				this.messageBlock[this.messageBlockIndex] = arr[i];
				this.messageBlockIndex++;
				i++;
			}
			SHA1.calcBlock(this.intermediateHash, new Uint8Array(this.messageBlock).buffer);
			this.messageBlockIndex = 0;
		}

		while (i + 64 <= buff.byteLength)
		{
			SHA1.calcBlock(this.intermediateHash, buff.slice(i, i + 64));
			i += 64;
		}
		if (i < buff.byteLength)
		{
			arr = new Uint8Array(buff);
			while (i < buff.byteLength)
			{
				this.messageBlock[this.messageBlockIndex] = arr[i];
				this.messageBlockIndex++;
				i++;
			}
		}
	}

	getValue()
	{
		let calBuff = new Array(64);
		let intHash = [
			this.intermediateHash[0],
			this.intermediateHash[1],
			this.intermediateHash[2],
			this.intermediateHash[3],
			this.intermediateHash[4]];
	
		let i;
		i = 0;
		while (i < this.messageBlockIndex)
		{
			calBuff[i] = this.messageBlock[i];
			i++;
		}
		if (this.messageBlockIndex < 55)
		{
			i = this.messageBlockIndex;
			calBuff[i++] = 0x80;
			while (i < 56)
			{
				calBuff[i] = 0;
				i++;
			}
	
		}
		else
		{
			i = this.messageBlockIndex;
			calBuff[i++] = 0x80;
			while (i < 64)
			{
				calBuff[i] = 0;
				i++;
			}
			SHA1.calcBlock(intHash, calBuff);
			i = 0;
			while (i < 56)
			{
				calBuff[i] = 0;
				i++;
			}
		}
		let view = new DataView(new Uint8Array(calBuff).buffer);
		view.setBigUint64(56, this.messageLength, false);
		SHA1.calcBlock(intHash, view.buffer);
		view = new DataView(new ArrayBuffer(20));
		i = 20;
		while (i > 0)
		{
			i -= 4;
			view.setUint32(i, intHash[i >> 2], false);
		}
		return view.buffer;
	}

	getBlockSize()
	{
		return 64;
	}

	static calcBlock(intermediateHash, messageBlock)
	{
		let w = new Array(80);
		const k = [0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6];
		let t;
		let temp;
		let a;
		let b;
		let c;
		let d;
		let e;
	
		let blk = new data.ByteReader(messageBlock);

		for(t = 0; t < 16; t++)
		{
			w[t] = blk.readUInt32(t * 4, false);
		}
	
		for(t = 16; t < 80; t++)
		{
			w[t] = data.rol32(w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16], 1);
		}
	
		a = intermediateHash[0];
		b = intermediateHash[1];
		c = intermediateHash[2];
		d = intermediateHash[3];
		e = intermediateHash[4];
	
		for(t = 0; t < 20; t++)
		{
			temp = (data.rol32(a, 5) + (((b & c) | ((~b) & d)) + e + w[t] + k[0])) & 0xffffffff;
			e = d;
			d = c;
			c = data.rol32(b, 30);
			b = a;
			a = temp;
		}
	
		for(t = 20; t < 40; t++)
		{
			temp = (data.rol32(a, 5) + (b ^ c ^ d) + e + w[t] + k[1]) & 0xffffffff;
			e = d;
			d = c;
			c = data.rol32(b, 30);
			b = a;
			a = temp;
		}
	
		for(t = 40; t < 60; t++)
		{
			temp = (data.rol32(a, 5) + ((b & c) | (b & d) | (c & d)) + e + w[t] + k[2]) & 0xffffffff;
			e = d;
			d = c;
			c = data.rol32(b, 30);
			b = a;
			a = temp;
		}
	
		for(t = 60; t < 80; t++)
		{
			temp = (data.rol32(a, 5) + (b ^ c ^ d) + e + w[t] + k[3]) & 0xffffffff;
			e = d;
			d = c;
			c = data.rol32(b, 30);
			b = a;
			a = temp;
		}
		
		intermediateHash[0] = (intermediateHash[0] + a) & 0xffffffff;
		intermediateHash[1] = (intermediateHash[1] + b) & 0xffffffff;
		intermediateHash[2] = (intermediateHash[2] + c) & 0xffffffff;
		intermediateHash[3] = (intermediateHash[3] + d) & 0xffffffff;
		intermediateHash[4] = (intermediateHash[4] + e) & 0xffffffff;
	}
}

export class MD5 extends Hash
{
	constructor()
	{
		super();
		this.buff = new Array(64);
		this.msgLeng = 0n;
		this.h = [0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476];
		this.buffSize = 0;
	}

	getName()
	{
		return "MD5";
	}

	clone()
	{
		let md5 = new MD5();
		md5.msgLeng = this.msgLeng;
		md5.h[0] = this.h[0];
		md5.h[1] = this.h[1];
		md5.h[2] = this.h[2];
		md5.h[3] = this.h[3];
		md5.buffSize = this.buffSize;
		return md5;
	}

	clear()
	{
		this.msgLeng = 0n;
		this.h = [0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476];
		this.buffSize = 0;
	}

	calc(buff)
	{
		if (!(buff instanceof ArrayBuffer))
			return;
		let i = 0;
		let arr;
		this.msgLeng += BigInt(buff.byteLength << 3);
		if ((buff.byteLength + this.buffSize) < 64)
		{
			arr = new Uint8Array(buff);
			while (i < buff.byteLength)
			{
				this.buff[this.buffSize + i] = arr[i];
				i++;
			}
			this.buffSize += buff.byteLength;
			return;
		}
	
		if (this.buffSize > 0)
		{
			arr = new Uint8Array(buff);
			while (this.buffSize < 64)
			{
				this.buff[this.buffSize] = arr[i];
				this.buffSize++;
				i++;
			}
			MD5.calcBlock(this.h, new Uint8Array(this.buff).buffer);
			this.buffSize = 0;
		}
		while (i + 64 <= buff.byteLength)
		{
			MD5.calcBlock(this.h, buff.slice(i, i + 64));
			i += 64;
		}
		if (i < buff.byteLength)
		{
			arr = new Uint8Array(buff);
			while (i < buff.byteLength)
			{
				this.buff[this.buffSize] = arr[i];
				this.buffSize++;
				i++;
			}
		}	
	}

	getValue()
	{
		let calBuff = new Array(64);
		let intHash = [
			this.h[0],
			this.h[1],
			this.h[2],
			this.h[3]];
		let i;
		i = 0;
		while (i < this.buffSize)
		{
			calBuff[i] = this.buff[i];
			i++;
		}
		if (this.buffSize < 56)
		{
			i = this.buffSize;
			calBuff[i++] = 0x80;
			while (i < 56)
			{
				calBuff[i] = 0;
				i++;
			}
	
		}
		else
		{
			i = this.buffSize;
			calBuff[i++] = 0x80;
			while (i < 64)
			{
				calBuff[i] = 0;
				i++;
			}
			MD5.calcBlock(intHash, calBuff);
			i = 0;
			while (i < 56)
			{
				calBuff[i] = 0;
				i++;
			}
		}
		let view = new DataView(new Uint8Array(calBuff).buffer);
		view.setBigUint64(56, this.msgLeng, true);
		MD5.calcBlock(intHash, view.buffer);
		view = new DataView(new ArrayBuffer(16));
		i = 16;
		while (i > 0)
		{
			i -= 4;
			view.setUint32(i, intHash[i >> 2], true);
		}
		return view.buffer;
	}

	getBlockSize()
	{
		return 64;
	}

	static step1(vals, w, x, y, z, dataNum, s)
	{
		vals[w] += vals[z] ^ (vals[x] & (vals[y] ^ vals[z]));
		vals[w] += dataNum;
		vals[w] = data.rol32(vals[w], s);
		vals[w] += vals[x];
	}
	
	static step2(vals, w, x, y, z, dataNum, s)
	{
		vals[w] += vals[y] ^ (vals[z] & (vals[x] ^ vals[y]));
		vals[w] += dataNum;
		vals[w] = data.rol32(vals[w], s);
		vals[w] += vals[x];
	}

	static step3(vals, w, x, y, z, dataNum, s)
	{
		vals[w] += vals[z] ^ vals[y] ^ vals[x];
		vals[w] += dataNum;
		vals[w] = data.rol32(vals[w], s);
		vals[w] += vals[x];
	}
	
	static step4(vals, w, x, y, z, dataNum, s)
	{
		vals[w] += vals[y] ^ (vals[x] | ~vals[z]);
		vals[w] += dataNum;
		vals[w] = data.rol32(vals[w], s);
		vals[w] += vals[x];
	}

	static calcBlock(hVals, block)
	{
		let view = new DataView(block);
		let blk = [view.getUint32(0, true),
			view.getUint32(4, true),
			view.getUint32(8, true),
			view.getUint32(12, true),
			view.getUint32(16, true),
			view.getUint32(20, true),
			view.getUint32(24, true),
			view.getUint32(28, true),
			view.getUint32(32, true),
			view.getUint32(36, true),
			view.getUint32(40, true),
			view.getUint32(44, true),
			view.getUint32(48, true),
			view.getUint32(52, true),
			view.getUint32(56, true),
			view.getUint32(60, true)];
		let vals = [hVals[0], hVals[1], hVals[2], hVals[3]];
		let a = 0;
		let b = 1;
		let c = 2;
		let d = 3;
	
		MD5.step1(vals, a, b, c, d, blk[0] + 0xd76aa478, 7);
		MD5.step1(vals, d, a, b, c, blk[1] + 0xe8c7b756, 12);
		MD5.step1(vals, c, d, a, b, blk[2] + 0x242070db, 17);
		MD5.step1(vals, b, c, d, a, blk[3] + 0xc1bdceee, 22);
		MD5.step1(vals, a, b, c, d, blk[4] + 0xf57c0faf, 7);
		MD5.step1(vals, d, a, b, c, blk[5] + 0x4787c62a, 12);
		MD5.step1(vals, c, d, a, b, blk[6] + 0xa8304613, 17);
		MD5.step1(vals, b, c, d, a, blk[7] + 0xfd469501, 22);
		MD5.step1(vals, a, b, c, d, blk[8] + 0x698098d8, 7);
		MD5.step1(vals, d, a, b, c, blk[9] + 0x8b44f7af, 12);
		MD5.step1(vals, c, d, a, b, blk[10] + 0xffff5bb1, 17);
		MD5.step1(vals, b, c, d, a, blk[11] + 0x895cd7be, 22);
		MD5.step1(vals, a, b, c, d, blk[12] + 0x6b901122, 7);
		MD5.step1(vals, d, a, b, c, blk[13] + 0xfd987193, 12);
		MD5.step1(vals, c, d, a, b, blk[14] + 0xa679438e, 17);
		MD5.step1(vals, b, c, d, a, blk[15] + 0x49b40821, 22);

		MD5.step2(vals, a, b, c, d, blk[1] + 0xf61e2562, 5);
		MD5.step2(vals, d, a, b, c, blk[6] + 0xc040b340, 9);
		MD5.step2(vals, c, d, a, b, blk[11] + 0x265e5a51, 14);
		MD5.step2(vals, b, c, d, a, blk[0] + 0xe9b6c7aa, 20);
		MD5.step2(vals, a, b, c, d, blk[5] + 0xd62f105d, 5);
		MD5.step2(vals, d, a, b, c, blk[10] + 0x02441453, 9);
		MD5.step2(vals, c, d, a, b, blk[15] + 0xd8a1e681, 14);
		MD5.step2(vals, b, c, d, a, blk[4] + 0xe7d3fbc8, 20);
		MD5.step2(vals, a, b, c, d, blk[9] + 0x21e1cde6, 5);
		MD5.step2(vals, d, a, b, c, blk[14] + 0xc33707d6, 9);
		MD5.step2(vals, c, d, a, b, blk[3] + 0xf4d50d87, 14);
		MD5.step2(vals, b, c, d, a, blk[8] + 0x455a14ed, 20);
		MD5.step2(vals, a, b, c, d, blk[13] + 0xa9e3e905, 5);
		MD5.step2(vals, d, a, b, c, blk[2] + 0xfcefa3f8, 9);
		MD5.step2(vals, c, d, a, b, blk[7] + 0x676f02d9, 14);
		MD5.step2(vals, b, c, d, a, blk[12] + 0x8d2a4c8a, 20);

		MD5.step3(vals, a, b, c, d, blk[5] + 0xfffa3942, 4);
		MD5.step3(vals, d, a, b, c, blk[8] + 0x8771f681, 11);
		MD5.step3(vals, c, d, a, b, blk[11] + 0x6d9d6122, 16);
		MD5.step3(vals, b, c, d, a, blk[14] + 0xfde5380c, 23);
		MD5.step3(vals, a, b, c, d, blk[1] + 0xa4beea44, 4);
		MD5.step3(vals, d, a, b, c, blk[4] + 0x4bdecfa9, 11);
		MD5.step3(vals, c, d, a, b, blk[7] + 0xf6bb4b60, 16);
		MD5.step3(vals, b, c, d, a, blk[10] + 0xbebfbc70, 23);
		MD5.step3(vals, a, b, c, d, blk[13] + 0x289b7ec6, 4);
		MD5.step3(vals, d, a, b, c, blk[0] + 0xeaa127fa, 11);
		MD5.step3(vals, c, d, a, b, blk[3] + 0xd4ef3085, 16);
		MD5.step3(vals, b, c, d, a, blk[6] + 0x04881d05, 23);
		MD5.step3(vals, a, b, c, d, blk[9] + 0xd9d4d039, 4);
		MD5.step3(vals, d, a, b, c, blk[12] + 0xe6db99e5, 11);
		MD5.step3(vals, c, d, a, b, blk[15] + 0x1fa27cf8, 16);
		MD5.step3(vals, b, c, d, a, blk[2] + 0xc4ac5665, 23);

		MD5.step4(vals, a, b, c, d, blk[0] + 0xf4292244, 6);
		MD5.step4(vals, d, a, b, c, blk[7] + 0x432aff97, 10);
		MD5.step4(vals, c, d, a, b, blk[14] + 0xab9423a7, 15);
		MD5.step4(vals, b, c, d, a, blk[5] + 0xfc93a039, 21);
		MD5.step4(vals, a, b, c, d, blk[12] + 0x655b59c3, 6);
		MD5.step4(vals, d, a, b, c, blk[3] + 0x8f0ccc92, 10);
		MD5.step4(vals, c, d, a, b, blk[10] + 0xffeff47d, 15);
		MD5.step4(vals, b, c, d, a, blk[1] + 0x85845dd1, 21);
		MD5.step4(vals, a, b, c, d, blk[8] + 0x6fa87e4f, 6);
		MD5.step4(vals, d, a, b, c, blk[15] + 0xfe2ce6e0, 10);
		MD5.step4(vals, c, d, a, b, blk[6] + 0xa3014314, 15);
		MD5.step4(vals, b, c, d, a, blk[13] + 0x4e0811a1, 21);
		MD5.step4(vals, a, b, c, d, blk[4] + 0xf7537e82, 6);
		MD5.step4(vals, d, a, b, c, blk[11] + 0xbd3af235, 10);
		MD5.step4(vals, c, d, a, b, blk[2] + 0x2ad7d2bb, 15);
		MD5.step4(vals, b, c, d, a, blk[9] + 0xeb86d391, 21);

		vals[a] += hVals[0];
		vals[b] += hVals[1];
		vals[c] += hVals[2];
		vals[d] += hVals[3];
		hVals[0] = vals[a];
		hVals[1] = vals[b];
		hVals[2] = vals[c];
		hVals[3] = vals[d];
	}
}
