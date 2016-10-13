#include <iostream>
#include "fs.h"

int main(int argc, char* argv[])
{
	if (argc > 1)
		run_extract(argv[1]);

	return 0;
}