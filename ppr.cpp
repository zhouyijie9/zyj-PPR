//implement PPR
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <unordered_map>
using namespace std;

#define MAX_NODE_NUM 300000
#define alpha 0.15
//#define threshold 1e-6

struct Node {
    vector<int> outNodes;
    double residue = 0;
    double tmp_residue = 0;
    double reserve = 0;
};

// int main(int argc, char const *argv[]) //会传入两个参数：argv[1]-edgepath和argv[2]-threshold
int main()
{
    // string edge_path = argv[1];
    // double threshold = atof(argv[2]);
    // string dest_path = argv[3];
    string edge_path = "/home/zyj/zhou/dataset/web-Google1.txt";
    double threshold = 1e-6;

    vector<Node> nodes; //nodes用来存放点向量
    int source = 0; //我想用文件中的第一个点作为源点，整个实验最后得到的是另外的点关于它的PPR值
    
    double start_read_file = clock();
    //读文件
    ifstream inFile(edge_path);
    if(!inFile)
    {
        cout << "open file " << edge_path << " failed." << endl;
        return 0;
    }

    int vertex_num, edge_num;
    inFile >> vertex_num >> edge_num;
    nodes.resize(vertex_num);
    
    int u, v;
    while(inFile >> u >> v)
    {
        nodes[u].outNodes.emplace_back(v);
    }
    inFile.close();
    nodes[0].residue = 1;

    double read_file_time = (clock() - start_read_file) / CLOCKS_PER_SEC; // 输出1：读文件时间
    
    //------------------------------------------------
    //每个点的residue随机赋值0~1
    // srand(time(NULL));
    // int N = 999; //精度为小数点后面3位
    // for(int i = 0; i < vertex_num; i++)
    // {
    //     nodes[vertex_map[i]].residue = rand() % (N + 1) / (float)(N + 1);
    //     cout << "点" << i << "的初始residue=" << nodes[vertex_map[i]].residue << endl;
    // }
    //------------------------------------------------

    int cnt = 0; // count the number of iterations
    long long int compute_cishu = 0;
    double start_computing = clock();

    // 开始迭代，根据公式，当每个点的【residue/出度<阈值】时停止迭代
    // 每个点的residue：有alpha部分转换为自己的reserve，有1-alpha部分平均传递给每个邻居
    while(1)
    {
        int nodeBelowThr = 0; // residue/出度 的值低于阈值的点个数

        for(int u = 0; u < vertex_num; ++u)
        {
            int outDegree = nodes[u].outNodes.size();
            double teleportValue = (1 - alpha) * nodes[u].residue / outDegree;
            for(int i = 0; i < outDegree; i++)
            {
                int v = nodes[u].outNodes[i];
                nodes[v].tmp_residue += teleportValue;
            }
            nodes[u].reserve += alpha * nodes[u].residue;
            nodes[u].residue = 0;
        }

        for(int u = 0; u < vertex_num; u++)
        {
            nodes[u].residue = nodes[u].tmp_residue;
            nodes[u].tmp_residue = 0;

            if(nodes[u].residue / nodes[u].outNodes.size() < threshold)
                nodeBelowThr++;
        }

        if(nodeBelowThr == vertex_num)
            break;

        cnt++;
    }

    double computing_time = (clock() - start_computing) / CLOCKS_PER_SEC; // 输出2：计算时间
    
    printf("%s%d\n", "step = ", cnt);
    cout << "加载文件时间：" << read_file_time << "\n计算时间" << computing_time << endl;
    // 将运行时间写入文件
    // string resultPath = "./out/result_ppr_origin.txt";
    // ofstream fout_1(resultPath, ios::app|ios::out);
    // fout_1 << "normal_graph_time:" << (clock()-start) / CLOCKS_PER_SEC << endl;
    // fout_1 << "normal_graph_step:" << cnt << endl;
    // fout_1.close();

    // string outPath = "./out/ppr.txt";
    // cout << "out path: " << outPath << endl;
    // ofstream fout(outPath);
    // for(int i = 0; i < vertex_num; i++)
    // {
    //     fout << vertex_reverse_map[i] << " residue: " << nodes[i].residue << " reserve: " << nodes[i].reserve << endl;
    // }

    // fout.close();
    // cout << "每个点的迭代结果：\n";
    // for(int i = 0; i < vertex_num; i++)  
    // {
    //     fout_1 << vertex_reverse_map[i] << " residue: " << nodes[i].residue << " reserve: " << nodes[i].reserve << endl;
    // }
    // fout_1.close();
    
    return 0;
}
