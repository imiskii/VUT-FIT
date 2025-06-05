/**
 * @author: Michal Ľaš (xlasmi00)
 * @brief: Calculating the level of a binary tree vertex
 * @file vuv.cpp
 * @date: 02.05.2025
 */

#include <mpi.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>

const int MASTER_ID = 0;

/************************************************************************************/
/* =============================== Class definitions ============================== */
/************************************************************************************/

/// @brief Edge of tree
struct Edge {
    char from;
    char to;
    bool isReversed;
    int id;
};

/// @brief One item of adjacency list
struct AdjacencyListItem {
    Edge *edge;                 // the main edge
    Edge *reverse;              // reverse edge to main edge
    AdjacencyListItem *next;    // next item
};

/// @brief Adjacency list holding pointer to the first edge
class AdjacencyList {
public:
    AdjacencyListItem *head;

    AdjacencyList(AdjacencyListItem *item = nullptr) : head(item) {}


    void AddItem(Edge *forward, Edge *reverse) {
        auto *newItem = new AdjacencyListItem{forward, reverse, this->head};
        this->head = newItem;
    }

};


/************************************************************************************/
/* ============================== Main functionality ============================== */
/************************************************************************************/


/// @brief Creates Edge items from string of characters which represent a binary tree
/// @param rawTree string representing a binary tree
/// @return Vector or initialized edges
std::vector<Edge> createEdges(std::string &rawTree) {
    unsigned num_edges = 2 * (rawTree.size() - 1);
    int edge_id = 0;
    std::vector<Edge> edges;
    edges.reserve(num_edges);

    for (unsigned i = 0; i < rawTree.size(); ++i) {
        unsigned left = i * 2 + 1;
        unsigned right = i * 2 + 2;
        if (left < rawTree.size()) { // Left child
            edges.push_back({ rawTree.at(i), rawTree.at(left), false, edge_id });
            edges.push_back({ rawTree.at(left), rawTree.at(i), true, (edge_id + 1) });
            edge_id += 2;
        }
        if (right < rawTree.size()){ // Right child
            edges.push_back({ rawTree.at(i), rawTree.at(right), false, edge_id});
            edges.push_back({ rawTree.at(right), rawTree.at(i), true, (edge_id + 1) });
            edge_id += 2;
        }
    }
    return edges;
}

/// @brief Creates an hash table of adjacency lists (one list for each vertex/node)
/// @param edges from which the adjacency list will be created
/// @return hash table mapping each vertex to its adjacency list
std::unordered_map<char, AdjacencyList> createAdjacencyList(std::vector<Edge> &edges) {
    std::unordered_map<char, AdjacencyList> adjList;
    
    for (unsigned i = 0; i < edges.size(); i += 2){
        auto &forward = edges.at(i);
        auto &reverse = edges.at(i+1);
        adjList[forward.from].AddItem(&forward, &reverse);
        adjList[forward.to].AddItem(&reverse, &forward);
    }
    return adjList;
}

/// @brief Finds the successor in Euler tour
/// @param myEdge Edge for wich a successor will be found
/// @param adjList Adjacency list
/// @return index of the successor edge in edge list (list of all edges from which was created adjacency list)
int computeEulerSuccessor(const Edge &myEdge, const std::unordered_map<char, AdjacencyList> &adjList) {
    // Pick the reverse edge
    const AdjacencyListItem *revItem = adjList.at(myEdge.to).head;
    while (revItem != nullptr) {
        if (revItem->edge->to == myEdge.from) { break; }
        revItem = revItem->next;
    }

    if (revItem->next != nullptr) {
        return revItem->next->edge->id;
    } else {
        return adjList.at(revItem->edge->from).head->edge->id;
    }
}

/// @brief Computes (int) rounded value of log2(x)
int log2ceil(int x) {
    return static_cast<int>(std::ceil(std::log2(x)));
}

/// @brief Computes suffix sum of an edge
/// @param rank index of the edge (process id)
/// @param size number of edges (number of processes)
/// @param myVal value/weight of the edge
/// @param mySuccessor successor of the edge (calculated from Euler)
/// @return Suffix sum of the edge
int suffixSum(int rank, int size, int myVal, int mySuccessor) {
    MPI_Win valWin, succWin;
    MPI_Win_create(&myVal, sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &valWin);
    MPI_Win_create(&mySuccessor, sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &succWin);

    for (unsigned k = 0; k < log2ceil(size); ++k) {
        int recvVal = 0, recvSucc = rank;

        // Synchronize before accessing windows
        MPI_Win_fence(0, valWin);
        MPI_Win_fence(0, succWin);

        if (mySuccessor != rank) {
            // Get Val[Succ[i]]
            MPI_Get(&recvVal, 1, MPI_INT, mySuccessor, 0, 1, MPI_INT, valWin);
            // Get Succ[Succ[i]]
            MPI_Get(&recvSucc, 1, MPI_INT, mySuccessor, 0, 1, MPI_INT, succWin);
        }

        MPI_Win_fence(0, valWin);
        MPI_Win_fence(0, succWin);

        if (mySuccessor != rank) {
            myVal += recvVal;
            mySuccessor = recvSucc;
        }
    }

    MPI_Win_free(&valWin);
    MPI_Win_free(&succWin);

    return myVal;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <binary_tree>\n";
        return 1;
    }

    // Tree with only one node
    std::string input = argv[1];
    if (input.size() == 1) {
        std::cout << input.at(0) << ":0" << std::endl;
        return 0;
    }
    
    int rank, size;
    char nodeVal;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<Edge> edges;
    edges.resize(size); // reserve space for edges
    std::unordered_map<char, AdjacencyList> adjList;

    /* Master read tree create edges */
    if (rank == MASTER_ID) {
        input = argv[1];
        if (size != 2 * (input.size() - 1)) {
            std::cerr << "Input string too short!" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        edges = createEdges(input);
    }

    /* ========================== Adjacency List ========================== */
    
    /* Broadcast edges to all processes */
    MPI_Bcast(edges.data(), sizeof(Edge) * size, MPI_BYTE, MASTER_ID, MPI_COMM_WORLD);
    Edge myEdge = edges.at(rank); // Each process pick edge based on rank
    adjList = createAdjacencyList(edges); // Each process creates AdjacencyList
    

    /* ============================ Euler tour ============================ */

    int mySuccessor = computeEulerSuccessor(myEdge, adjList);
    if (rank == MASTER_ID) { // Master breaks the circle in Euler tour
        mySuccessor = 0;
    }

    /* ============================ Suffix sum ============================ */

    /// Assign weights / values
    int myVal;
    if (mySuccessor == rank) {
        myVal = 0;
    } else {
        myVal = myEdge.isReversed ? +1 : -1;
    }

    myVal = suffixSum(rank, size, myVal, mySuccessor);

    if (!myEdge.isReversed) {
        myVal += 1;
    }

    /* ============================== Results ============================= */

    std::vector<int> suffixValues;
    if (rank == MASTER_ID) { suffixValues.resize(size); }
    MPI_Gather(&myVal, 1, MPI_INT, suffixValues.data(), 1, MPI_INT, MASTER_ID, MPI_COMM_WORLD);

    if (rank == MASTER_ID) {
        std::cout << input.at(0) << ":0,";
        for (unsigned i = 0; i < edges.size(); i += 2) {
            if (i != 0) { std::cout << ","; }
            std::cout << edges.at(i).to << ":" << suffixValues.at(i);
        }
        std::cout << std::endl;
    }

    MPI_Finalize();

    return 0;
}