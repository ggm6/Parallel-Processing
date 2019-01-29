/*

Name: Garrett McDonnell

NOTE: opening file as "nqmq.dat" in getLocations function, prompted for # of threads

Compilation: g++ barrier.cc Lab4.cpp Lab4 -o -pthread

Run: ./Lab4

Floyd's implementation found at Geeksforgeeks.org, but altered to facilitate multithreading

*/


#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <thread>
#include <list>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include "barrier.h"

using namespace std;
using namespace std::chrono;

struct path
{
	vector<int> cities;
	int distance;
};

int getLocations(vector<string>& v);
void createAdjacencyMatrix(vector <vector<int> >& v,const int& numCities);
void printMatrix(const vector <vector<int> >& v);
void printPaths(const vector <vector<path> >& v);
void floydAlg(vector <vector<path> >* paths, vector <vector<int> >* dist, const int& numThreads, const int& thread, Barrier* barr);
void initDistMat(vector <vector<int> >& dist,const vector <vector<int> >& v);
int getThreads();
void initPaths(vector <vector<path> >& paths,const vector <vector<int> >& dist);
void menu(int& startNum, int& finishNum, const vector<string>& locations);
void getPath(const int& start, const int& finish, const vector <vector<path> >& paths, const vector<string>& locations);
void buildPathRecursively(const vector <vector<path> >& paths, vector<int>& newVec, const int& start, int finish);

ifstream file;






int main()
{
	int threadCount,numCities,start,finish;
	vector<string> locations;
	vector <vector<int> > adjacencyMatrix,distMat;
	vector <vector<path> > paths;

	numCities=getLocations(locations);

	threadCount=getThreads();
	thread threads[threadCount];

	createAdjacencyMatrix(adjacencyMatrix,numCities);
	initDistMat(distMat,adjacencyMatrix);

	void* useless;
	Barrier barr(threadCount,CLONE_THREAD,useless);

	auto startTime = high_resolution_clock::now();  // Timer (and algorithm) begin after adjacency matrix and distance matrix initialized

	initPaths(paths,distMat);

	if (threadCount>1)
	{
		for (int i=0; i<threadCount; ++i)
			threads[i]=thread(floydAlg,&paths,&distMat,threadCount,i,&barr);
		for (int i=0; i<threadCount; ++i)
			threads[i].join();
	}
	else
		floydAlg(&paths,&distMat,1,0,&barr);

	auto stopTime=high_resolution_clock::now();
	auto duration=duration_cast<microseconds> (stopTime-startTime);

	if (threadCount>1)
		cout << "Algorithm running time for " << threadCount << " threads: ";
	else
		cout << "Algorithm sequential running time: ";

	cout << duration_cast<microseconds> (duration).count() << " microseconds" << endl;

	string loop="Y";
	while (loop=="Y" || loop=="y")
	{

		menu(start,finish,locations);

		getPath(start,finish,paths,locations);

		cout << "Another path (y/n)?  ";
		cin >> loop;
		cin.clear();
		cin.ignore(INT8_MAX,'\n');
		cout << endl;
	}

	return 0;
}






int getLocations(vector<string>& v)
{
	file.open("nqmq.dat");
	string line,numCities;
	getline(file,numCities);
	int num=atoi(numCities.c_str());

	for (int i=0; i<num; ++i)
	{
		getline(file,line);
		line.erase(remove(line.begin(),line.end(),'\r'),line.end());
		line.erase(remove(line.begin(),line.end(),'\n'),line.end());
		v.push_back(line);
	}
	return num;
}

void createAdjacencyMatrix(vector <vector<int> >& v, const int& numCities)
{
	v.resize(numCities);
	for (int i=0; i<numCities; ++i)
		v[i].resize(numCities,100000);
	
	string line;
	string escapeString="-1 0 0";
	while (getline(file,line))
	{
		line.erase(remove(line.begin(),line.end(),'\r'),line.end());
		line.erase(remove(line.begin(),line.end(),'\n'),line.end());
		if (line==escapeString)
			break;

		string city1, city2, miles;
		size_t pos=line.find(' ');
		city1=line.substr(0,pos);
		size_t pos2=line.find(' ', pos+1);
		city2=line.substr(pos+1,pos2-2);
		miles=line.substr(pos2+1,line.length());
		int city1Num,city2Num,milesNum;
		city1Num=atoi(city1.c_str());
		city2Num=atoi(city2.c_str());
		milesNum=atoi(miles.c_str());
		v[city1Num][city2Num]=milesNum;
		v[city2Num][city1Num]=milesNum;
		v[city1Num][city1Num]=0;
		v[city2Num][city2Num]=0;
	}

	file.close();
}

void printMatrix(const vector<vector<int> >& v)
{
	for (int i=0; i<v.size(); ++i)
	{
		for (int j=0; j<v[i].size(); ++j)
			if (v[i][j]==100000)
				cout << "max" << ' ';
			else
				cout << v[i][j] << ' ';
		cout << endl;
	}
}

void floydAlg(vector <vector<path> >* paths, vector <vector<int> >* dist, const int& numThreads, const int& thread, Barrier* barr)
{
	int stepCount=numThreads;
    for (int k = 0; k < dist->size(); k++) 
    { 
        for (int i = thread; i < dist->size(); i+=stepCount) 
        { 
            for (int j = 0; j < (*dist)[i].size(); j++) 
            { 
                if ((*dist)[i][k] + (*dist)[k][j] < (*dist)[i][j])
                { 
                    (*dist)[i][j] = (*dist)[i][k] + (*dist)[k][j];
                    auto it=(*paths)[i][j].cities.begin();
                    vector<int> newVec;
                    buildPathRecursively(*paths,newVec,*it,k);
                    while (!(*paths)[i][j].cities.empty())
                    	(*paths)[i][j].cities.pop_back();
                    (*paths)[i][j].cities=newVec;
                    (*paths)[i][j].cities.push_back(j);
                    (*paths)[i][j].distance=(*dist)[i][j];
                } 
            }
        } 
        barr->Wait();
    }
}

int getThreads()
{
	string threads;
	cout << "How many threads would you like to use (1/2/4/7)?  ";
	getline(cin,threads);
	cout << endl;
	int threadCount=atoi(threads.c_str());
	return threadCount;
}

void initDistMat(vector <vector<int> >& dist,const vector <vector<int> >& v)
{
	dist.resize(v.size());
	for (int i=0; i<v.size(); ++i)
		dist[i].resize(v[i].size());

	for (int i = 0; i < v.size(); i++)
	{
        for (int j = 0; j < v[i].size(); j++) 
            dist[i][j] = v[i][j];
	}
}

void initPaths(vector <vector<path> >& paths,const vector <vector<int> >& dist)
{
	paths.resize(dist.size());
	for (int i=0; i<dist.size(); ++i)
		paths[i].resize(dist[i].size());

	for (int i = 0; i < dist.size(); i++)
	{
        for (int j = 0; j < dist[i].size(); j++) 
        {
           	paths[i][j].distance = dist[i][j];
           	paths[i][j].cities.push_back(i),paths[i][j].cities.push_back(j);
        }
	}
}

void printPaths(const vector <vector<path> >& v)
{
	for (int i = 0; i < v.size(); i++)
	{
        for (int j = 0; j < v.size(); j++) 
        {
           	for (auto it=v[i][j].cities.begin(); it!=v[i][j].cities.end(); ++it)
           	{
           		if (it!=v[i][j].cities.end()-1)
           			cout << *it << "->";
           		else
           			cout << *it << ": ";
           	}
           	if (v[i][j].cities.size()>1)
           		cout << v[i][j].distance << endl;
           	
        }
	}
}

void menu(int& startNum, int& finishNum, const vector<string>& locations)
{
	cout << endl << "NQMQ MENU" << endl << "-------------------------" << endl;
	int i=1;
	for (auto it=locations.begin(); it!=locations.end(); ++it)
	{
		cout << i << ". " << *it << endl;
		++i;
	}

	string start,finish;
	cout << endl << "Path from (using number): ";
	cin >> start;
	cout << endl << "Path to (using number): ";
	cin >> finish;
	cout << endl;
	startNum=atoi(start.c_str())-1;
	finishNum=atoi(finish.c_str())-1;
}

void getPath(const int& start, const int& finish, const vector <vector<path> >& paths, const vector<string>& locations)
{
	cout << " ** " << locations[start] << " to " << locations[finish] << " ** " << endl << endl;
	cout << "Path: " << endl;
	for (auto it=paths[start][finish].cities.begin(); it!=paths[start][finish].cities.end(); ++it)
	{
		if (it==paths[start][finish].cities.end()-1)
			cout << locations[*it];
		else
			cout << locations[*it] << " -> ";
	} 
	cout << endl << "Distance: " << paths[start][finish].distance << " miles" << endl << endl;
}

void buildPathRecursively(const vector <vector<path> >& paths, vector<int>& newVec, const int& start, int finish)
{
	auto rit=paths[start][finish].cities.rbegin();
	++rit;
	if (start!=finish) {
		buildPathRecursively(paths,newVec,start,*rit);
	}
	newVec.push_back(finish);
}