#include <iostream>
#include <cstdlib>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <vector>

#define PI 3.141592653589793

struct Point
{
    double x, y;
};

Point point() {
    Point p;
    p.x = (double)rand() / RAND_MAX;
    p.y = (double)rand() / RAND_MAX;
    return p;
}

double In_zone(Point p) {
    return ((double)p.x * p.x + p.y * p.y) <= 1;
}

int main(int argc, char** argv) {

    srand(time(0));

    int size, rank;
    long long points_to_calculate;
    int in_zone = 0;
    int total_points = std::atoi(argv[1]);
    Point* arr = new Point[total_points];

    
    MPI_Init(NULL, NULL);

    
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    
    int blocklengths[2] = { 1,1 };
    MPI_Datatype types[2] = { MPI_DOUBLE, MPI_DOUBLE };
    MPI_Datatype mpi_point_type;
    MPI_Aint offsets[2];

    offsets[0] = offsetof(Point, x);
    offsets[1] = offsetof(Point, y);

    MPI_Type_create_struct
    (2, blocklengths, offsets, types, &mpi_point_type);

    MPI_Type_commit(&mpi_point_type);
  
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
       
        for (int i = 0; i < total_points; i++) {
           
            arr[i] = point();
        }
        
        points_to_calculate = total_points;
    }
    else
        points_to_calculate = int((total_points / size) * rank);

   
    MPI_Bcast(arr, total_points, mpi_point_type, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    

    double starttime, endtime, pi;

    starttime = MPI_Wtime();

    for (int i = 0; i < points_to_calculate; i++)

        in_zone += In_zone(arr[i]);

    pi = (double)4.0 * in_zone / points_to_calculate;

    endtime = MPI_Wtime();

   


    std::cout << "Rank: " << rank << "\tPoints: " << points_to_calculate << "\tTime: " << endtime - starttime << "\tTime: " << endtime - starttime << "\tpi: " << pi << "\tError: " << fabs(pi - PI) << "\tIn_zone: " << in_zone << std::endl;
    
    delete arr;
   
    MPI_Finalize();
}

