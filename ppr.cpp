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
    vector<bool> is_active;
    is_active.resize(vertex_num, false);
    is_active[source] = true;
    // vector<double> residues_and_reserves;
    // residues_and_reserves.resize(vertex_num * 2);
    // residues_and_reserves[source] = 1.0;
    queue<int> active_nodes_queue;
    active_nodes_queue.push(source);
    double r_sum = 1.0;
    double start_computing_time = clock();
    long long int number_of_pushes = 0, previous_number_of_pushes = 0;
    double threshold_to_reject = l1_error / edge_num;
    // 开始迭代，根据公式，当【每个点】的【residue/出度<阈值】时停止迭代
    // 每个点的residue：有alpha部分转换为自己的reserve，有1-alpha部分平均传递给每个邻居
    // 如果还有点的【residue/出度>=阈值】，即active_nodes_queue不为空，就要继续迭代
    while(!active_nodes_queue.empty() && r_sum >= l1_error)
    {
        if (number_of_pushes - previous_number_of_pushes >= edge_num)
        {
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
        double curr_node_residue = residues[curr_node];
        double increment = curr_node_residue * (1 - alpha) / out_degree;
        reserves[curr_node] += alpha * curr_node_residue;
        residues[curr_node] = 0;
        r_sum -= alpha * curr_node_residue;
        for (int j = pos_0; j < pos_1; j++)
        {
            int curr_neighbour = edges[j];
            residues[curr_neighbour] += increment;
            if (residues[curr_neighbour] >= out_degree * threshold_to_reject && !is_active[curr_neighbour]) // 怎么判断邻居在获得增量以后会不会被激活？
            {
                is_active[curr_neighbour] = true;
                active_nodes_queue.push(curr_neighbour);
            }
        }
    }
    const size_t num_iter = number_of_pushes / edge_num;
    printf("#Iter:%s%lu\tr_sum:%.12f\tTime Used:%.4f\t#Pushes:%llu\n",
        (num_iter < 10 ? "0" : ""), num_iter, r_sum, (clock() - start_computing_time) / CLOCKS_PER_SEC, number_of_pushes);
    double computing_time = (clock() - start_computing_time) / CLOCKS_PER_SEC; // 输出2：计算时间
    printf("r_sum:%.12f\tTime Used:%.4f\n", r_sum, computing_time);
    string outPath = "./out/ppr3.txt";
    cout << "out path: " << outPath << endl;
    ofstream fout(outPath);
    for(int i = 0; i < vertex_num; i++)
    {
        fout << i << " residue: " << residues[i] << " reserve: " << reserves[i] << endl;
    }
    fout.close();
    return 0;
}
