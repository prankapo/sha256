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
	char msg[MSG_LEN];
	uint8_t *msg_bin;
	uint32_t primes[64], h[8], k[64], w[64];
};

struct constants cnt;

int main(int argc, char **argv)
{
	findprimes(64);
	printarray32(cnt.primes, 64, 'i');
	fillh();
	printarray32(cnt.h, 8, 'h');
	fillk();
	printarray32(cnt.k, 64, 'h');
	system("pause");
	system("clear");
	printf(">> ");
	fgets(cnt.msg, MSG_LEN, stdin);		//entering the message

	//finding the length of the message
	int len;
	len = -1;
	for(int i = 0; i < 128; ++i)
	{
		if(cnt.msg[i] == 0 ||
		   cnt.msg[i] == 10)
		{
			if(len == -1)
			{
				len = i;
			}
		}
	}
	
	//copying message into a 64 byte (512 bits) array. 
	uint8_t tmp[64];
	for(int i = 0; i < 64; ++i)
	{
		if(i < len)
		{
			tmp[i] = cnt.msg[i];
		}
		else if(i == len)
		{
			tmp[i] = 0b10000000;
		}
		else if(i > len)
		{
			tmp[i] = 0x00;
		}
	}
	tmp[63] = len * 8; 			//over here, length in bytes is to be multiplied by no. of bits/byte = 8
	cnt.msg_bin = tmp;		//assigning tmp to cnt.msg_bin
	
	for(int i = 0; i < 64; ++i)
	{
		printf("%x ", cnt.msg_bin[i]);
	}
	printf("size of padded message = %d bits", sizeof(tmp[0])*64*8);
	printf("\n");
	
	//Making a message schedule consisting of 32 bit words
	for(int i = 0; i < 64; ++i)
	{
		if(i * 4 < 64)
		{
			int j = i * 4;
			cnt.w[i] = (cnt.msg_bin[j] << (8 * 3)) | (cnt.msg_bin[j + 1] << (8 * 2)) | (cnt.msg_bin[j + 2] << (8 * 1)) | (cnt.msg_bin[j + 3] << 0);
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
		printf("%02d. %08x \n",i , cnt.w[i]);
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
	printf("\n%X\n%X\n%X\n%X\n%X\n%X\n%X\n%X\n", a, b, c, d, e, f, g, h);
	cnt.h[0] += a;
	cnt.h[1] += b;
	cnt.h[2] += c;
	cnt.h[3] += d;
	cnt.h[4] += e;
	cnt.h[5] += f;
	cnt.h[6] += g;
	cnt.h[7] += h;
	printarray32(cnt.h, 8, 'h');
	printf("\n");
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