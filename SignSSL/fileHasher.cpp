#include <fstream>
#include <stdexcept>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>
#include <iostream>

#include "fileHasher.h"
#include "multithreadHasher.h"

using namespace std;

const int NUM_THREAD = 4;
atomic<bool> is_done[NUM_THREAD] = { true, true, true, true };
mutex mtx;

fileHasher::fileHasher(std::string filePathIn, std::string filePathOut, int blockSize)
{
	_filePathIn = filePathIn;
	_filePathOut = filePathOut;
	_blockSize = blockSize;
}

fileHasher::~fileHasher()
{
}

string fileHasher::Hasher()
{
	MD5_CTX c;
	ifstream fileStream;
	fileStream.open(_filePathIn);
	if (fileStream.is_open()) {
		fileStream.seekg(0, fileStream.end);
		auto length = fileStream.tellg();
		fileStream.seekg(0, fileStream.beg);
		string buffer(_blockSize, ' ');
		unsigned char md5_buf[MD5_DIGEST_LENGTH];
		string result;
		for (auto i = 0; i < length; i = i + _blockSize) {
			fileStream.read(&buffer[0], _blockSize);
			MD5_Update(&c, &buffer, _blockSize);
		}
		MD5_Final(md5_buf, &c);
		for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
				result += md5_buf[i];
		}
		return result;
	}
	else {
		throw invalid_argument("received bad input file.");
	}
}

void CalculateHashThread(size_t& result, string buffer, int num_of_thread, int _blockSize) {
	MD5_CTX c;
	unsigned char md5_buf[MD5_DIGEST_LENGTH];
	MD5_Update(&c, &buffer, _blockSize);
	MD5_Final(md5_buf, &c);
	mtx.lock();
	for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		result += md5_buf[i];
	}
	mtx.unlock();
	is_done[num_of_thread] = true;
}

size_t fileHasher::HasherMultithread()
{
	std::ifstream fileStream;
	fileStream.open(_filePathIn);
	if (fileStream.is_open()) {
		fileStream.seekg(0, fileStream.end);
		auto length = fileStream.tellg();
		fileStream.seekg(0, fileStream.beg);
		string buffer(_blockSize, ' ');
		size_t result = 0;
		thread threads[NUM_THREAD];
		int curr_thread = 0;
		for (auto i = 0; i < length; i = i + _blockSize) {
			fileStream.read(&buffer[0], _blockSize);
			while (true) {
				curr_thread++;
				if (curr_thread == 4) {
					curr_thread = 0;
				}
				if (is_done[curr_thread]) {
					if (threads[curr_thread].joinable()) {
						threads[curr_thread].join();
					}
					is_done[curr_thread] = false;
					threads[curr_thread] = thread(CalculateHashThread, ref(result), buffer, curr_thread,_blockSize);
					break;
				}
			}
		}
		for (auto i = 0; i < NUM_THREAD; i++) {
			if (threads[i].joinable()) {
				threads[i].join();
			}
		}
		return result;
	}
	else {
		throw std::invalid_argument("received bad input file.");
	}
}

size_t fileHasher::HasherMultithreadWithClass() {
	multithreadHasher multithreadHasherInstance(NUM_THREAD, _filePathIn, _filePathOut, _blockSize);
	return multithreadHasherInstance.CalculateHash();
}