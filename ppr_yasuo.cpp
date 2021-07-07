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

    int outAdjNum = 0; // 出度：即真实连接的点的个数。比如0连了一个真实点1，一个虚拟点2；2连了三个真实点。那0的outAdjNum就是4
    int weight = 1; // 如果是真实点则为1，虚拟点则为其通过虚拟边可以到达的真实点的个数
};

class PPR_yasuo{
public:
    PPR_yasuo(){}

    //读边，把每个点的信息（outNodes，reserve，residue）存到nodes里
    void getNodesVec(string compress_edge_path) {
        int u, v;
        int flag = 0;
        int source = 0; //把读到的第一个点作为原点

        ifstream inFile(compress_edge_path);
        if (!inFile) {
            cout << "open file " << compress_edge_path << " failed." << endl;
        }
        while (inFile >> u >> v) {
            if(flag == 0)
            {
                source = u;//用文件中的第一个点作为源点
                nodes[vertex_map[source]].residue = 1;
                flag = 1;
            }
            nodes[vertex_map[u]].outNodes.emplace_back(vertex_map[v]);
        }
        inFile.close();
        cout << "finish read file... " << compress_edge_path << endl;
    }

    // 读取所有点（包括虚拟点）
    void read_nodes(string compress_vertex_path) {
        ifstream inFile(compress_vertex_path);
        if(!inFile){
            cout << "open file failed. " << compress_vertex_path << endl;
            exit(0);
        }
        int all_nodes_num, real_nodes_num;
        inFile >> all_nodes_num >> real_nodes_num;  // 文件第一行表示所有顶点个数
        cout << "all_nodes_num=" << all_nodes_num << ", real_nodes_num=" << real_nodes_num << endl;
        // 申请node向量
        //vector<Node> nodes;
        nodes.resize(all_nodes_num);
        int u, weight_, outAdjNum_;
        while(inFile >> u >> weight_ >> outAdjNum_){
            if(vertex_map.find(u) == vertex_map.end()){
                vertex_map[u] = vertex_num;
                vertex_reverse_map[vertex_num] = u;
                u = vertex_num++;
            }
            else{
                u = vertex_map[u];
            }
            nodes[u].weight = weight_;
            nodes[u].outAdjNum = outAdjNum_;
            if(weight_ > 1) {
                // 虚拟点在文件最后，记录第一个出现的位置
                if(virtual_node_start == 0){
                    virtual_node_start = u;
                }
            }
        }
        //cout << "vertex_num=" << vertex_num << endl;
        inFile.close();
        cout << "finish read file... " << compress_vertex_path << endl;
    }

    int run(string filename)
    {
        string v_path = "/home/zyj/zhou/CLionProjects/untitled/out/" + filename + ".v"; // 压缩图点文件，包含超点和源顶点的对应关系
        string e_path = "/home/zyj/zhou/CLionProjects/untitled/out/" + filename + ".e"; // 压缩图边文件，超点之间的边
        //string edge_new_path = edge_new_path_;  // 原始图文件，即解压之后的图
        string outPath = "./out/ppr_yasuo.txt";   // 保存最终计算结果
        string outPath_compress = "./out/ppr_yasuo_com.txt";   // 保存第一次收敛结果
        read_nodes(v_path);
        getNodesVec(e_path);

        cout << "vertex_num=" << vertex_num << ", virtual_node_start=" << virtual_node_start << endl;
        // 将运行时间写入文件
        string resultPath = "./out/result.txt";
        ofstream fout_1(resultPath, ios::app);

        int step = 0; //统计迭代几轮
        // int increment_num = 1;
        double pre_time = 0;
        double sum_time = 0;
        double start = clock();
        int nodeBelowThr = 0;

        while(1)
        {
            nodeBelowThr = 0; // residue值低于阈值的点 个数

            // real node
            for(int i = 0; i < virtual_node_start; i++) {
                Node& node = nodes[i];
                int outDegree = node.outAdjNum;
                double teleportVal = (1 - alpha) * node.residue / outDegree;
                for(auto v : node.outNodes){ //
                    //nodes[v].reserve += teleportVal * nodes[v].weight; // 乘weight？
                    nodes[v].tmp_residue += teleportVal * nodes[v].weight;
                }
            }
            // virtual node
            for(int i = virtual_node_start; i < vertex_num; i++){
                Node& node = nodes[i];
                int outDegree = node.outAdjNum;
                double teleportVal = node.tmp_residue / outDegree;
                for(auto v : node.outNodes){
                    nodes[v].tmp_residue += teleportVal;
                }
                node.tmp_residue = 0;
            }

            for(int u = 0; u < virtual_node_start; u++)
            {
                nodes[u].residue = nodes[u].tmp_residue;
                nodes[u].tmp_residue = 0;

                if(nodes[u].residue / nodes[u].outNodes.size() < threshold)
                    nodeBelowThr++;
            }


            if(nodeBelowThr == vertex_num)
                break;

            step++;
        }
        cout << "step=" << step << ", Compressed graph convergence" << endl;
        pre_time = (clock()-start) / CLOCKS_PER_SEC;
        cout << "time: " << pre_time << "s" << endl;
        fout_1 << "compress_graph_1st_time:" << pre_time << endl;
        fout_1 << "compress_graph_1st_step:" << step << endl;
        fout_1.close();
        // 测试: 输出压缩计算的结果
        cout << "\nout path: " << outPath_compress << endl;
        ofstream fout_com(outPath_compress);
//        for(auto n_: nodes){
//            if(n_.weight == 1){ // 只写入真实点
//                fout_com << vertex_reverse_map[n_] << ' ' << n_.second.reserve << endl;
//            }
//        }
        for(int i = 0; i < virtual_node_start; i++)
            fout_com << vertex_reverse_map[i] << "点，residue=" << nodes[i].residue << "，reverse=" << nodes[i].reserve << "\n";
        fout_com.close();

    }
    ~PPR_yasuo() {}
//public:
    //unordered_map<int, Node> nodes; //nodes存放页面向量，大小为N
    vector<Node> nodes;
    unordered_map<int, int> vertex_map; //原id: 新id
    unordered_map<int, int> vertex_reverse_map; // 新id: 原id
    int virtual_node_start = 0; // 虚拟点的第一个id
    int vertex_num = 0; // 所有点的个数
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
    string filename = "twt";
    ppry.run(filename);
    return 0;
}
