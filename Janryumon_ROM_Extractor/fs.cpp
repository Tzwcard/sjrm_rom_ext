#include "fs.h"

/* simple enough with only ascii chars */
void get_file_path(char *dest, char *refUnicode)
{
	int i, pos = 0;
	for (i = 0; i < 0x100 && refUnicode[2 * i] != 0; i++)
	{
		if (refUnicode[2 * i] & 0x80)
			dest[pos++] = '_';
		else
			dest[pos++] = refUnicode[2*i];
	}
	dest[pos] = 0;
}

void try_create_directory(char *file_path)
{
	int pos = 0;
	while (file_path[pos] != 0)
	{
		if (file_path[pos] == '/')
		{
			file_path[pos] = 0;
			if (!PathIsDirectory(file_path)) // shlwapi.h
				CreateDirectory(file_path, NULL);
			file_path[pos] = '/';
		}
		pos++;
	}
}

int run_extract(char *path)
{
	std::fstream test;
	test.open(path, std::ios::in| std::ios::binary);

	size_t file_size;
	test.seekg(0, std::ios::end);
	file_size = test.tellg();
	test.seekg(0, std::ios::beg);

	unsigned char *buffer = new unsigned char [file_size];
	test.read((char*)buffer, file_size);
	test.close();

	jrm_rom_header *ptrHeader = (jrm_rom_header*)buffer;

	unsigned long indexLen = sizeof(unsigned long) + ptrHeader->count_index * sizeof(jrm_rom_file_index);
	unsigned char *index = new unsigned char [indexLen];

	uncompress(index, &indexLen, (const Bytef*)(buffer + ptrHeader->pos_index), ptrHeader->size_index); // use zlib

	jrm_rom_file_index *ptrIndex = (jrm_rom_file_index*)index;
	unsigned char key = 0;
	char *path_out = new char[0x100];
	unsigned long ypos = 0, x = 0;

	for (x=0; x<ptrHeader->count_index; x++)
	{
		key = (ptrIndex->key & 0xff) ^ (ptrIndex->key >> 8 & 0xff) ^ (ptrIndex->key >> 16 & 0xff) ^ (ptrIndex->key >> 24 & 0xff);
		get_file_path(path_out, ptrIndex->file_path);
		// printf("%04d: file_name = %s\n      file_start = 0x%08x, file_size = 0x%08x, key = 0x%02x\n", x, path_out, ptrIndex->pos_start, ptrIndex->size_file, key);
		printf("%s: %d bytes.\n", path_out, ptrIndex->size_file);

		for (ypos = 0; ypos < ptrIndex->pos_delta; ypos++)
			buffer[ypos + ptrIndex->pos_start] ^= key;

		try_create_directory(path_out);
		test.open(path_out, std::ios::binary|std::ios::out);
		test.write((char*)buffer + ptrIndex->pos_start, ptrIndex->pos_delta);
		test.close();


		ptrIndex++;
	}

	delete[]path_out;
	delete[]index;
	delete[]buffer;

	return 0;
}
