#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

struct Node {
    vector<int> outNodes;
    vector<int> inNodes;
};


int main()
{
    double d = 0.85;
    double threshold = 1e-6;

    string edge_path = "/home/zyj/zhou/dataset/a.txt";
    ifstream inFile(edge_path);
    if(!inFile)
    {
        cout << "open file: " << edge_path << " failed." << endl;
        return 0;
    }
    int n, m;
    inFile >> n >> m;
    vector<Node> nodes(n);

    int u, v;
    while (inFile >> u >> v)
    {
        nodes[u].outNodes.push_back(v);
        nodes[v].inNodes.push_back(u);
    }

    vector<double> PPR(n, 0);
    
    // 假设点0是user，求别的每个点相对于user的PPR值
    // user的初始PPR值为1
    int user = 0;
    PPR[user] = 1;

    vector<double> oldPPR(n, 0);
    oldPPR[user] = 1;
    int N = 999;
    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        oldPPR[i] = rand() % (N + 1) / (float)(N + 1);
        //cout << i << "的oldppr初始值：" << oldPPR[i] << endl;
    }

    int r_i;
    int iter_num = 0;
    int should_stop = 0;
    while (iter_num < 300)
    {
        iter_num++;

        for (int i = 0; i < n; i++)
        {
            double sum_tmp=0;
            r_i = (i == user) ? 1 : 0;
            for (int j = 0; j < nodes[i].inNodes.size(); j++)
            {
                int curr_node = nodes[i].inNodes[j];
                sum_tmp += oldPPR[curr_node] / (1.0 * nodes[curr_node].outNodes.size());
            }
            PPR[i] = (1 - d) * r_i + d * sum_tmp;
        }

        for (int i = 0; i < n; i++){
            oldPPR[i] = PPR[i];
            // PPR[i] = 0;
        }

        //每个顶点被访问到的概率趋于稳定时停止游走
        // for (int i = 0; i < n; i++) {
        //     if (abs(oldPPR[i] - PPR[i]) < threshold)
        //         should_stop++;
        // }
        // if(should_stop == n)
        //     break;
        // else {
        //     should_stop = 0;
        //     oldPPR = PPR;
        // }
    }

    cout << "总共迭代次数：" << iter_num << endl;
    for (int i = 0; i < n; i++)
    {
        cout << PPR[i] << endl;
    }

    return 0;
}