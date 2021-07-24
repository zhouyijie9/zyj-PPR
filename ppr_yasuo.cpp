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

// 用于压缩图的节点
struct Node
{
    vector<int> outNodes; // 出邻居, 不能有重复的边
    double residue = 0;
    double tmp_residue = 0;
    double reserve = 0;

    int weight = 1;    // 如果是真实点则为1，虚拟点则为其通过虚拟边可以到达的真实点的个数
    int outAdjNum = 0; // 出度：即真实连接的点的个数。比如0连了一个真实点1，一个虚拟点2；2连了三个真实点。那0的outAdjNum就是4
};

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

        // 申请node向量
        nodes.resize(all_nodes_num);
        int u, weight_, outAdjNum_;

        while(inFile >> u >> weight_ >> outAdjNum_){
            nodes[u].weight = weight_;
            nodes[u].outAdjNum = outAdjNum_;
            // if(weight_ > 1) {
            //     // 虚拟点在文件最后，记录第一个出现的位置
            //     if(virtual_node_start == 0){
            //         virtual_node_start = u;
            //     }
            // }
        }
        //cout << "vertex_num=" << vertex_num << endl;
        inFile.close();
        // cout << "finish read file... " << compress_vertex_path << endl;
    }

    //读边，把每个点的信息（outNodes，reserve，residue）存到nodes里
    void getNodesVec(string compress_edge_path) {
        int u, v;
        nodes[0].residue = 1; // 第一个点是原点

        ifstream inFile(compress_edge_path);
        if (!inFile) {
            cout << "open file " << compress_edge_path << " failed." << endl;
        }
        while (inFile >> u >> v) {
            nodes[u].outNodes.emplace_back(v);
        }
        inFile.close();
        // cout << "finish read file... " << compress_edge_path << endl;
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
        double sum_time = 0;
        double start = clock();
        double real_compute_time = 0;
        double virtual_compute_time = 0;
        int real_edge = 0;
        int virtual_edge = 0;
        
        int nodeBelowThr = 0;
        while(1)
        {
            nodeBelowThr = 0; // residue值低于阈值的点的个数

            // real_compute_time -= clock();

            for(int i = 0; i < virtual_node_start; i++)
            {
                Node &r_node = nodes[i];
                double teleportVal = (1 - alpha) * r_node.residue / r_node.outAdjNum;
                int outDegree = r_node.outNodes.size();
                for (int i = 0; i < outDegree; i++)
                {
                    Node &v = nodes[r_node.outNodes[i]];
                    v.tmp_residue += teleportVal * v.weight;
                    jisuancishu++;
                }
                r_node.reserve += alpha * r_node.residue;
                r_node.residue = 0;
            }

            // real_compute_time += clock();

            // virtual_compute_time -= clock();

            for(int i = virtual_node_start; i < all_nodes_num; i++)
            {
                Node &v_node = nodes[i];
                int outDegree = v_node.outNodes.size();
                double teleportVal = v_node.tmp_residue / outDegree; //虚拟点的outAdjNum和weight是一样的
                for (int i = 0; i < outDegree; i++)
                {
                    Node &v = nodes[v_node.outNodes[i]];
                    v.tmp_residue += teleportVal;
                    jisuancishu++;
                }
                v_node.tmp_residue = 0;
            }

            // virtual_compute_time += clock();

            for (int i = 0; i < virtual_node_start; i++) {
                Node& r_node = nodes[i];
                r_node.residue = r_node.tmp_residue;
                r_node.tmp_residue = 0;

                if (r_node.residue / r_node.outAdjNum < threshold)
                    nodeBelowThr++;
            }

            if(nodeBelowThr == virtual_node_start)
                break;

            step++;
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
        

        string outPath = "./out/ppr_yasuo.txt";
        cout << "out path: " << outPath << endl;
        ofstream fout(outPath);
        for(int i = 0; i < real_nodes_num; i++)
        {
            fout << i << " residue: " << nodes[i].residue << " reserve: " << nodes[i].reserve << endl;
        }
    }

    ~PPR_yasuo() {}
public:
    vector<Node> nodes;
    int virtual_node_start; // 虚拟点的第一个id
    int all_nodes_num; // 所有点的个数
    int real_nodes_num; // 真实点的个数
    long long int jisuancishu = 0;

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
