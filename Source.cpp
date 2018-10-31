#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

void fileToMemoryTransfer(char *fileName, char **data, size_t & numOfBytes) {
	streampos begin, end;
	ifstream inFile(fileName, ios::in | ios::binary | ios::ate);
	if (!inFile)
	{
		cerr << "Cannot open " << fileName << endl;
		inFile.close();
		exit(1);
	}
	size_t size = inFile.tellg();
	char * buffer = new  char[size];
	inFile.seekg(0, ios::beg);
	inFile.read(buffer, size);
	inFile.close();
	*data = buffer;
	numOfBytes = size;
}


int main(int argc, char** argv) {

	char *test;
	size_t size;
	fileToMemoryTransfer(argv[1], &test, size);
	
	vector<size_t> global(256, 0);
	vector<thread> workers;
	int numThreads = thread::hardware_concurrency();

	int rows = size / numThreads;
	int extra = size % numThreads;
	int start = 0; // each thread does [start..end)
	int end = rows;
	

	for (int t = 1; t <= numThreads; t++) {
		if (t == numThreads) { // last thread does extra rows:
			end += extra;
		}

		workers.push_back(thread([start, end, &test, &global]() {
			for (int i = start; i < end; i++) {
				for (size_t j = 0; j < global.size(); j++) {
					if (test[i] == j) {
					global[j]++;
					}
				}
			}
		})
		);
		start = end;
		end = start + rows;
	}

	for_each(workers.begin(), workers.end(), [](thread &t) {
		t.join();
	});

	cout << "Run with one global histogram" << endl;

	for (size_t i = 0; i < global.size(); i++) {
		cout << i << ": " << global[i] << endl;
	}

	
}