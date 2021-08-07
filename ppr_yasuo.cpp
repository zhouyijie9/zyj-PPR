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
        if(!inFile)
        {
            cout << "open file failed. " << compress_vertex_path << endl;
            exit(0);
        }
        inFile >> all_nodes_num >> real_nodes_num;  // 文件第一行表示所有顶点个数
        weights.resize(all_nodes_num);
        out_adj_nums.resize(all_nodes_num);
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
        // cout <<"line_count: " << line_count << endl;
        inFile0.close();
        //------------------------------------------------------------------
        start_pos.resize(all_nodes_num + 1);
        start_pos[line_count] = line_count;
        int curr_index = -1;
        int pos = 0;
        ifstream inFile(compress_edge_path);
        if (!inFile) {
            cout << "open file " << compress_edge_path << " failed." << endl;
        }
        int mmm = 0;
        while (inFile >> u >> v)
        {
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
        residues.resize(real_nodes_num); 
        reserves.resize(real_nodes_num);

        // cout << "vertex_num=" << all_nodes_num << ", virtual_node_start=" << virtual_node_start << endl;
        //cout << "read file time: " << (clock() - start_read_file) / CLOCKS_PER_SEC << endl;

        int step = 0; //统计迭代几轮
        double pre_time = 0;
        double start = clock();
        residues_and_reserves[0] = 1.0;
        
        int node_below_thr = 0;
        int pos_0, pos_1;
        int out_degree;
        while (1)
        {

            real_compute_time -= clock();

            for(int i = 0; i < virtual_node_start; i++)
            {
                pos_0 = start_pos_and_out_adj_nums[i * 2];
                pos_1 = start_pos_and_out_adj_nums[i * 2 + 2];
                out_degree = pos_1 - pos_0;
                double r_and_r = residues_and_reserves[i * 2];

                double one_teleport_value = (1 - alpha) * r_and_r / start_pos_and_out_adj_nums[i*2+1]; // 要传播的一份residue。真实点获得一份，虚拟点获得weight份
                for (int j = 0; j < out_degree; j++)
                {
                    int curr_node_index = edges[pos_0 + j];
                    tmp_residues[curr_node_index] += one_teleport_value * weights[curr_node_index];
                    real_jisuancishu++;
                }
                residues_and_reserves[i*2+1] += alpha * r_and_r;
                //residues[i] = 0;
            }

            real_compute_time += clock();

            virtual_compute_time -= clock();

            for(int i = virtual_node_start; i < all_nodes_num; i++)
            {
                pos_0 = start_pos_and_out_adj_nums[i * 2];
                pos_1 = start_pos_and_out_adj_nums[i * 2 + 2];
                out_degree = pos_1 - pos_0;

                double teleport_value = tmp_residues[i] / out_degree; // 没有虚拟点连虚拟点，虚拟点连的都是真实点，每点分一份tmp_residue就行
                for (int j = 0; j < out_degree; j++)
                {
                    int curr_node_index = edges[pos_0 + j];
                    tmp_residues[curr_node_index] += teleport_value;
                    virtual_jisuancishu++;
                }
                tmp_residues[i] = 0;
            }
            virtual_compute_time += clock();

            for(int i = 0; i < virtual_node_start; i++)
            {
                pos_0 = start_pos_and_out_adj_nums[i * 2];
                pos_1 = start_pos_and_out_adj_nums[i * 2 + 2];
                out_degree = pos_1 - pos_0;

                residues_and_reserves[i*2] = tmp_residues[i];
                tmp_residues[i] = 0;

                if (residues_and_reserves[i*2] / start_pos_and_out_adj_nums[i * 2 + 1] < threshold)
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
        cout << "real计算次数: " << real_jisuancishu << endl;
        cout << "virtual计算次数: " << virtual_jisuancishu << endl;
        cout << "real time: " << real_compute_time/CLOCKS_PER_SEC << "s" << endl;
        cout << "virtual time: " << virtual_compute_time/CLOCKS_PER_SEC << "s" << endl;
        cout << "real计算时间/计算次数=" << real_compute_time / CLOCKS_PER_SEC / real_jisuancishu << endl;
        cout << "virtual计算时间/计算次数=" << virtual_compute_time / CLOCKS_PER_SEC / virtual_jisuancishu << endl;

        
        // cout << "real edge: " << real_edge << endl;
        // cout << "virtual edge: " << virtual_edge << endl;

        // 测试: 输出压缩计算的结果
        // cout << "\nout path: " << outPath_compress << endl;
        // ofstream fout_com(outPath_compress);
        // for(int i = 0; i < virtual_node_start; i++)
        //     fout_com << vertex_reverse_map[i] << "点，residue=" << nodes[i].residue << "，reverse=" << nodes[i].reserve << "\n";
        // fout_com.close();
        

        // string outPath = "./out/ppr_yasuo2.txt";
        // cout << "out path: " << outPath << endl;
        // ofstream fout(outPath);
        // for(int i = 0; i < real_nodes_num; i++)
        // {
        //     fout << i << " residue: " << residues_and_reserves[i*2] << " reserve: " << residues_and_reserves[i*2+1] << endl;
        // }
        // fout.close();
    }

    ~PPR_yasuo() {}
public:
    // vector<Node> nodes;
    int virtual_node_start; // 虚拟点的第一个id
    int all_nodes_num; // 所有点的个数
    int real_nodes_num; // 真实点的个数
    long long int jisuancishu = 0;
    long long int real_jisuancishu = 0;
    long long int virtual_jisuancishu = 0;
    vector<double> residues;
    vector<double> reserves;
    vector<int> weights;
    vector<int> out_adj_nums;
    vector<int> start_pos;
    vector<int> edges;
    double real_compute_time;
    double virtual_compute_time;
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
