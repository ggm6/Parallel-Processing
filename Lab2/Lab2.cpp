#include <iostream>
#include <mpi.h>
#include <ctime>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[])
{
	int rank, size;
	srand(time(0));
	int potatoTimer=12+rand()%30;
	int node=-1;
	bool markStart=true;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	while (potatoTimer>-1)
	{
		int prevNode=-1;
		if (rank==0 && markStart==true)
		{
			prevNode=0;
			node=rand()%12;
			while (node==0)
				node=rand()%12;
			cout << "Node 0 starting send off, passing to node " << node << '!' << endl;
			markStart=false;
		}
		MPI_Barrier(MPI_COMM_WORLD);
		if (rank==node)
		{
			prevNode=node;
			if (potatoTimer==0)
			{
                                cout << "Node " << rank << " is it, game over!" << endl << endl;
			}
			else
			{
				cout << "Node " << node << " has the potato, passing to node ";
				while (node==rank)
					node=rand()%12;
				cout << node << "." << endl;
			}

		}
		MPI_Barrier(MPI_COMM_WORLD);
		if (prevNode==rank)
		{
			for (int i=0; i<size; ++i)
                        {
                                if (i!=rank)
                                {
                                        MPI_Send(&node, 1, MPI_INT, i,0, MPI_COMM_WORLD);
                                }
                        }

		}
		else
                {       MPI_Status status;
                        MPI_Recv(&node, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
                }

		MPI_Barrier(MPI_COMM_WORLD);
		--potatoTimer;
	}

	MPI_Finalize();

	return 0;
}
