/*PSEUDOCODE FROM WIKIPEDIA
Note 1: All variables are 32 bit unsigned integers and addition is calculated modulo 232
Note 2: For each round, there is one round constant k[i] and one entry in the message schedule array w[i], 0 ≤ i ≤ 63
Note 3: The compression function uses 8 working variables, a through h
Note 4: Big-endian convention is used when expressing the constants in this pseudocode,
    and when parsing message block data from bytes to words, for example,
    the first word of the input message "abc" after padding is 0x61626380

Initialize hash values:
(first 32 bits of the fractional parts of the square roots of the first 8 primes 2..19):
h0 := 0x6a09e667
h1 := 0xbb67ae85
h2 := 0x3c6ef372
h3 := 0xa54ff53a
h4 := 0x510e527f
h5 := 0x9b05688c
h6 := 0x1f83d9ab
h7 := 0x5be0cd19

Initialize array of round constants:
(first 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311):
k[0..63] :=
   0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
   0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
   0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
   0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
   0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
   0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2

Pre-processing (Padding):
begin with the original message of length L bits
append a single '1' bit
append K '0' bits, where K is the minimum number >= 0 such that L + 1 + K + 64 is a multiple of 512
append L as a 64-bit big-endian integer, making the total post-processed length a multiple of 512 bits
such that the bits in the message are L 1 00..<K 0's>..00 <L as 64 bit integer> = k*512 total bits

Process the message in successive 512-bit chunks:
break message into 512-bit chunks
for each chunk
    create a 64-entry message schedule array w[0..63] of 32-bit words
    (The initial values in w[0..63] don't matter, so many implementations zero them here)
    copy chunk into first 16 words w[0..15] of the message schedule array

    Extend the first 16 words into the remaining 48 words w[16..63] of the message schedule array:
    for i from 16 to 63
        s0 := (w[i-15] rightrotate  7) xor (w[i-15] rightrotate 18) xor (w[i-15] rightshift  3)
        s1 := (w[i-2] rightrotate 17) xor (w[i-2] rightrotate 19) xor (w[i-2] rightshift 10)
        w[i] := w[i-16] + s0 + w[i-7] + s1

    Initialize working variables to current hash value:
    a := h0
    b := h1
    c := h2
    d := h3
    e := h4
    f := h5
    g := h6
    h := h7

    Compression function main loop:
    for i from 0 to 63
        S1 := (e rightrotate 6) xor (e rightrotate 11) xor (e rightrotate 25)
        ch := (e and f) xor ((not e) and g)
        temp1 := h + S1 + ch + k[i] + w[i]
        S0 := (a rightrotate 2) xor (a rightrotate 13) xor (a rightrotate 22)
        maj := (a and b) xor (a and c) xor (b and c)
        temp2 := S0 + maj
 
        h := g
        g := f
        f := e
        e := d + temp1
        d := c
        c := b
        b := a
        a := temp1 + temp2

    Add the compressed chunk to the current hash value:
    h0 := h0 + a
    h1 := h1 + b
    h2 := h2 + c
    h3 := h3 + d
    h4 := h4 + e
    h5 := h5 + f
    h6 := h6 + g
    h7 := h7 + h

Produce the final hash value (big-endian):
digest := hash := h0 append h1 append h2 append h3 append h4 append h5 append h6 append h7
*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdint.h>
#include<string.h>

#define MSG_LEN 128

int findprimes(int);
int fillh();
int fillk();
int isprime(int);
int printarray32(uint32_t *, int, char);
int printarray64(uint64_t *, int, char);
uint32_t rightrotate32(uint32_t, int);

struct constants{
	char *msg;
	uint8_t *msg_bin;
	uint32_t primes[64], h[8], k[64], w[64];
};

struct constants cnt;

int main(int argc, char **argv)
{
	findprimes(64);
	fillh();
	fillk();
	if(argc == 2)
	{
		cnt.msg = argv[1];
	}
	else if(argc == 1)
	{
		char ch[] = "";
		cnt.msg = ch;
	}
	//finding the length of the message
	long int L;			//long int has a size of 8 bytes on my machine. 8 * 8 = 64 bits.
	L = 0;
	while(cnt.msg[L] != 0)
	{
		++L;
	}
	L *= 8;
	
	//padding
	int K = 0;
	while(((L + 1 + K + 64) % 512) != 0)
	{
		++K;
	}
	//copying message into an array of size = multiple of 512.
	int psize = (L + 1 + K + 64) / 8;
	uint8_t tmp[psize];
	for(int i = 0; i < psize; ++i)
	{
		if(i < (L / 8))
		{
			tmp[i] = cnt.msg[i];
		}
		else if(i == (L / 8))
		{
			tmp[i] = 0b10000000;
		}
		else if((i > (L / 8)) & (i < psize - 8))
		{
			tmp[i] = 0x00;
		}
		else if(i >= psize - 8)
		{
			tmp[i] = L >> ((psize - i - 1) * 8);
		}
	}
	cnt.msg_bin = tmp;		//assigning tmp to cnt.msg_bin

	//breaking message into 512 bit chunks
	//Making a message schedule consisting of 32 bit words
	int number_of_chunks;
	number_of_chunks = (L + 1 + K + 64) / 512;
	for(int break_point = 0; break_point < number_of_chunks; ++break_point)
	{
		for(int i = 0; i < 64; ++i)
		{
			if(i * 4 < 64)
			{
				int j = (break_point * 64) + (i * 4);
				cnt.w[i] = 	(cnt.msg_bin[j] << (8 * 3)) | 
						(cnt.msg_bin[j + 1] << (8 * 2)) | 
						(cnt.msg_bin[j + 2] << (8 * 1)) | 
						(cnt.msg_bin[j + 3] << 0);
			}
			else if(i >= 16)
			{
				/*
				s0 := (w[i-15] rightrotate  7) xor (w[i-15] rightrotate 18) xor (w[i-15] rightshift  3)
				s1 := (w[i-2] rightrotate 17) xor (w[i-2] rightrotate 19) xor (w[i-2] rightshift 10)
				w[i] := w[i-16] + s0 + w[i-7] + s1
				*/
				uint32_t s0, s1; 
				s0 = rightrotate32(cnt.w[i - 15], 7) ^ rightrotate32(cnt.w[i - 15], 18) ^ (cnt.w[i - 15] >> 3);
				s1 = rightrotate32(cnt.w[i - 2], 17) ^ rightrotate32(cnt.w[i - 2], 19) ^ (cnt.w[i - 2] >> 10);
				cnt.w[i] = cnt.w[i - 16] + s0 + cnt.w[i - 7] + s1;
			}
		}
		uint32_t a, b, c, d, e, f, g, h;
		a = cnt.h[0];
		b = cnt.h[1];
		c = cnt.h[2];
		d = cnt.h[3];
		e = cnt.h[4];
		f = cnt.h[5];
		g = cnt.h[6];
		h = cnt.h[7];
		
		//compression function main loop
		/*
		for i from 0 to 63
			S1 := (e rightrotate 6) xor (e rightrotate 11) xor (e rightrotate 25)
			ch := (e and f) xor ((not e) and g)
			temp1 := h + S1 + ch + k[i] + w[i]
			S0 := (a rightrotate 2) xor (a rightrotate 13) xor (a rightrotate 22)
			maj := (a and b) xor (a and c) xor (b and c)
			temp2 := S0 + maj

			h := g
			g := f
			f := e
			e := d + temp1
			d := c
			c := b
			b := a
			a := temp1 + temp2
		*/
		for(int i = 0; i < 64; ++i)
		{
			uint32_t s0, s1, ch, temp1, temp2, maj;
			s1 = rightrotate32(e, 6) ^ rightrotate32(e, 11) ^ rightrotate32(e, 25);
			ch = (e & f) ^ ((~e) & g);
			temp1 = h + s1 + ch + cnt.k[i] + cnt.w[i];
			s0 = rightrotate32(a, 2) ^ rightrotate32(a, 13) ^ rightrotate32(a, 22);
			maj = (a & b) ^ (a & c) ^ (b & c);
			temp2 = s0 + maj;
			
			h = g;
			g = f;
			f = e;
			e = d + temp1;
			d = c;
			c = b;
			b = a;
			a = temp1 + temp2;
		}
		cnt.h[0] += a;
		cnt.h[1] += b;
		cnt.h[2] += c;
		cnt.h[3] += d;
		cnt.h[4] += e;
		cnt.h[5] += f;
		cnt.h[6] += g;
		cnt.h[7] += h;
	}
	
	FILE *fp = fopen("sha256_result.txt", "w+");
	for(int i = 0; i < 8; ++i)
	{
		fprintf(fp, "%08x", cnt.h[i]);
	}
	fclose(fp);
	return 0;
}

int findprimes(int N)
{
	int num = 2;
	int count = 0;
	while(count < 64)
	{
		if(isprime(num) == 0)
		{
			cnt.primes[count] = num;
			++count;
		}
		++num;
	}
	return 0;
}

int fillh()
{
	for(int i = 0; i < 8; ++i)
	{
		cnt.h[i] = 0;
		double val = pow(cnt.primes[i], 0.5);
		val = val - (int)(val);
		for(int j = 31; j >= 0; --j)
		{
			cnt.h[i] += ((int)(val * 2)) * pow(2, j);
			val = val * 2;
			if(val > 1)
			{
				val -= 1;
			}
		}
	}
	return 0;
}

int fillk()
{
	for(int i = 0; i < 64; ++i)
	{
		cnt.k[i] = 0;
		double val = pow(cnt.primes[i], (1.0 / 3.0));
		val = val - (int)(val);
		for(int j = 31; j >= 0; --j)
		{
			cnt.k[i] += ((int)(val * 2)) * pow(2, j);
			val = val * 2;
			if(val > 1)
			{
				val -= 1;
			}
		}
	}
	return 0;
}

int isprime(int num)
{
	int flag = 0;
	for(int i = 2; i <= num / 2; ++i)
	{
		if(num % i == 0)
		{
			++flag;
		}
	}
	return flag;
}

int printarray32(uint32_t *ptr, int len, char mode)
{
	printf("\n[");
	for(int i = 0; i < len; ++i)
	{
		if(mode == 'i')
		{
			printf("%d, ", ptr[i]);
		}
		else if(mode == 'h')
		{
			printf("%x, ", ptr[i]);
		}
	}
	printf("]\n");
	return 0;
}

int printarray64(uint64_t *ptr, int len, char mode)
{
	printf("\n[");
	for(int i = 0; i < len; ++i)
	{
		if(mode == 'i')
		{
			printf("%d, ", ptr[i]);
		}
		else if(mode == 'h')
		{
			printf("%x, ", ptr[i]);
		}
	}
	printf("]\n");
	return 0;
}

uint32_t rightrotate32(uint32_t x, int n)
{
	return (x << (32 - n)) | (x >> n);
}