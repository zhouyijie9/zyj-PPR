#include <iostream>
#include "virtual_node_miner.hpp"
#include <string>
#include <vector>
#include <ctime>
#include <cstdio>
#include <sys/resource.h>


int main(int argc,char *argv[]) {
    auto start_com = time(nullptr);
    if (argc != 6) {
        printf("incorrect arguments.\n");
        printf("<input_graph> <output_graph_edge> <output_graph_vertex> <CLUSTER_THRESHOLD> <VIRTUAL_THRESHOLD>\n");
        abort();
    }
    std::string input_path(argv[1]);
    std::string output_path_e(argv[2]);
    std::string output_path_v(argv[3]);
    int CLUSTER_THRESHOLD = atoi(argv[4]);
    int VIRTUAL_THRESHOLD = atoi(argv[5]);

    const rlim_t kStackSize = 1 * 1024 * 1024 * 1024;
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < kStackSize)
        {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }

    virtual_node_miner vnminer(CLUSTER_THRESHOLD, VIRTUAL_THRESHOLD);
    double start_load_file = clock();
    vnminer.load_graph(input_path);
    std::cout << "load file time: " << (clock()-start_load_file) / CLOCKS_PER_SEC<< "s\n";
    double start = clock();
    vnminer.compress(20);
    // vnminer.increment_compress("/home/zyj/zhou/dataset/d.txt");
    vnminer.write_graph(output_path_e); // write edge file
    vnminer.computeX(); // compute x[v] v in V, The number of real nodes that can be reached from v using virtual edges
    vnminer.computeY(); // y[v]: v's real outadjsum.
    vnminer.write_vertex(output_path_v); //

    //std::cout << "compress time=" << time(nullptr) - start_com << std::endl;
    std::cout << "incremental compress time: " << (clock()-start) / CLOCKS_PER_SEC<< "s\n";

    return 0;
}