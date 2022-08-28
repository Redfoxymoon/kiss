#include <stdlib.h>
#include <string.h>

extern unsigned int cp932_table[];

static int cp932_to_utf16(unsigned int *r, unsigned char *s)
{
	int ret = 0;
	unsigned int buf;

	if((*s >= 0x81 && *s <= 0x9f) || (*s >= 0xe0 && *s <= 0xef) || (*s >=0xfa && *s <= 0xfc))
	{
		buf = *(s++) << 8;
		buf |= *s;
		ret = 2;
	}
	else
	{
		buf = *s;
		ret = 1;
	}

	*r = cp932_table[buf];

	if(!*r)
		return -1;

	return ret;
}

static int utf16_to_utf8(char *s, unsigned int *r)
{
	if(*r == (*r & 0x7f))
	{
		*s = *r & 0x7f;
		return 1;
	}
	else if(*r == (*r & 0x7ff))
	{
		*(s++) = 0300 | ((*r >> 6) & 0277);
		*s = 0200 | (*r&077);
		return 2;
	}
	else if(*r == (*r & 0xffff))
	{
		*(s++) = 0340 | ((*r >> 12) & 0337);
		*(s++) = 0200 | ((*r >> 6) & 077);
		*s = 0200 | (*r & 077);
		return 3;
	}
	else if(*r == (*r & 0x10ffff))
	{
		*(s++) = 0360 | ((*r >> 18) & 0357);
		*(s++) = 0200 | ((*r >> 12) & 077);
		*(s++) = 0200 | ((*r >> 6) & 077);
		*s = 0200 | (*r & 077);
		return 4;
	}
	else
	{
		return -1;
	}
}

int cp932_to_utf8(const char *cp932, char **utf8)
{
	char buf[4];
	unsigned int r;
	int slen, ulen, len, i, j;

	len = strlen(cp932);

	*utf8 = malloc((len * 4) + 1);

	if(!*utf8)
		return -2;

	i = 0;
	while(len)
	{
		slen = cp932_to_utf16(&r, (unsigned char *)cp932);

		if(slen < 0)
			return -1;

		ulen = utf16_to_utf8(buf, &r);

		cp932 += slen;
		len -= slen;

		for(j = 0; j < ulen; j++)
		{
			(*utf8)[i++] = buf[j];
		}
	}
	(*utf8)[i] = '\0';

	return 0;
}

#if TEST
#include <stdio.h>
int main()
{
	int i;
	/*const char *la = "\x5c\x20\x82\xcd\x82\xeb\x8f\x97\xfb\x48\xfa\x55";*/
	//const char *la = "\x69\x6d\x61\x67\x65\x5c\x62\x67\x5c\x75\x70\x5c\x83\x5c\x83\x74\x83\x42\x83\x41\x95\x94\x89\xae\x2e\x70\x6e\x67";
	const char *la = "\x41\x52\xe3\x83\x87\xe3\x82\xa3\xe3\x82\xb9\xe3\x83\x97\xe3\x83\xac\xe3\x82\xa4\xe3\x81\xae\xe3\x81\xbf\x2e\x70\x6e\x67";
	char *la2;
	int status;
	status = cp932_to_utf8(la, &la2);

	if(status < 0)
	{
		puts("encoding error");
		exit(1);
	}

	printf("string %s\n", la2);
	for(i = 0; i < strlen(la); i++)
		printf("char %hhx\n", la2[i]);
	free(la2);
}
#endif
