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

using namespace std;

double alpha = 0.15;
double threshold = 1e-6;

class PPR_yasuo{
public:
    PPR_yasuo(){}

    // 读取所有点（包括虚拟点）
    void read_nodes(string compress_vertex_path) {
        ifstream inFile(compress_vertex_path);
        if(!inFile){
            cout << "open file failed. " << compress_vertex_path << endl;
            exit(0);
        }
        inFile >> all_nodes_num >> real_nodes_num;  // 文件第一行表示所有顶点个数
        virtual_node_start = real_nodes_num;

        residues.resize(all_nodes_num); 
        residues[0] = 1.0;
        tmp_residues.resize(all_nodes_num);
        reserves.resize(real_nodes_num);
        weights.resize(all_nodes_num);
        out_adj_nums.resize(all_nodes_num);
        start_pos.resize(all_nodes_num + 1);

        int u, weight_, out_adj_num_;
        while (inFile >> u >> weight_ >> out_adj_num_)
        {
            weights[u] = weight_;
            out_adj_nums[u] = out_adj_num_;
        }
        inFile.close();
    }

    //读边
    void getNodesVec(string compress_edge_path) {
        
        int u, v;

        //----------------------------------------------------------------
        int line_count = 0; //边数，最好能在压缩图的时候就算出来
        ifstream inFile0(compress_edge_path);
        if (!inFile0) {
            cout << "open file " << compress_edge_path << " failed." << endl;
        }
        while (inFile0 >> u >> v) {
            line_count++;
        }
        cout <<"line_count: " << line_count << endl;
        inFile0.close();
        //------------------------------------------------------------------

        start_pos[all_nodes_num] = line_count;
        edges.resize(line_count);
        int curr_index = -1;
        int pos = 0;
        ifstream inFile(compress_edge_path);
        if (!inFile) {
            cout << "open file " << compress_edge_path << " failed." << endl;
        }
        while (inFile >> u >> v) {
            edges.emplace_back(v);
            if (u != curr_index)
            {
                curr_index = u;
                start_pos[u] = pos;
            }
            pos++;
        }
        inFile.close();
    }

    int run(string filename)
    {
        // double start_read_file = clock();
        string v_path = "/home/zyj/zhou/PPR-proj/out/" + filename + ".v"; // 压缩图点文件，包含超点和源顶点的对应关系
        string e_path = "/home/zyj/zhou/PPR-proj/out/" + filename + ".e"; // 压缩图边文件，超点之间的边
        read_nodes(v_path);
        getNodesVec(e_path);

        cout << "vertex_num=" << all_nodes_num << ", virtual_node_start=" << virtual_node_start << endl;
        //cout << "read file time: " << (clock() - start_read_file) / CLOCKS_PER_SEC << endl;

        int step = 0; //统计迭代几轮
        double pre_time = 0;
        // double sum_time = 0;
        double start = clock();
        // double real_compute_time = 0;
        // double virtual_compute_time = 0;
        // int real_edge = 0;
        // int virtual_edge = 0;
        
        int node_below_thr = 0;
        int pos_0, pos_1;
        int out_degree;
        while (1)
        {
            node_below_thr = 0; // residue值低于阈值的点的个数

            // real_compute_time -= clock();

            for(int i = 0; i < virtual_node_start; i++)
            {
                pos_0 = start_pos[i];
                pos_1 = start_pos[i + 1];
                out_degree = pos_1 - pos_0;

                double teleport_value = (1 - alpha) * residues[i] / out_adj_nums[i];
                // int outDegree = r_node.outNodes.size();
                for (int j = 0; j < out_degree; j++)
                {
                    int curr_node = edges[pos_0 + j];
                    tmp_residues[curr_node] += teleport_value * weights[curr_node];
                    // Node &v = nodes[r_node.outNodes[i]];
                    // v.tmp_residue += teleportVal * v.weight;
                    // jisuancishu++;
                }
                // r_node.reserve += alpha * r_node.residue;
                // r_node.residue = 0;
                reserves[i] += alpha * residues[i];
            }

            // real_compute_time += clock();

            // virtual_compute_time -= clock();

            for(int i = virtual_node_start; i < all_nodes_num; i++)
            {
                // Node &v_node = nodes[i];
                pos_0 = start_pos[i];
                pos_1 = start_pos[i + 1];
                out_degree = pos_1 - pos_0;

                // int outDegree = v_node.outNodes.size();
                double teleport_value = tmp_residues[i] / out_degree;
                for (int j = 0; j < out_degree; j++)
                {
                    // Node &v = nodes[v_node.outNodes[i]];
                    // v.tmp_residue += teleportVal;
                    int curr_node = edges[pos_0 + j];
                    tmp_residues[curr_node] += teleport_value;
                    jisuancishu++;
                }
                // v_node.tmp_residue = 0;
                tmp_residues[i] = 0;
            }

            // virtual_compute_time += clock();

            // for (int i = 0; i < virtual_node_start; i++) {
            //     Node& r_node = nodes[i];
            //     r_node.residue = r_node.tmp_residue;
            //     r_node.tmp_residue = 0;

            //     if (r_node.residue / r_node.outAdjNum < threshold)
            //         nodeBelowThr++;
            // }

            for(int i = 0; i < virtual_node_start; i++)
            {
                pos_0 = start_pos[i];
                pos_1 = start_pos[i + 1];
                out_degree = pos_1 - pos_0;

                residues[i] = tmp_residues[i];
                tmp_residues[i] = 0;

                if (residues[i] / out_degree < threshold)
                    node_below_thr++;
            }

            if(node_below_thr == virtual_node_start)
                break;

            step++;
            node_below_thr = 0;
        }
        pre_time = (clock()-start) / CLOCKS_PER_SEC;
        cout << "step=" << step << ", Compressed graph convergence" << endl;
        cout << "computing time: " << pre_time << "s" << endl;
        cout << "计算次数: " << jisuancishu << endl;
        // cout << "real time: " << real_compute_time/CLOCKS_PER_SEC << "s" << endl;
        // cout << "virtual time: " << virtual_compute_time/CLOCKS_PER_SEC << "s" << endl;
        // cout << "real edge: " << real_edge << endl;
        // cout << "virtual edge: " << virtual_edge << endl;

        // 测试: 输出压缩计算的结果
        // cout << "\nout path: " << outPath_compress << endl;
        // ofstream fout_com(outPath_compress);
        // for(int i = 0; i < virtual_node_start; i++)
        //     fout_com << vertex_reverse_map[i] << "点，residue=" << nodes[i].residue << "，reverse=" << nodes[i].reserve << "\n";
        // fout_com.close();
        

        string outPath = "./out/ppr_yasuo2.txt";
        cout << "out path: " << outPath << endl;
        ofstream fout(outPath);
        for(int i = 0; i < real_nodes_num; i++)
        {
            fout << i << " residue: " << residues[i] << " reserve: " << reserves[i] << endl;
        }
        fout.close();
    }

    ~PPR_yasuo() {}
public:
    // vector<Node> nodes;
    int virtual_node_start; // 虚拟点的第一个id
    int all_nodes_num; // 所有点的个数
    int real_nodes_num; // 真实点的个数
    long long int jisuancishu = 0;
    vector<double> residues;
    vector<double> tmp_residues;
    vector<double> reserves;
    vector<int> weights;
    vector<int> out_adj_nums;
    vector<int> start_pos;
    vector<int> edges;

};

int main(int argc, char const *argv[])
{
    // 运行命令: ./a.out 0.0000001 ./input/p2p-31_new.e p2p-31
    //threshold = atof(argv[1]);
    threshold = 1e-6;
    PPR_yasuo ppry = PPR_yasuo();
    //string edge_new_path(argv[2]);
    //string edge_new_path = "";
    //string filename(argv[3]);
    string filename = "google_yasuo";
    // double start = clock();
    ppry.run(filename);
    // double finish = clock();
    // cout << "computing time: " << (finish - start) / CLOCKS_PER_SEC << "s\n";
    return 0;
}
