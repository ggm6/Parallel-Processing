#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <sstream>
#include <chrono>
#include <algorithm>

using namespace std;

const int numThreads = 1000;
const int numParticles = 1000;
const int G = 1;
const int mj = 1;
const int numSteps = 100;
const double dt = .01;
const double minDist = 0.1;

void getBodies(string file,pair<int,int> positions[numParticles]);
void computeAcc(pair<int,int> positions[numParticles]);




int main(int argc,char* argv[]) 
{
	string file(argv[1]);
	pair<int,int> positions[numParticles];
	getBodies(file,positions);

	return 0;
}




void getBodies(string file,pair<int,int> positions[numParticles]) {
	ifstream inFile;
	inFile.open(file);
	string line;
	for (int i=0; i<numParticles; ++i) {
		getline(inFile,line);
		line.erase(remove(line.begin(),line.end(),'\r'),line.end());
		line.erase(remove(line.begin(),line.end(),'\n'),line.end());
		size_t pos=line.find(' ');
		string xStr=line.substr(0,pos);
		string yStr=line.substr(pos+1,line.length());
		int xInt;
		istringstream(xStr) >> xInt;
		int yInt;
		istringstream(yStr) >> yInt;
		positions[i]=make_pair(xInt,yInt);
	}
}

void computeAcc(pair<int,int> positions[numParticles]) {
	
}