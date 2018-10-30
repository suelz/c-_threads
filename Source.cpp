#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <mutex>
#include <algorithm>

using namespace std;

//void fileToMemoryTransfer(char *fileName, char **data, size_t & numOfBytes) {
//	streampos begin, end;
//	ifstream inFile(fileName, ios::in | ios::binary | ios::ate);
//	if (!inFile)
//	{
//		cerr << "Cannot open " << fileName << endl;
//		inFile.close();
//		exit(1);
//	}
//	size_t size = inFile.tellg();
//	char * buffer = new  char[size];
//	inFile.seekg(0, ios::beg);
//	inFile.read(buffer, size);
//	inFile.close();
//	*data = buffer;
//	numOfBytes = size;
//}

int main() {


	ifstream inFile("test.txt", ios::in | ios::binary | ios::ate);

	if (!inFile) {
		cerr << "Cannot open " << endl;
		inFile.close();
		exit(1);
	}

	size_t length = inFile.tellg();
	char * buffer = new char[length];
	inFile.seekg(0, ios::beg);
	inFile.read(buffer, length);
	inFile.close();

	vector<size_t> global(256, 0);
	vector<thread> workers;
	int numThreads = thread::hardware_concurrency();

	int section = length / numThreads;
	int leftOver = length % numThreads;
	int start = 0;
	int end = section;
	mutex m;

	for (int t = 1; t <= numThreads; t++) {
		if (t == numThreads) {
			end += leftOver;
		}

		workers.push_back(thread([start, end, &m, &buffer, &global]() {
			for (int i = start; i < end; i++) {
				for (int j = 0; j < global.size(); j++) {
					if (buffer[i] == j) {
						global[j]++;
					}
				}
			}
		})
		);
		start = end;
		end = start + section;
	}

	for_each(workers.begin(), workers.end(), [](thread &t) {
		t.join();
	}
	);

	cout << "Run with one global histogram" << endl;

	for (int i = 0; i < global.size(); i++) {
		cout << i << ": " << global[i] << endl;
	}


	//---------------------------------------------------------------------------
	cout << "Run with local histograms" << endl;
	start = 0;
	end = section;

}