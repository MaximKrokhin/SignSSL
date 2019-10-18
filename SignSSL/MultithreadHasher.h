#pragma once

#include <future>
#include <mutex>
#include <fstream>

using namespace std;

struct multithreadHasher
{
private:
	int _numbofThreads;
	int _currThread;
	vector<bool> _isDone;
	vector<thread> _threads;
	mutex _mtx;
	int _result;
	ifstream _inputfileStream;
	ofstream _outputfileStream;
	int _blockSize;
public:
	multithreadHasher(int numberofThreads, string inputfilePath, string outputfilePath, int blockSize);
	~multithreadHasher();
	void CalculateHashThread(string buffer, int num_of_thread);
	size_t CalculateHash();
};