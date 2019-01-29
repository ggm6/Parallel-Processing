/*
// Part 1. C++11 threads test
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
using namespace std;

int thread_count;
mutex my_mutex;

void hello(int my_rank) {
   my_mutex.lock();
   cout << "Hello from thread " << my_rank << " of " << thread_count << endl;
   my_mutex.unlock();
}

int main (int argc, char* argv[ ]) {
   int t;
   vector<thread> thread_handles;
   thread_count = strtol (argv[1], NULL, 10);
   for (t = 0; t < thread_count; t++)
      thread_handles.push_back(std::thread(hello, t));
   for (t = 0; t < thread_count; t++) thread_handles[t].join();
   return 0;
}

// end Part 1
*/
/*
// Part 2. OpenMP test
#include <iostream>
#include <stdlib.h>
#include <omp.h>
using namespace std;

int thread_count;

void hello() {
   int my_rank = omp_get_thread_num();
   thread_count = omp_get_num_threads();
#  pragma omp critical
   cout << "Hello from thread " << my_rank << " of " << thread_count << endl;
}

int main(int argc, char* argv[]) {
   thread_count = strtol(argv[1], NULL, 10);
  
#  pragma omp parallel num_threads(thread_count)
   hello();
   
   return 0;
}
// end Part 2
*/
/*
// Part 3. MPI test
#include <iostream>
#include <mpi.h>
using namespace std;

void hello() {
   int my_rank, thread_count;
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &thread_count);
   cout << "Hello from thread " << my_rank << " of " << thread_count << endl;
}

int main(int argc, char* argv[]) {
   MPI_Init(&argc, &argv);
   hello();
   MPI_Finalize();
   
   return 0;
}
// end Part 3
*/
