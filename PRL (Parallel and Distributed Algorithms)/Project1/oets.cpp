/**
 * @author: Michal Ľaš (xlasmi00)
 * @brief: Odd-Even Transpozition sort
 * @file oets.cpp
 * @date: 26.03.2025
 */

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <stdio.h>


const char *NUMBERS_FILE = "numbers";
const int MASTER_ID = 0;

/* Returns True if given number is odd else return False */
inline bool is_odd(int x){
    return (x % 2) == 1;
}

/* Sends value to process with rank one greater */
inline void send_to_right_neighbor(unsigned char *value, int my_rank){
    MPI_Send(value, 1, MPI_BYTE, my_rank + 1, 0, MPI_COMM_WORLD);
}

/* Receives value from process with rank one smaller */
inline void recv_from_left_neighbor(unsigned char *value, int my_rank){
    MPI_Recv(value, 1, MPI_BYTE, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

/* Sends value to process with rank one smaller */
inline void send_to_left_neighbor(unsigned char *value, int my_rank){
    MPI_Send(value, 1, MPI_BYTE, my_rank - 1, 0, MPI_COMM_WORLD);
}

/* Receives value from process with rank one greater */
inline void recv_from_right_neighbor(unsigned char *value, int my_rank){
    MPI_Recv(value, 1, MPI_BYTE, my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}


/* Reads file with given name and specified length of the loaded data */
unsigned char *read_file(const char *filename, unsigned length){
    std::ifstream stream (filename, std::ifstream::binary);
    unsigned char *buffer = new unsigned char[length];
    stream.read((char*) buffer, length);
    
    if (!stream){
        std::cerr << "error: only " << stream.gcount() << " numbers were read!" << std::endl;
        buffer = nullptr;
    } else {
        stream.close();
    }

    return buffer;
}


/* Swap the two values if the value is greater than other */
void swap_if_greater(unsigned char *value, unsigned char *other){
    if (*value > *other){ // swap
        unsigned char tmp = *value;
        *value = *other;
        *other = tmp;
    }
}


int main(int argc, char *argv[]){

    int rank, num_processes;
    unsigned char *numbers = nullptr;
    unsigned char number;

    MPI_Init(&argc, &argv);

    int file_read_status = 1; // Assume success

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    /* Master reads the file with numbers */
    if (rank == MASTER_ID){
        numbers = read_file(NUMBERS_FILE, num_processes);
        if (!numbers) file_read_status = 0;
    }

    /* Broadcast result of the read file operation, in case of failure return -1 */
    MPI_Bcast(&file_read_status, 1, MPI_INT, MASTER_ID, MPI_COMM_WORLD);

    if (!file_read_status){
        MPI_Finalize();
        return -1;
    }

    /* Distribute numbers to all processes */
    MPI_Scatter(numbers, 1, MPI_BYTE, &number, 1, MPI_BYTE, MASTER_ID, MPI_COMM_WORLD);


    /* Main algorithm loop */
    int last_odd = 2 * (num_processes / 2) - 1;
    int last_even = 2 * ((num_processes - 1) / 2);
    int cycles_count = (int) ((num_processes / 2.0) + 0.5);
    
    for (int i = 0; i < cycles_count; i++){
        // Odd step
        if (is_odd(rank) && rank < last_even){              /// Odd ranks
            send_to_right_neighbor(&number, rank);
            recv_from_right_neighbor(&number, rank);
        } else if (rank != MASTER_ID && rank <= last_even){ /// Even ranks except Master
            unsigned char left_number;
            recv_from_left_neighbor(&left_number, rank);
            swap_if_greater(&left_number, &number);
            send_to_left_neighbor(&left_number, rank);
        }

        // Even step
        if (!is_odd(rank) && rank < last_odd){              /// Even ranks
            send_to_right_neighbor(&number, rank);
            recv_from_right_neighbor(&number, rank);
        } else if (rank <= last_odd){                       /// Odd ranks
            unsigned char left_number;
            recv_from_left_neighbor(&left_number, rank);
            swap_if_greater(&left_number, &number);
            send_to_left_neighbor(&left_number, rank);
        }
    }

    /* Collect numbers from all processes */
    MPI_Gather(&number, 1, MPI_BYTE, numbers, 1, MPI_BYTE, MASTER_ID, MPI_COMM_WORLD);    

    /* Print numbers and free allocated memory */
    if (rank == MASTER_ID){

        for (int i = 0; i < num_processes; i++){
            std::cout << (unsigned) numbers[i] << std::endl;
        }

        delete[] numbers;
    }

    MPI_Finalize();

    return 0;
}

/* END OF FILE */