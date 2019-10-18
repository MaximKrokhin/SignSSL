#include "multithreadHasher.h"
#include "openssl/md5.h"
#include <string>

multithreadHasher::multithreadHasher(int numberOfThreads, string inputFilePath, string outputFilePath, int chunkSize) :
	_numbofThreads(numberOfThreads), _currThread(0), _isDone(numberOfThreads, true),
	_result(0), _blockSize(chunkSize), _threads(numberOfThreads) {
	_inputfileStream.open(inputFilePath);
	_outputfileStream.open(outputFilePath);
}

multithreadHasher::~multithreadHasher()
{
	_inputfileStream.close();
	_outputfileStream.close();
}

void multithreadHasher::CalculateHashThread(string buffer, int num_of_thread) {
	MD5_CTX c;
	unsigned char md5_buf[MD5_DIGEST_LENGTH];
	MD5_Update(&c, &buffer, _blockSize);
	MD5_Final(md5_buf, &c);
	_mtx.lock();
	for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		_result += md5_buf[i];
	}
	_mtx.unlock();
	_isDone[num_of_thread] = true;
}

size_t multithreadHasher::CalculateHash()
{
	if (!_outputfileStream) {
		throw invalid_argument("cannot create output file.");
	}
	if (_inputfileStream.is_open()) {
		_inputfileStream.seekg(0, _inputfileStream.end);
		auto length = _inputfileStream.tellg();
		_inputfileStream.seekg(0, _inputfileStream.beg);
		std::string buffer(_blockSize, ' ');
		for (auto i = 0; i < length; i = i + _blockSize) {
			_inputfileStream.read(&buffer[0], _blockSize);
			while (true) {
				_currThread++;
				if (_currThread == 4) {
					_currThread = 0;
				}
				if (_isDone[_currThread]) {
					if (_threads[_currThread].joinable()) {
						_threads[_currThread].join();
					}
					_isDone[_currThread] = false;
					_threads[_currThread] = thread(&multithreadHasher::CalculateHashThread, this, buffer, _currThread);
					break;
				}
			}
		}
		for (auto i = 0; i < _numbofThreads; i++) {
			if (_threads[i].joinable()) {
				_threads[i].join();
			}
		}
		_outputfileStream << _result;
		return _result;
	}
	else {
		throw std::invalid_argument("received bad input file.");
	}
}