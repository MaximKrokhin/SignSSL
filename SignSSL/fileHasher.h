#pragma once

#include <string>
#include "openssl/md5.h"

using namespace std;

struct fileHasher
{
private:
	string _filePathIn, _filePathOut;
	int _blockSize;
public:
	fileHasher(string filePathIn, string filePathOut, int blockSize);
	~fileHasher();
	string Hasher();
	size_t HasherMultithread();
	size_t HasherMultithreadWithClass();
};