#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "readbytes.h"
#include "cp932.h"

#ifdef _WIN32
#include "win32compat.h"
#endif

int main(int argc, char *argv[])
{
	int i;
	char c;
	char *name;
	char filename[512];
	FILE *archive, *out;
	unsigned char sbuf[8];
	unsigned int count, file_size, file_offset, file_offset_next, pos, end_of_file;

	if(argc != 2)
	{
		printf("%s: <archive.arc>\n", argv[0]);
		return 1;
	}

	archive = fopen(argv[1], "rb");

	if(fread(sbuf, 1, 4, archive) != 4)
	{
		puts("not a KISS archive");
		return 1;
	}

	count = read_uint32_le(sbuf);

	fseek(archive, 0, SEEK_END);
	end_of_file = ftell(archive);
	fseek(archive, 4, SEEK_SET);

	if(count == 0)
	{
		puts("not a KISS archive");
		exit(1);
	}

	printf("index count: %d\n", count);

	while(count)
	{
		i = 0;
		while((c = fgetc(archive)) != 0)
		{
			filename[i] = c;
			i++;
		}
		filename[i] = '\0';

		fread(sbuf, 1, 8, archive);
		file_offset = read_uint64_le(sbuf);

		printf("%d\n", file_offset);

		pos = ftell(archive);
		while((c = fgetc(archive)) != 0);

		if(count - 1)
		{
			fread(sbuf, 1, 8, archive);
			file_offset_next = read_uint64_le(sbuf);

			file_size = file_offset_next - file_offset;
			fseek(archive, pos, SEEK_SET);
		}
		else
		{
			file_size = end_of_file - file_offset;
		}

		cp932_to_utf8(filename, &name);

		printf("filename %s\n", name);
		printf("file size %d\n", file_size);

		pos = ftell(archive);
		fseek(archive, file_offset, SEEK_SET);

		out = fopen(name, "wb");

		while(file_size)
		{
			putc(fgetc(archive), out);
			file_size--;
		}
		fclose(out);
		fseek(archive, pos, SEEK_SET);

		count--;
	}

	return 0;
}
