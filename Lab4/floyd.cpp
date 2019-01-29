/*
	Author : Jeremy Moore
	Date : 11/13/18
	IPP Lab 3 - Floyds Algorithm with C++ Threads
	Compilation : g++ -g -Wall -o floyd barrier.cc floyd.cpp -pthread -std=c++11
	Usage : ./main <data file>
*/

#include <iostream>
#include <vector>
#include <thread>  // C++ Threads
#include <fstream> // Used to read in input file
#include <string>
#include <ctime>
#include "barrier.h"
using namespace std;

struct Path
{
	vector<int> cities;
};

int threadcount; // Number of threads that will be used.
int cityAmount;   // Number of cities in the file.

// Prototype for floyd algorithm.
// TODO : Convert this to use a vector.
void floyd(vector< vector<int> >* dist, vector< vector<Path> >* Paths, Barrier* barrier, int cityAmount, int start_city, int end_city, int threads, int myThread);
void printMatrix(vector< vector<int> > matrix, int cityAmount);
int getThreadAmount();
void pathFinder(vector <vector<Path> > paths, vector<int>& v, int start, int finish);



int main(int argc, char* argv[])
{


	// Read in input file.
	fstream inputFile;
	inputFile.open(argv[1]);
	cout << endl << "Opening file : " << argv[1] << endl;

	// First integer is the amount of citites
	inputFile >> cityAmount;
	cout << "Number of cities : " << cityAmount << endl;

	string cityList[cityAmount]; // Holds the city name list.
	// int adjacenceyMatrix[cityAmount][cityAmount]; // Holds the distances from each city via a matrix.
	vector< vector<int> > adjacenceyMatrix(cityAmount, vector<int> (cityAmount, 0));

	// Read in city names
	for (int i = 0; i < cityAmount; i++)
	{ 
		inputFile >> cityList[i];
		// cout << cityList[i] << endl; // Uncomment for debugging.
	}

	// Read in distances to adjancencey matrix.
	int x, y, dist;
	while (!inputFile.eof())
	{

		inputFile >> x;
		inputFile >> y;
		inputFile >> dist;
		if (x != -1)
		{
			adjacenceyMatrix[x][y] = dist;
			adjacenceyMatrix[y][x] = dist;
		}
		else
			break;
		//cout << "x: " << x << "y: " << y << "dist : " << dist << endl;
		
	}

	// If a cell is empty. Fill it with a large number.
	for (int i = 0; i < cityAmount; i++)
		for (int j = 0; j < cityAmount; j++)
			if (adjacenceyMatrix[i][j] == NULL)
				{ adjacenceyMatrix[i][j] = 999999; }

	// Set middle values to 0
	for(int i = 0; i < cityAmount; i++)
		adjacenceyMatrix[i][i] = 0;

	// print matrix for debugging
	//printMatrix(adjacenceyMatrix, cityAmount);


	int start_city, end_city;
	string path[cityAmount]; // May need to change this.
	bool another_path = true;

	// Main loop
	while(another_path == true)
	{
		// Prompt the user with a nice menu.
		cout << "\n\nNQMQ Menu\n";
		cout << "---------\n";

		for(int i = 0; i < cityAmount; i++)
		{
			cout << i+1 << ". " << cityList[i] << endl;
		}

		cout << "\nPath from? ";
		cin >> start_city;

		cout << "To? ";
		cin >> end_city;
		cout << "\n";

		// Print which cities we are going to
		cout << "** " << cityList[start_city-1] << " to " << cityList[end_city-1] << " **" << endl;

		threadcount = getThreadAmount(); // Number of threads entered by user.
		thread threads[threadcount]; // Create and array of threads

		// Declare dist. vector that will store all of our distances.
		vector< vector<int> > dist(cityAmount, vector<int> (cityAmount, 0));
		void* unused;
		Barrier barrier(threadcount,CLONE_THREAD,unused);

		vector< vector<Path> > paths;
		paths.resize(cityAmount);
		for (int i=0; i<cityAmount; i++)
			paths[i].resize(cityAmount);

		for (int i=0; i<cityAmount; i++)
		{
			for (int j=0; j<cityAmount; j++)
				paths[i][j].cities.push_back(i), paths[i][j].cities.push_back(j);
		}

		clock_t time=clock();

		// Copy initililization matrix in to our dist matrix
		dist = adjacenceyMatrix;

		for (int i = 0; i < threadcount; i++)
			threads[i]=thread(floyd, &dist, &paths, &barrier, cityAmount, start_city, end_city, threadcount, i);
		for (int i = 0; i < threadcount; i++)
			threads[i].join(); // Join all threads back together.

		time=clock()-time;
		cout << "Time of algorithm: " << (double) time/CLOCKS_PER_SEC << " seconds" << endl;
	
		// TODO : Print visitied cities along the way
		cout << endl << "Path:" << endl;

		for (auto it=paths[start_city-1][end_city-1].cities.begin(); it!=paths[start_city-1][end_city-1].cities.end(); ++it)
		{
			int i=*it;
			if (it==paths[start_city-1][end_city-1].cities.end()-2)
				cout << cityList[i] << endl;
			else if (it==paths[start_city-1][end_city-1].cities.end()-1)
				cout << "Distance: " << *it << endl;
			else
				cout << cityList[i] << " -> ";
		}
		

		// Print out the distance
		
		// cout << "Distance: " << distance << endl; 


		// Ask the user if they want to do another path.
		char choice;
		cout << "Another path (y/n)? ";
		cin >> choice;

		if (choice == 'y')
			{ another_path = true; }
		else
			{ another_path = false; }
		cin.clear();
		cin.ignore(1000,'\n');

	}
	return 0;
}


void floyd(vector< vector<int> >* dist, vector< vector<Path> >* Paths, Barrier* barrier, int cityAmount, int start_city, int end_city, int threads, int myThread)
{

	int distance;

	// Floyds algorithm
	for (int k = 0; k < cityAmount; k++)
	{
		for (int i = myThread; i < cityAmount; i+=threads)
		{
			for (int j = 0; j < cityAmount; j++)
			{
				if ((*dist)[i][k] + (*dist)[k][j] < (*dist)[i][j])
				{
					(*dist)[i][j] = (*dist)[i][k] + (*dist)[k][j];
					vector<int> v;
                    pathFinder(*Paths,v,*(*Paths)[i][j].cities.begin(),k);
                    while (!(*Paths)[i][j].cities.empty())
                    	(*Paths)[i][j].cities.pop_back();
                    (*Paths)[i][j].cities=v;
                    (*Paths)[i][j].cities.push_back(j);
				}
			}
		}
		barrier->Wait();
	}

	barrier->Wait();  // Makes sure all distances for all threads have been updated before continuing

	// Print the new matrix for debugging
	//printMatrix(*dist, cityAmount);
	//barrier->Wait();

	if (myThread==0)
	{
		distance = (*dist)[start_city-1][end_city-1];
		//obj->cities += " -> " + to_string(end_city);
		//paths->push_back(end_city);
		//cout << obj->cities << endl;
		//cout << "Distance: " << distance << endl; 
		(*Paths)[start_city-1][end_city-1].cities.push_back(distance); // This contains the path from start_city to end_city, but the last element of vector is distance
	}
}

void printMatrix(vector< vector<int> > matrix, int cityAmount)
{
	for (int i = 0; i < cityAmount; i++)
	{
		cout << '\n';
		for (int j = 0; j < cityAmount; j++)
			{ cout << "[ " << matrix[i][j] << " ]"; }
	}
}

int getThreadAmount()
{
	// Get the amount of threads the user wants to use.
	int threadcount;
	cout << "How many threads do you wants to use? " << endl;
	cout << "Options: 1, 2, 4, 7" << endl;
	cin >> threadcount;

	return threadcount;
}

void pathFinder(vector <vector<Path> > paths, vector<int>& v, int start, int finish)
{
	auto it=--paths[start][finish].cities.end()-1;
	if (start!=finish) {
		pathFinder(paths,v,start,*it);
	}
	v.push_back(finish);
}