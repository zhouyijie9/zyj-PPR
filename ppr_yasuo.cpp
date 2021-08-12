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

int main()
{
    double r_sum = 1.0;
    double alpha = 0.15;
    double l1_error = 1e-6;
    string v_path = "/home/zyj/zhou/PPR-proj/out/google_yasuo.v"; // 压缩图点文件
    string e_path = "/home/zyj/zhou/PPR-proj/out/google_yasuo.e"; // 压缩图边文件
    int all_nodes_num = 0;  // 所有点的个数
    int real_nodes_num = 0; // 真实点的个数
    int edge_num = 0; // 所有边的条数
    vector<double> residues;
    vector<double> tmp_residues;
    vector<double> reserves;
    vector<int> weights;
    vector<int> out_adj_nums;
    vector<int> start_pos;
    vector<int> edges;
    // cout << "顶点个数：" << all_nodes_num << "，真实点个数：" << all_nodes_num << "，边条数：" << edge_num << endl;
    // printf("顶点个数：%d，真实点个数：%d，边条数：%d", all_nodes_num, real_nodes_num, edge_num);

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

    // for (int i = 0; i < 20; i++) {
    //     cout << start_pos[i] << ' ';
    // }

    cout << "\n顶点个数：" << all_nodes_num << "，真实点个数：" << real_nodes_num << "，边条数：" << edge_num << endl;

    residues.resize(real_nodes_num);
    reserves.resize(real_nodes_num);
    tmp_residues.resize(all_nodes_num);
    double threshold_to_reject = l1_error / edge_num; // 这个edge_num应该是原图上边的数量，不是压缩图边的数量。先不改了
    int source = 0;
    residues[source] = 1.0;

    int iter_num = 0;
    long long int push_num = 0, prev_push_num = 0;
    double start_computing_time = clock();
    while(r_sum >= l1_error)
    {
        // if(push_num - prev_push_num >= edge_num) {
        //     prev_push_num = push_num;
        //     cout << "iter " << push_num / edge_num << "---------\n";
        // }
        // cout << "push_num=" << push_num++ << endl;

        for (int i = 0; i < real_nodes_num; i++) // 将真实点的residue进行push，先存在所有点的tmp_residue里面
        {
            // double &curr_residue = residues[i];
            if(residues[i] < threshold_to_reject)
                continue;
            else
            {
                int pos_0 = start_pos[i];
                int pos_1 = start_pos[i + 1];
                int out_degree = pos_1 - pos_0;
                push_num += out_degree;
                double increment = residues[i] * (1 - alpha) / out_adj_nums[i];
                // cout << residues[i] << "---" << 1 - alpha << "---" << out_adj_nums[i] << endl;
                // cout << "increment=" << increment << endl;
                for (int j = pos_0; j < pos_1; j++)
                {
                    int curr_neighbour = edges[j];
                    tmp_residues[curr_neighbour] += increment * weights[curr_neighbour];
                }
                reserves[i] += alpha * residues[i];
                r_sum -= alpha * residues[i];
                residues[i] = 0;
            }
        }
        // cout << tmp_residues[1] << endl;
        // cout << tmp_residues[2] << endl;
        // cout << tmp_residues[928349] << endl;
        // return 0;
        for (int i = 0; i < real_nodes_num; i++) // 把真实点的tmp_residue加到它们自身的residue上
        {
            if(tmp_residues[i] == 0)
                continue;
            // double &tmp_residue_ = tmp_residues[i];
            residues[i] += tmp_residues[i];
            tmp_residues[i] = 0;
        }
        
        for(int i = real_nodes_num; i < all_nodes_num; i++) // 把虚拟点的tmp_residue进行push，直接加到真实点的residue上
        {
            if(tmp_residues[i] == 0)
                continue;
            int pos_0 = start_pos[i];
            int pos_1 = start_pos[i + 1];
            int out_degree = pos_1 - pos_0;
            push_num += out_degree;
            // double &tmp_residue_ = tmp_residues[i];
            double increment = tmp_residues[i] / out_degree;
            for (int j = pos_0; j < pos_1; j++)
            {
                int curr_neighbour = edges[j];
                residues[curr_neighbour] += increment;
            }
            tmp_residues[i] = 0;
        }
    }
    cout << "computing time: " << (clock() - start_computing_time) / CLOCKS_PER_SEC << endl;
    // cout << "r_sum: " << r_sum << endl;
    string outPath = "./out/ppr_yasuo4.txt";
    cout << "out path: " << outPath << endl;
    ofstream fout(outPath);
    for (int i = 0; i < real_nodes_num; i++) {
        fout << i << " residue: " << residues[i] << " reserve: " << reserves[i] << endl;
    }
    fout.close();

    return 0;
}
