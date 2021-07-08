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

//int main(int argc, char const *argv[]) //会传入两个参数：argv[1]-edgepath和argv[2]-threshold
int main()
{
    //string edge_path = argv[1];
    //double threshold = atof(argv[2]);
    string edge_path = "/home/zyj/zhou/dataset/a.txt";
    double threshold = 1e-6;

    // int vertex_num = 0; // 所有点的个数
    //unordered_map<int, Node> nodes; //nodes存放页面向量
    vector<Node> nodes; //nodes用来存放点向量
    unordered_map<int, int> vertex_map; //原id: 新id
    unordered_map<int, int> vertex_reverse_map; // 新id: 原id
    int source = 0; //我想用文件中的第一个点作为源点，整个实验最后得到的是另外的点关于它的PPR值

    //先读文件
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
    int flag = 0;
    int curr_n = 0;
    while(inFile >> u >> v)
    {
        if(flag == 0)
        {
            source = u;//用文件中的第一个点作为源点
            flag = 1;
        }
        // 传说中的重新编号：对每个id做一个map映射，映射成0开始的连续编号，写入结果时再映射回最初编号
        if(vertex_map.find(u) == vertex_map.end()) {
            vertex_map[u] = curr_n;
            vertex_reverse_map[curr_n] = u;
            u = curr_n++;
        }
        else {
            u = vertex_map[u];
        }

        if(vertex_map.find(v) == vertex_map.end()){
            vertex_map[v] = curr_n;
            vertex_reverse_map[curr_n] = v;
            v = curr_n++;
        }
        else {
            v = vertex_map[v];
        }

        nodes[u].outNodes.emplace_back(v);
        //然后取vertex_reverse_map[key值]可以得到实际图中的点
    }
    //cout << "curr_n=" << curr_n << ", vertex_num=" << vertex_num << endl;
    inFile.close();
    nodes[vertex_map[source]].residue = 1;
    
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
    double start = clock();

    // start iterating, stop when every node's residue below threshold
    // 每个点的residue的分配：有alpha部分转换为自己的reserve，有1-alpha部分平均传递给每个邻居
    while(1)
    {
        int nodeBelowThr = 0; // residue值低于阈值的点 个数

        for(int u = 0; u < vertex_num; u++)
        {
            int outDegree = nodes[u].outNodes.size();
            double teleportValue = (1 - alpha) * nodes[u].residue / outDegree;
            for(int i = 0; i < outDegree; i++)
            {
                int v = nodes[u].outNodes[i];
                //nodes[v].residue += teleportValue;
                nodes[v].tmp_residue += teleportValue;
            }
            nodes[u].reserve += alpha * nodes[u].residue;
            //nodes[u].residue = 0; //这里不用赋值为0也没事，第./+7行用tmp_residue进行值的替换 效果一样的
        }

        //cout << "第" << cnt << "轮结果：\n";

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

    printf("%s%d\n", "step = ", cnt);
    cout << "time: " << (clock()-start) / CLOCKS_PER_SEC<< "s\n";
    // 将运行时间写入文件
    // string resultPath = "./out/result.txt";
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
    cout << "每个点的迭代结果：\n";
    for(int i = 0; i < vertex_num; i++)
    {
        cout << vertex_reverse_map[i] << " residue: " << nodes[i].residue << " reserve: " << nodes[i].reserve << endl;
    }
    return 0;
}
