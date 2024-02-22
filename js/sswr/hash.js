import * as data from "./data.js";

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

function sha1CircularShift(x, y)
{
	return ((x << y) | (x >> (32 - y)))	& 0xffffffff;
}
export class SHA1 extends Hash
{
	constructor()
	{
		super();
		this.clear();
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
		this.messageLength        = 0;
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
		this.messageLength += (buff.byteLength << 3);
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
		view.setUint32(56, this.messageLength, false);
		SHA1.calcBlock(intHash, view.buffer);
		view = new DataView(new Uint8Array(20));
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
			w[t] = sha1CircularShift(1, w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16]);
		}
	
		a = intermediateHash[0];
		b = intermediateHash[1];
		c = intermediateHash[2];
		d = intermediateHash[3];
		e = intermediateHash[4];
	
		for(t = 0; t < 20; t++)
		{
			temp = (sha1CircularShift(5, a) + (((b & c) | ((~b) & d)) + e + w[t] + k[0])) & 0xffffffff;
			e = d;
			d = c;
			c = sha1CircularShift(30, b);
			b = a;
			a = temp;
		}
	
		for(t = 20; t < 40; t++)
		{
			temp = (sha11CircularShift(5, a) + (b ^ c ^ d) + e + w[t] + k[1]) & 0xffffffff;
			e = d;
			d = c;
			c = sha1CircularShift(30, b);
			b = a;
			a = temp;
		}
	
		for(t = 40; t < 60; t++)
		{
			temp = (sha1CircularShift(5, a) + ((b & c) | (b & d) | (c & d)) + e + w[t] + k[2]) & 0xffffffff;
			e = d;
			d = c;
			c = sha1CircularShift(30, b);
			b = a;
			a = temp;
		}
	
		for(t = 60; t < 80; t++)
		{
			temp = (sha1CircularShift(5, a) + (b ^ c ^ d) + e + w[t] + k[3]) & 0xffffffff;
			e = d;
			d = c;
			c = sha1CircularShift(30, b);
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