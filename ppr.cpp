//implement PPR
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <unordered_map>
#include <queue>
#include <stdlib.h>
using namespace std;

#define MAX_NODE_NUM 300000
#define alpha 0.15

int main()
{
    string edge_path = "/home/zyj/zhou/dataset/web-Google1.txt";
    double l1_error = 1e-6;
    int source = 0; //我想用文件中的第一个点作为源点，整个实验最后得到的是另外的点关于它的PPR值

    ifstream inFile(edge_path);
    if(!inFile) {
        cout << "open file " << edge_path << " failed." << endl;
        return 0;
    }
    int vertex_num, edge_num;
    inFile >> vertex_num >> edge_num;
    cout << "edge_num=" << edge_num << endl;
    int u, v;
    int curr_index = -1;
    int pos = 0;
    vector<int> start_pos;
    start_pos.resize(vertex_num + 1, 0);
    vector<int> edges;
    while (inFile >> u >> v) {
        edges.emplace_back(v);
        if (u != curr_index) {
            curr_index = u;
            start_pos[u] = pos;
        }
        pos++;
    }
    inFile.close();
    start_pos[vertex_num] = edge_num;
    vector<double> reserves;
    reserves.resize(vertex_num, 0);
    vector<double> residues;
    residues.resize(vertex_num, 0);
    residues[source] = 1.0;
    vector<double> tmp_residues;
    tmp_residues.resize(vertex_num, 0);
    double r_sum = 1.0;
    double threshold_to_reject = l1_error / edge_num;
    long long int push_num = 0, prev_push_num = 0;
    double start_computing_time = clock();
    while (r_sum >= l1_error) {
        // if(push_num - prev_push_num >= edge_num) {
        //     prev_push_num = push_num;
        //     size_t num_iter = push_num / edge_num;
        //     printf("#Iter:%s%lu\tr_sum:%.12f\tTime Used:%.4f\t#Pushes:%llu\n",
        //         (num_iter < 10 ? "0" : ""), num_iter, r_sum, (clock() - start_computing_time) / CLOCKS_PER_SEC, push_num);
        // }

        for(int i = 0; i < vertex_num; i++) { // 每个点的residue值进行传播，暂时累加到tmp_residue上
            if(residues[i] < threshold_to_reject)
                continue;
            int pos_0 = start_pos[i];
            int pos_1 = start_pos[i + 1];
            int out_degree = pos_1 - pos_0;
            push_num++;
            double increment = (1 - alpha) * residues[i] / out_degree;
            for (int j = pos_0; j < pos_1; j++)
            {
                int curr_neighbour = edges[j];
                tmp_residues[curr_neighbour] += increment;
            }
            reserves[i] += alpha * residues[i];
            r_sum -= alpha * residues[i];
            residues[i] = 0;
        }

        for(int i = 0; i < vertex_num; i++) { // 每个点的tmp_residue加到residue上
            if(tmp_residues[i] == 0)
                continue;
            residues[i] += tmp_residues[i];
            tmp_residues[i] = 0;
        }
    }
    // cout << "computing time: " << (clock() - start_computing_time) / CLOCKS_PER_SEC << endl;
    // cout << "r_sum: " << r_sum << endl;
    const size_t num_iter = push_num / edge_num;
    printf("#Iter:%s%lu\tr_sum:%.12f\tTime Used:%.4f\t#Pushes:%llu\n",
        (num_iter < 10 ? "0" : ""), num_iter, r_sum, (clock() - start_computing_time) / CLOCKS_PER_SEC, push_num);
    double computing_time = (clock() - start_computing_time) / CLOCKS_PER_SEC; // 输出2：计算时间
    printf("r_sum:%.12f\tTime Used:%.4f\n", r_sum, computing_time);

    // string outPath = "./out/ppr4.txt";
    // cout << "out path: " << outPath << endl;
    // ofstream fout(outPath);
    // for(int i = 0; i < vertex_num; i++)
    // {
    //     fout << i << " residue: " << residues[i] << " reserve: " << reserves[i] << endl;
    // }
    // fout.close();
    return 0;
}
