#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <ctime>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <queue>
#include <stdlib.h>
using namespace std;

int main(int argc, char const *argv[])
{
    double alpha = 0.15;
    double l1_error = 1e-6;
    string v_path = "/home/zyj/zhou/PPR-proj/out/google_yasuo.v"; // 压缩图点文件
    string e_path = "/home/zyj/zhou/PPR-proj/out/google_yasuo.e"; // 压缩图边文件
    int all_nodes_num = 0;  // 所有点的个数
    int real_nodes_num = 0; // 真实点的个数
    int edge_num = 0; // 所有边的条数
    vector<double> residues;
    vector<double> reserves;
    vector<int> weights;
    vector<int> out_adj_nums;
    vector<int> start_pos;
    vector<int> edges;

    // 读.v文件
    ifstream inFile_v(v_path);
    if (!inFile_v) {
        cout << "open file " << v_path << " failed." << endl;
        exit(0);
    }
    inFile_v >> all_nodes_num >> real_nodes_num;
    weights.resize(all_nodes_num);
    out_adj_nums.resize(all_nodes_num);
    int node, weight_, out_adj_num_;
    while (inFile_v >> node >> weight_ >> out_adj_num_)
    {
        weights[node] = weight_;
        out_adj_nums[node] = out_adj_num_;
    }
    inFile_v.close();

    // 读.e文件
    int u, v;
    //----------------------------------------------------------------
    // int edge_num = 0; //边数，最好能在压缩图的时候就写进文件，这次压缩算法里没写进去，所以额外算一下
    ifstream inFile0(e_path);
    if (!inFile0) {
        cout << "open file " << e_path << " failed." << endl;
    }
    while (inFile0 >> u >> v) {
        edge_num++;
    }
    inFile0.close();
    //------------------------------------------------------------------
    start_pos.resize(all_nodes_num + 1);
    start_pos[all_nodes_num] = edge_num;
    int curr_index = -1;
    int pos = 0;
    ifstream inFile(e_path);
    while (inFile >> u >> v) {
        edges.emplace_back(v);
        if (u != curr_index) {
            curr_index = u;
            start_pos[u] = pos;
        }
        pos++;
    }
    inFile.close();
    //----------------------------------.e文件读完
    //想获得

    residues.resize(all_nodes_num);
    reserves.resize(real_nodes_num);
    int source = 0; //我想用文件中的第一个点作为源点，整个实验最后得到的是另外的点关于它的PPR值
    residues[source] = 1.0;
    printf("edge_num=%d\n", edge_num);
    double threshold_to_reject = l1_error / 5241298;
    vector<bool> is_active;
    is_active.resize(all_nodes_num);
    double r_sum = 1.0;
    queue<int> active_nodes_queue;
    active_nodes_queue.push(source);
    long long unsigned int number_of_pushes = 0, previous_number_of_pushes = 0;
    double start_computing_time = clock();
    long long int while_step = 0;
    while (!active_nodes_queue.empty() && r_sum >= l1_error) {
        while_step++;
        if (number_of_pushes - previous_number_of_pushes >= edge_num) {
            previous_number_of_pushes = number_of_pushes;
            size_t num_iter = number_of_pushes / edge_num;
            printf("#Iter:%s%lu\tr_sum:%.12f\tTime Used:%.4f\t#Pushes:%llu\n",
                   (num_iter < 10 ? "0" : ""), num_iter, r_sum, (clock() - start_computing_time) / CLOCKS_PER_SEC, number_of_pushes);
        }
        int curr_node = active_nodes_queue.front();
        active_nodes_queue.pop();
        is_active[curr_node] = false;
        int pos_0 = start_pos[curr_node];
        int pos_1 = start_pos[curr_node + 1];
        int out_degree = pos_1 - pos_0;
        number_of_pushes += out_degree;
        double curr_residue = residues[curr_node];
        residues[curr_node] = 0;
        double increment = 0;
        // 以下对真实点和虚拟点分开讨论
        if (curr_node < real_nodes_num) { // 真实点
            increment = (1 - alpha) * curr_residue / out_adj_nums[curr_node]; // 比在原图上多了访问out_adj_nums数组的时间
            reserves[curr_node] += alpha * curr_residue;
            r_sum -= alpha * curr_residue;
            for (int j = pos_0; j < pos_1; j++) {
                int curr_neighbour = edges[j];
                // 对于邻居也要分真实点和虚拟点进行讨论
                if (curr_neighbour < real_nodes_num) {
                    residues[curr_neighbour] += increment;
                    if (residues[curr_neighbour] >= out_degree * threshold_to_reject && !is_active[curr_neighbour]) {
                        is_active[curr_neighbour] = true;
                        active_nodes_queue.push(curr_neighbour);
                    }
                }
                else {
                    residues[curr_neighbour] += increment * weights[curr_neighbour]; // 比在原图上多了访问weights数组的时间
                    if (!is_active[curr_neighbour]) {
                        is_active[curr_neighbour] = true;
                        active_nodes_queue.push(curr_neighbour);
                    }
                }
            }
        }
        else { // 虚拟点，邻居全是真实点
            increment = curr_residue / out_degree;
            for (int j = pos_0; j < pos_1; j++) {
                int curr_neighbour = edges[j];
                residues[curr_neighbour] += increment;
                if (residues[curr_neighbour] >= out_degree * threshold_to_reject && !is_active[curr_neighbour]) {
                    is_active[curr_neighbour] = true;
                    active_nodes_queue.push(curr_neighbour);
                }
            }
        }
    }
    cout << "while_step="<<while_step << endl;
    const size_t num_iter = number_of_pushes / edge_num;
    printf("#Iter:%s%lu\tr_sum:%.12f\tTime Used:%.4f\t#Pushes:%llu\n",
           (num_iter < 10 ? "0" : ""), num_iter, r_sum, (clock() - start_computing_time) / CLOCKS_PER_SEC, number_of_pushes);
    double computing_time = (clock() - start_computing_time) / CLOCKS_PER_SEC; // 输出2：计算时间
    printf("r_sum:%.12f\tTime Used:%.4f\n", r_sum, computing_time);
    // string outPath = "./out/ppr_yasuo3.txt";
    // cout << "out path: " << outPath << endl;
    // ofstream fout(outPath);
    // for(int i = 0; i < real_nodes_num; i++)
    // {
    //     fout << i << " residue: " << residues[i] << " reserve: " << reserves[i] << endl;
    // }
    // fout.close();

    
    return 0;
}
