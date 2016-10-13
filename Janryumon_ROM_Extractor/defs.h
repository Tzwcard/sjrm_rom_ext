struct jrm_rom_header
{
	char magic[4];
	unsigned long version;
	unsigned long var1;
	unsigned long pos_index;
	unsigned long size_index;
	unsigned long count_index;
};

struct jrm_rom_file_index
{
	char file_path[0x200];
	unsigned long pos_start;
	unsigned long pos_delta; // ?
	unsigned long size_file; // ?
	unsigned long unk[3];    // ?
	unsigned long key;       // maybe the hash, idk
};