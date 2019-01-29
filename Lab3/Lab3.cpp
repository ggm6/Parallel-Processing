#include <iostream>
#include <mpi.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

typedef int boolArr[16][16];
typedef int boolArr2[8][8];

class arrPair
{
	public:
		boolArr2 arr;
		int process;

		arrPair(boolArr2 x, int rank)
		{
			for (int i=0; i<8; ++i)
			{
				for (int j=0; j<8; ++j)
				{
					arr[i][j]=x[i][j];
				}
			}

			process=rank;
		}

		arrPair()
		{
			for (int i=0; i<8; ++i)
			{
				for (int j=0; j<8; ++j)
				{
					arr[i][j]=-1;
				}
			}

			process=-1;
		}
};

void file2arr(int (&arr)[16][16], const char* fileName);
void printArr(const int arr[16][16]);
void printArr2(const int arr[8][8]);
void printArr3(const int arr[10][10]);
void finalPrint(const int arr[16][16]);
void nextGen(int (&arr)[8][8], const int bigArr[16][16], int rank);
void initSmallArr(const int (&arr)[16][16], int (&smallArr)[8][8], int rank);
void swapIfNecessary(char** argv);
int getNeighborCount(const int (&arr)[10][10], int m, int n);
void arrayOfArrays2Array(int (&arr)[16][16], arrPair arrayOfArrays[4]);

int main(int argc, char* argv[])
{	// MPI startup
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// Declare new MPI datatype for 16x16 array
	MPI_Datatype boolArrType;
	MPI_Type_contiguous(sizeof(boolArr)/sizeof(int), MPI_INT, &boolArrType);
	MPI_Type_commit(&boolArrType);

	// Declare new MPI dataype for 8x8 array
	MPI_Datatype boolArrType2;
	MPI_Type_contiguous(sizeof(boolArr2)/sizeof(int), MPI_INT, &boolArrType2);
	MPI_Type_commit(&boolArrType2);

	// declare new MPI datatype for arrPair class, which contains 8x8 array and an int for rank
	MPI_Datatype pairType;
	MPI_Type_contiguous(sizeof(arrPair)/sizeof(int), MPI_INT, &pairType);
	MPI_Type_commit(&pairType);

	int loop;
	boolArr arr;  // Takes command line argument for number of generations (can be 1st or 2nd argument) and txt file, and creates 16x16 array
	istringstream(argv[1]) >> loop;
	if (!loop)
	{
		istringstream(argv[2]) >> loop;
		file2arr(arr,argv[1]);
	}
	else
		file2arr(arr,argv[2]);

	while (loop>0)
	{
		if (rank==0)
			cout << endl << loop << endl << endl;
		if (rank==0)
			printArr(arr);
		boolArr2 smallArr;
		initSmallArr(arr,smallArr, rank);  // Initializes 8x8 array from section of the 16x16
		nextGen(smallArr,arr,rank);  // Calculates the next gen for 8x8 array

		arrPair myPair(smallArr,rank);  // myPair object created using 8x8 array and rank
		arrPair arrayOfArrays[4];

		MPI_Gather(&myPair,1, pairType,arrayOfArrays, 1, pairType, 0, MPI_COMM_WORLD); // All 8x8 arrays gathered into a size 4 array of 2D  arrays
		if (rank==0) // Rank 0 creates new 16x16 array from the array of 2D arrays, then broadcasts new 16x16 to other ranks
			arrayOfArrays2Array(arr,arrayOfArrays);
		MPI_Bcast(&arr,1, boolArrType, 0, MPI_COMM_WORLD);
		--loop;
	}
		if (rank==0)
			finalPrint(arr);  // Prints remaining living cells after game has ended

		MPI_Barrier(MPI_COMM_WORLD);


	MPI_Finalize();
	return 0;
}

void file2arr(int (&arr)[16][16], const char* fileName)
{
	for (int i=0; i<16; ++i)
	{
		for (int j=0; j<16; ++j)
		{
			arr[i][j]=0;
		}
	}

	ifstream file;
	file.open(fileName);
	string line;
	while (getline(file,line))
	{
		if (line!="-1 0") 
		{
			size_t pos=line.find(' ');
			string xStr=line.substr(0,pos);
			string yStr=line.substr(pos+1,line.length());
			int xInt;
			istringstream(xStr) >> xInt;
			int yInt;
			istringstream(yStr) >> yInt;
			arr[xInt-1][yInt-1]=1;
		}
	}
	file.close();
}

void printArr(const int arr[16][16])
{
	cout << ' ';
	int tracker=1;
	for (int i=1; i<=16; ++i)
	{
		if (i<16)
			cout << ' ' << tracker;
		else
			cout << ' ' << tracker << endl;
		if (i==8)
			tracker=0;
		++tracker;
	}
	for (int i=0; i<16; ++i)
	{
		if (i<9)
				cout << ' ';
		for (int j=0; j<16; ++j)
		{
			if (j==0)
				cout << i+1;
			cout << (arr[i][j]?'#':'-') << ' ';
		}
		cout << endl;
	}
}

void printArr2(const int arr[8][8])
{
	cout << ' ';
	int tracker=1;
	for (int i=1; i<=8; ++i)
	{
		if (i<8)
			cout << ' ' << tracker;
		else
			cout << ' ' << tracker << endl;
		if (i==8)
			tracker=0;
		++tracker;

	}
	for (int i=0; i<8; ++i)
	{
		for (int j=0; j<8; ++j)
		{
			if (j==0)
				cout << i+1 << ' ';
			cout << (arr[i][j]?'#':'-') << ' ';
		}
		cout << endl;
	}
}

void printArr3(const int arr[10][10])
{
	cout << ' ';
	int tracker=1;
	for (int i=1; i<=10; ++i)
	{
		if (i<10)
			cout << ' ' << tracker;
		else
			cout << ' ' << tracker << endl;
		if (i==5)
			tracker=0;
		++tracker;
	}
	for (int i=0; i<10; ++i)
	{
		if (i<9)
				cout << ' ';
		for (int j=0; j<10; ++j)
		{
			if (j==0)
				cout << i+1;
			if (arr[i][j]==1)
				cout << '#' << ' ';
			else if (arr[i][j]==0)
				cout << '-' << ' ';
			else
				cout << '|' << ' ';
		}
		cout << endl;
	}
}

void nextGen(int (&arr)[8][8], const int bigArr[16][16], int rank)
{	
	int newArr[10][10];

	{
		for (int i=0; i<10; ++i)
		{
			for (int j=0; j<10; ++j)
				newArr[i][j]=-1;
		}
	}

	if (rank==0)
	{
		for (int i=0; i<9; ++i)
		{
			for (int j=0; j<9; ++j)
			{
				newArr[i+1][j+1]=bigArr[i][j];
			}
		}
		//printArr3(newArr);
	}
	else if (rank==1)
	{
		for (int i=0; i<9; ++i)
		{
			for (int j=7; j<16; ++j)
			{
				newArr[i+1][j-7]=bigArr[i][j];
			}
		}
		//printArr3(newArr);
	}
	else if (rank==2)
	{
		for (int i=7; i<16; ++i)
		{
			for (int j=0; j<9; ++j)
			{
				newArr[i-7][j+1]=bigArr[i][j];
			}
		}
		//printArr3(newArr);
	}
	else if (rank==3)
	{
		for (int i=7; i<16; ++i)
		{
			for (int j=7; j<16; ++j)
			{
				newArr[i-7][j-7]=bigArr[i][j];
			}
		}
		//printArr3(newArr);
	}
	MPI_Barrier(MPI_COMM_WORLD);

 	// I check all the neighbors of every cell in the new array, and update the old array based upon GoL rules
	for (int i=1; i<9; ++i)
	{
		for (int j=1; j<9; ++j)
		{

			if (newArr[i][j]==0)
			{
				if (getNeighborCount(newArr,i,j)==3)
					arr[i-1][j-1]=1;
			}
			else if (newArr[i][j]==1)
			{
				if (getNeighborCount(newArr,i,j)<2 || getNeighborCount(newArr,i,j)>3)
					arr[i-1][j-1]=0;
			}
		}
	}
}

void swapIfNecessary(char** argv)
{
	if (isdigit(*argv[2]))
	{
		char* temp=argv[2];
		argv[2]=argv[1];
		argv[1]=temp;
	}
}

int getNeighborCount(const int (&arr)[10][10], int m, int n)
{
	int neighborCount=0;
	if (arr[m-1][n-1]==1)
		++neighborCount;
	if (arr[m-1][n]==1)
		++neighborCount;
	if (arr[m-1][n+1]==1)
		++neighborCount;
	if (arr[m][n-1]==1)
		++neighborCount;
	if (arr[m][n+1]==1)
		++neighborCount;
	if (arr[m+1][n-1]==1)
		++neighborCount;
	if (arr[m+1][n]==1)
		++neighborCount;
	if (arr[m+1][n+1]==1)
		++neighborCount;

	return neighborCount;
}

void initSmallArr(const int (&arr)[16][16], int (&smallArr)[8][8], int rank)
{

	for (int i=0; i<8; ++i)
	{
		for (int j=0; j<8; ++j)
		{
			smallArr[i][j]=-1;
		}
	} 

	if (rank==0)
	{
		for (int i=0; i<8; ++i)
		{
			for (int j=0; j<8; ++j)
			{
				smallArr[i][j]=arr[i][j];
			}
		}
	}
	else if (rank==1)
	{
		for (int i=0; i<8; ++i)
		{
			for (int j=0; j<8; ++j)
			{
				smallArr[i][j]=arr[i][j+8];
			}
		}
	}
	else if (rank==2)
	{
		for (int i=0; i<8; ++i)
		{
			for (int j=0; j<8; ++j)
			{
				smallArr[i][j]=arr[i+8][j];
			}
		}
	}
	else if (rank==3)
	{
		for (int i=0; i<8; ++i)
		{
			for (int j=0; j<8; ++j)
			{
				smallArr[i][j]=arr[i+8][j+8];
			}
		}
	}
}

void arrayOfArrays2Array(int (&arr)[16][16], arrPair arrayOfArrays[4])
{
	for (int i=0; i<4; ++i)
	{
		for (int j=0; j<8; ++j)
		{
			for (int k=0; k<8; ++k)
			{	
				if (i==0)
					arr[j][k]=arrayOfArrays[i].arr[j][k];
				else if (i==1)
					arr[j][k+8]=arrayOfArrays[i].arr[j][k];
				else if (i==2)
					arr[j+8][k]=arrayOfArrays[i].arr[j][k];
				else if (i==3)
					arr[j+8][k+8]=arrayOfArrays[i].arr[j][k];
			}
		}
	}
}

void finalPrint(const int arr[16][16])
{
	cout << endl << "Remaining cells:" << endl << endl;
	for (int i=0; i<16; ++i)
	{
		for (int j=0; j<16; ++j)
		{
			cout << ' ' << arr[i][j] << ' ';
			if (arr[i][j]==1)
			{
				//cout << '\t' << i+1 << ' ' << j+1 << endl;
			}
		}
		cout << endl;
	}
	cout << endl;
}