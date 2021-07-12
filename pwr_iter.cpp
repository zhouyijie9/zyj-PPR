#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

struct Node {
    vector<int> outNodes;
};

vector<double> operator*(const double a, const vector<double>& v)
{
    vector<double> r;

    r.reserve(v.size());
    for (auto i = 0; i < v.size(); ++i)
    {
        r.push_back(a * v[i]);
    }
    return r;
}

vector<double> operator+(const vector<double>& v1, const vector<double>& v2)
{
    vector<double> r;

    r.reserve(v1.size());
    for (auto i = 0; i < v1.size(); ++i)
    {
        r.push_back(v1[i] + v2[i]);
    }
    return r;
}

vector<double> operator*(const vector<double>& v1, const vector<vector<double> >& v2)
{
    vector<double> r;
    double tmp;

    r.reserve(v1.size());

    for (int i = 0; i < v1.size(); ++i)
    {
        tmp = 0;

        for (int j = 0; j < v1.size(); ++j)
            tmp += v1[j] * v2[j][i];
        
        r.push_back(tmp);
    }
    return r;
}

int main()
{
    double alpha = 0.15;
    double threshold = 1e-6;

    string edge_path = "/home/zyj/zhou/dataset/a.txt";
    ifstream inFile(edge_path);
    if(!inFile)
    {
        cout << "open file " << edge_path << " failed." << endl;
        return 0;
    }
    int n, m;
    inFile >> n >> m;

    vector<double> gamma(n, 0); //gamma是一个1×n的向量，每个位置表示随机游走到对应点的alive概率
    // gamma[0] = 1.0; //在第一次迭代时，只有0位置的点是alive的

    //------------------------------------------------
    //每个点的alive概率随机赋值0~1
    // srand(time(NULL));
    // int N = 999; //精度为小数点后面3位
    // for(int i = 0; i < n; i++)
    // {
    //     gamma[i] = rand() % (N + 1) / (float)(N + 1);
    //     cout << "点" << i << "的初始alive概率=" << gamma[i] << endl;
    // }    
    //----------------------------------------------------

    vector<double> ppr_value(n, 0); //每次迭代的alive概率乘上alpha累加到此向量中
    vector<vector<double> > I(n, vector<double>(n, 0)); //I是指示矩阵，为1表示此处有边
    vector<vector<double> > P(n, vector<double>(n, 0)); //P是转移矩阵，每个值代表跳转的概率

    // 向P中填入数据
    vector<Node> nodes(n); //nodes用来存放点向量
    int u, v;
    while (inFile >> u >> v)
    {
        nodes[u].outNodes.push_back(v);
        I[u][v] = 1;
    }
    inFile.close();

    for(int i = 0; i < n; ++i)
    {
        int out_degree = nodes[i].outNodes.size();

        for(int j = 0; j < n; ++j)
            P[i][j] = I[i][j] / out_degree;
    }

    int iter_num = 0;
    int should_stop = 0;
    while(iter_num < 100)
    {
        iter_num++;
        
        ppr_value = ppr_value + alpha * gamma; //重载了+和×运算

        gamma = (1 - alpha) * gamma * P; //重载了两个×运算

    }

    //将结果写入文件
    string resultPath = "./out/pwr_itr_result_source_residue_1.txt";
    // string resultPath = "./out/pwr_itr_result_random_residue.txt";
    // ofstream fout(resultPath);

    for (int i = 0; i < n; i++)
    {
        cout << ppr_value[i] << endl;
    }
    
    // fout.close();
    
}