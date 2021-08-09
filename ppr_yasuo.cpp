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

class PPR_yasuo{
public:
    PPR_yasuo(){}

    // 读取所有点（包括虚拟点）
    void read_nodes(string compress_vertex_path)
    {
        ifstream inFile(compress_vertex_path);
        if(!inFile) {
            cout << "open file failed. " << compress_vertex_path << endl;
            exit(0);
        }
        // 文件第一行表示所有顶点个数
        inFile >> all_nodes_num >> real_nodes_num;
        weights.resize(all_nodes_num);
        out_adj_nums.resize(all_nodes_num);
        int u, weight_, out_adj_num_;
        while (inFile >> u >> weight_ >> out_adj_num_) {
            weights[u] = weight_;
            out_adj_nums[u] = out_adj_num_;
        }
        inFile.close();
    }

    //读边
    void getNodesVec(string compress_edge_path)
    {
        int u, v;
        //----------------------------------------------------------------
        // int edge_num = 0; //边数，最好能在压缩图的时候就写进文件，这次压缩算法里没写进去，所以额外算一下
        ifstream inFile0(compress_edge_path);
        if (!inFile0) {
            cout << "open file " << compress_edge_path << " failed." << endl;
        }
        while (inFile0 >> u >> v) {
            edge_num++;
        }
        // cout <<"line_count: " << line_count << endl;
        inFile0.close();
        //------------------------------------------------------------------
        start_pos.resize(all_nodes_num + 1);
        start_pos[all_nodes_num] = edge_num;
        int curr_index = -1;
        int pos = 0;
        ifstream inFile(compress_edge_path);
        if (!inFile) {
            cout << "open file " << compress_edge_path << " failed." << endl;
        }
        while (inFile >> u >> v) {
            edges.emplace_back(v);
            if (u != curr_index) {
                curr_index = u;
                start_pos[u] = pos;
            }
            pos++;
        }
        inFile.close();
    }

    int run(string filename)
    {
        
        double alpha = 0.15;
        double l1_error = 1e-6;
        string v_path = "/home/zyj/zhou/PPR-proj/out/" + filename + ".v"; // 压缩图点文件
        string e_path = "/home/zyj/zhou/PPR-proj/out/" + filename + ".e"; // 压缩图边文件
        read_nodes(v_path);
        getNodesVec(e_path);
        residues.resize(all_nodes_num); 
        reserves.resize(real_nodes_num);
        int source = 0; //我想用文件中的第一个点作为源点，整个实验最后得到的是另外的点关于它的PPR值
        residues[source] = 1.0;
        printf("edge_num=%d\n", edge_num);
        double threshold_to_reject = l1_error / edge_num;
        vector<bool> is_active;
        is_active.resize(all_nodes_num);
        double r_sum = 1.0;
        queue<int> active_nodes_queue;
        active_nodes_queue.push(source);
        long long unsigned int number_of_pushes = 0, previous_number_of_pushes = 0;
        double start_computing_time = clock();
        while (!active_nodes_queue.empty() && r_sum >= l1_error) {
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
            double curr_residue = residues[curr_node];
            residues[curr_node] = 0;
            int out_adj_num_ = out_adj_nums[curr_node];
            // 以下对真实点和虚拟点分开讨论
            if(curr_node < real_nodes_num) { // 真实点
                double increment = (1 - alpha) * curr_residue / out_adj_num_;
                reserves[curr_node] += alpha * curr_residue;
                r_sum -= alpha * curr_residue;
                for(int j= pos_0; j < pos_1; j++) {
                    int curr_neighbour = edges[j];
                    // 对于邻居也要分真实点和虚拟点进行讨论
                    if(curr_neighbour < real_nodes_num) {
                        residues[curr_neighbour] += increment;
                        if(residues[curr_neighbour] >= out_degree * threshold_to_reject && !is_active[curr_neighbour]) {
                            is_active[curr_neighbour] = true;
                            active_nodes_queue.push(curr_neighbour);
                        }
                    }
                    else {
                        residues[curr_neighbour] += increment * weights[curr_neighbour];
                        if(!is_active[curr_neighbour]) {
                            is_active[curr_neighbour] = true;
                            active_nodes_queue.push(curr_neighbour);
                        }
                    }
                }
            }
            else { // 虚拟点，邻居全是真实点
                double increment = curr_residue / out_adj_num_;
                for(int j = pos_0; j < pos_1; j++) {
                    int curr_neighbour = edges[j];
                    residues[curr_neighbour] += increment;
                    if (residues[curr_neighbour] >= out_degree * threshold_to_reject && !is_active[curr_neighbour]) {
                        is_active[curr_neighbour] = true;
                        active_nodes_queue.push(curr_neighbour);
                    }
                }
            }
        }
        const size_t num_iter = number_of_pushes / edge_num;
        printf("#Iter:%s%lu\tr_sum:%.12f\tTime Used:%.4f\t#Pushes:%llu\n",
               (num_iter < 10 ? "0" : ""), num_iter, r_sum, (clock() - start_computing_time) / CLOCKS_PER_SEC, number_of_pushes);
        double computing_time = (clock() - start_computing_time) / CLOCKS_PER_SEC; // 输出2：计算时间
        printf("r_sum:%.12f\tTime Used:%.4f\n", r_sum, computing_time);
        string outPath = "./out/ppr_yasuo3.txt";
        cout << "out path: " << outPath << endl;
        // ofstream fout(outPath);
        // for(int i = 0; i < real_nodes_num; i++)
        // {
        //     fout << i << " residue: " << residues[i] << " reserve: " << reserves[i] << endl;
        // }
        // fout.close();
    }

    ~PPR_yasuo() {}
public:
    int all_nodes_num = 0; // 所有点的个数
    int real_nodes_num = 0; // 真实点的个数
    int edge_num = 0; // 所有边的条数
    vector<double> residues;
    vector<double> reserves;
    vector<int> weights;
    vector<int> out_adj_nums;
    vector<int> start_pos;
    vector<int> edges;
};

int main(int argc, char const *argv[])
{
    PPR_yasuo ppry = PPR_yasuo();
    string filename = "google_yasuo";
    ppry.run(filename);
    return 0;
}
