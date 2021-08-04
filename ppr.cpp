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

// struct Node {
//     vector<int> outNodes;
//     double residue = 0;
//     double tmp_residue = 0;
//     double reserve = 0;
// };

// int main(int argc, char const *argv[]) //会传入两个参数：argv[1]-edgepath和argv[2]-threshold
int main()
{
    // string edge_path = argv[1];
    // double threshold = atof(argv[2]);
    // string dest_path = argv[3];
    string edge_path = "/home/zyj/zhou/dataset/web-Google1.txt";
    double threshold = 1e-6;
    double real_compute_time;

    //vector<Node> nodes; //nodes用来存放点向量
    int source = 0; //我想用文件中的第一个点作为源点，整个实验最后得到的是另外的点关于它的PPR值
    // int sid = 0;
    long long int jisuancishu = 0;

    double start_read_file_time = clock();

    ifstream inFile(edge_path);
    if(!inFile)
    {
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
    double read_file_time = (clock() - start_read_file_time) / CLOCKS_PER_SEC; // 输出1：读文件时间

    start_pos[vertex_num] = edge_num;

    // vector<double> residues;
    // residues.resize(vertex_num + 1, 0);

    vector<double> tmp_residues;
    tmp_residues.resize(vertex_num + 1, 0);

    // vector<double> reserves;
    // reserves.resize(vertex_num + 1, 0);

    vector<double> residues_and_reserves;
    residues_and_reserves.resize(vertex_num * 2);

    residues_and_reserves[source] = 1.0;


    int cnt = 0; // 统计迭代次数

    double teleport_value;
    int out_degree;
    int node_below_thr = 0;
    int pos_0, pos_1;

    double start_computing_time = clock();
    // 开始迭代，根据公式，当【每个点】的【residue/出度<阈值】时停止迭代
    // 每个点的residue：有alpha部分转换为自己的reserve，有1-alpha部分平均传递给每个邻居
    while (1)
    {
        real_compute_time -= clock();
        for (int i = 0; i < vertex_num; i++)
        {
            pos_0 = start_pos[i];
            pos_1 = start_pos[i + 1];
            out_degree = pos_1 - pos_0;

            teleport_value = (1 - alpha) * residues_and_reserves[i*2] / out_degree;
            for (int j = 0; j < out_degree; j++)
            {
                int curr_node = edges[pos_0 + j];
                tmp_residues[curr_node] += teleport_value;
                jisuancishu++;
            }
            residues_and_reserves[i*2+1] += alpha * residues_and_reserves[i*2];
            // residues[i] = 0;
        }
        real_compute_time += clock();

        for(int i = 0; i < vertex_num; i++)
        {
            pos_0 = start_pos[i];
            pos_1 = start_pos[i + 1];
            out_degree = pos_1 - pos_0;

            residues_and_reserves[i*2] = tmp_residues[i];
            tmp_residues[i] = 0;

            if (residues_and_reserves[i*2] / out_degree < threshold)
                node_below_thr++;
        }

        if(node_below_thr == vertex_num)
            break;

        cnt++;
        node_below_thr = 0;
    }

    double computing_time = (clock() - start_computing_time) / CLOCKS_PER_SEC; // 输出2：计算时间

    printf("%s%d\n", "step = ", cnt);
    cout << "加载文件时间：" << read_file_time << "\n计算时间：" << computing_time << endl;
    cout << "计算次数：" << jisuancishu << endl;
    cout << "real time: " << real_compute_time/CLOCKS_PER_SEC << "s" << endl;

    // string outPath = "./out/ppr2.txt";
    // cout << "out path: " << outPath << endl;
    // ofstream fout(outPath);
    // for(int i = 0; i < vertex_num; i++)
    // {
    //     fout << i << " residue: " << residues[i] << " reserve: " << reserves[i] << endl;
    // }

    // fout.close();
    
    return 0;
}
