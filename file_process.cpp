#include <iostream>
#include <fstream>
using namespace std;

// 删除指定行
void DelLineData(string fileName, int lineNum)
{
	ifstream infile;
	infile.open(fileName);
	
	string strFileData = "";
	int line = 1;
	string line_str;
	while(getline(infile, line_str))
	{
        if (line != lineNum)
		{
			strFileData += line_str;
			strFileData += '\n';
		}
		line++;
	}
	infile.close();
 
	//写入文件
	ofstream outfile;
	outfile.open(fileName);
	outfile.flush();
	outfile << strFileData;
	outfile.close();
}

// 修改指定行
void ModifyLineData(string fileName, int lineNum, char* lineData)
{
	ifstream in;
	in.open(fileName);
 
	string strFileData = "";
	int line = 1;
	char tmpLineData[1024] = {0};
	while(in.getline(tmpLineData, sizeof(tmpLineData)))
	{
		if (line == lineNum)
		{
			strFileData += CharToStr(lineData);
			strFileData += "\n";
		}
		else
		{
			strFileData += CharToStr(tmpLineData);
			strFileData += "\n";
		}
		line++;
	}
	in.close();
 
	//写入文件
	ofstream out;
	out.open(fileName);
	out.flush();
	out<<strFileData;
	out.close();
}

// 读取第3行的【# Nodes: 281903 Edges: 2312497】中两个数字
string get_n_m_form_line3(string fileName)
{
    ifstream infile;
    infile.open(fileName);

    string line_str;
    int curr_line = 1;
	while (getline(infile, line_str))
	{
		if(curr_line == 3)
            break;
        else
            curr_line++;
	}
    //cout << line_str << endl;
	infile.close();

    string n_str, m_str;
    int lianxushuzi = 0;
    for(int i = 0; i < line_str.size(); i++)
    {
        if(line_str[i] >= '0' && line_str[i] <= '9')
        {
            if (lianxushuzi == 0)
            {
                lianxushuzi = 1;
                n_str += line_str[i];
            }
            else if (lianxushuzi == 1)
                n_str += line_str[i];
            else if (lianxushuzi == 2)
                m_str += line_str[i];
        }
        else
            if(lianxushuzi == 1)
                lianxushuzi = 2;
    }
    string n_m_str = n_str + ' ' + m_str + '\n';
    return n_m_str;
}



int main()
{
    string fileName = "/home/zyj/zhou/dataset/web-Stanford1.txt";

    string str_insert_to_line1 = get_n_m_form_line3(fileName); // 从第三行中获取n和m

    // 删除1~4行
    for(int i = 1; i <= 4; i++)
        DelLineData(fileName, i);
    
    // 把str_insert_to_line1插入第一行

    

    // 删除前4行数据，随后把n和m写入第一行


    return 0;
}

