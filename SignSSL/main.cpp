#include <iostream>
#include "fileHasher.h"
using namespace std;
int main(int argc, char* argv[])
{
	if (argc != 4) {
		cout << "Incorrect parametres" << '\n';
		return 1;
	}
	string input_file = argv[1];
	string output_file = argv[2];
	int block_size = atoi(argv[3]);

	fileHasher file_hasher(input_file, output_file, block_size);
	try {
		size_t result = file_hasher.HasherMultithreadWithClass();
		cout  << result << '\n';
	}
	catch (const invalid_argument & e) {
		cout << "Bad argument: " << e.what() << '\n';
	}

	return 0;
}