#include <iostream>
#include <algorithm>
#include <queue>
#include <vector>
#include <map>
#include <stack>
#include <time.h>
using namespace std;
const int maxn = 1e5;
struct node
{
    string content;
    int begincnt;
    vector<string> endcontent;
    int nex;
    int to;
    node(string a, int b = 0, vector<string> d = vector<string>(0), int c = -1, int f = -1) : content(a), begincnt(b), endcontent(d), nex(c), to(f) {}
    bool contain(string a)
    {
        return content.find(a, 0) != content.npos;
    }
    vector<string> getparam()
    {
        vector<string> ans;
        if (this->contain("send"))
        {
            int temid = content.find(",", 0);
            if (content.find("(", temid + 1) != content.npos) //����������ж������
            {
                int temid2 = content.find("(", temid + 1); //"("���±�
                string param = content.substr(temid2 + 1, content.find(")", temid2 + 1) - temid2 - 1);
                while (param.find(",", 0) != param.npos)
                {
                    int temid3 = param.find(",", 0);
                    ans.push_back(param.substr(0, temid3));
                    param = param.substr(temid3 + 1, param.length() - temid3 - 1);
                }
                ans.push_back(param);
            }
            else
            {
                int temid2 = content.find(")", temid + 1);
                ans.push_back(content.substr(temid + 1, temid2 - temid - 1));
            }
        }
        else if (this->contain("receive"))
        {
            int temid = content.find("<-", 0);
            string param = content.substr(0, temid);
            if (param.find(",", 0) != param.npos) //�ж�����ŷָ��Ĳ���
            {
                while (param.find(",", 0) != param.npos)
                {
                    int temid3 = param.find(",", 0);
                    ans.push_back(param.substr(0, temid3));
                    param = param.substr(temid3 + 1, param.length() - temid3 - 1);
                }
                ans.push_back(param);
            }
            else
                ans.push_back(param);
        }
        return ans;
    }
};
vector<node> P;           //�洢���в����ߵ�����
vector<int> start_index;  //ÿ�����������ϵ���ʼ�±�
vector<int> par_position; //ÿ�����������ϵ�ǰ��ȡ�����±�
vector<bool> ismulti;     //��ʶÿ���������Ƿ��ж��
vector<string> par_name;  //��������
int stid = -1;
bool iserror = false; //��ʶ���������Ƿ��д���
string lasstore = ""; //�洢��һ��receive���ݵı���
stack<string> Loopstmt;
map<string, int> mp;     //���������͵������ϱ��ӳ��
int participate_num = 0; //����������
vector<string> result;
stack<int> emptyid;
map<int, int> dep;
int mxde = 0;
bool is_in_Loop = 0;
vector<string> lasrecv;
stack<int> lasbegincnt;
int firstSenderId;
bool transform; //�Ƿ���Ҫת���ɸ�ֵ

bool cmp(string A, string B)
{
    int id1 = A.find(":", 0);
    int id2 = A.find(")", 0);
    string value1 = A.substr(id1 + 1, id2 - id1 - 1);
    int id3 = B.find(":", 0);
    int id4 = B.find(")", 0);
    string value2 = B.substr(id3 + 1, id4 - id3 - 1);
    int idA = 0, idB = 0;
    int cnt = 0;
    for (auto it : par_name)
    {
        if (value1 == it)
            idA = cnt;
        if (value2 == it)
            idB = cnt;
        cnt++;
    }
    return idA > idB;
}
void link(int parid)
{
    int sid = par_position[parid];
    if (sid >= start_index[parid + 1])
        return;
    //�ҵ���ǰ�������е�send���
    while (sid < start_index[parid + 1] && P[sid].contain("send") == 0)
        sid++;
    //cout<<"&&&&&&&&&&"<<start_index[parid+1]<<endl;
    //cout<<"####"<<sid<<endl;
    if (sid >= start_index[parid + 1])
        return;

    //�ҵ���send���ķ���Ŀ��
    int destination = P[sid].to;
    //cout<<"destination: "<<destination<<endl;
    par_position[parid] = sid + 1;
    int rid = par_position[destination];
    //�ҵ�send����Ӧ��receive���
    while (rid < start_index[destination + 1] && P[rid].contain("receive") == 0)
        rid++;
    //cout<<"####"<<rid<<endl;
    //cout<<"&&&&&&&&&&"<<start_index[destination+1]<<endl;
    if (rid >= start_index[destination + 1])
    {
        puts("Send without Receive!");
        iserror = 1;
        return;
    }
    //����
    P[sid].nex = rid;

    //�ϴ�recive����ѭ��ͷ��send
    if (P[rid].begincnt && ismulti[destination])
    {
        P[sid].begincnt += P[rid].begincnt;
        P[rid].begincnt = 0;
    }

    if (ismulti[parid])
    {
        //�´�send����ѭ��β
        int temrid = rid;
        while (temrid < start_index[destination + 1] && P[temrid].contain("send") == 0 && P[temrid].endcontent.size() == 0 && P[temrid].begincnt == 0)
            temrid++;
        if (temrid >= start_index[destination + 1] || P[temrid].contain("send") || P[temrid].begincnt && temrid > rid)
            temrid--;
        //cout<<"�´���"<<temrid<<endl;
        P[temrid].endcontent.insert(P[temrid].endcontent.begin(), P[sid].endcontent.begin(), P[sid].endcontent.end());
        P[sid].endcontent.clear();
    }

    par_position[destination] = rid + 1;
    link(destination);
}
void outputBeforeTransform(int stid)
{
    while (stid != -1)
    {
        //cout << stid << endl;
        if (P[stid].begincnt)
        {
            int num = P[stid].begincnt;
            lasbegincnt.push(num);
            while (num--)
            {
                emptyid.push((int)result.size());
                result.push_back("");
            }
        }
        result.push_back(P[stid].content);

        if (P[stid].endcontent.size() > 0)
        {
            sort(P[stid].endcontent.begin(), P[stid].endcontent.begin() + lasbegincnt.top(), cmp);
            lasbegincnt.pop();
            for (auto it : P[stid].endcontent)
            {
                if (emptyid.empty())
                {
                    cout << "Wrong Answer!place1!\n";
                    exit(0);
                }
                result[emptyid.top()] = it;
                int desz = emptyid.size();
                dep[emptyid.top()] = desz;
                mxde = max(mxde, (int)emptyid.size());
                emptyid.pop();
                dep[result.size()] = desz;
                result.push_back("Loop_End");
            }
        }
        stid = P[stid].nex;
    }
}
void transformToAssignment(int stid)
{
    while (stid != -1)
    {
        //cout << stid << endl;
        if (P[stid].contain("send"))
        {
            int nx = P[stid].nex;
            if (nx == -1 || P[nx].contain("receive") == 0)
            {
                cout << "Wrong Answer!place2\n";
                exit(0);
            }
            vector<string> sendpar = P[stid].getparam();
            vector<string> recvpar = P[nx].getparam();
            int sz1 = sendpar.size();
            if (sz1 != recvpar.size())
            {
                cout << "Wrong Answer!place3\n";
                exit(0);
            }
            if (P[stid].begincnt)
            {
                int num = P[stid].begincnt;
                lasbegincnt.push(num);
                while (num--)
                {
                    emptyid.push((int)result.size());
                    result.push_back("");
                }
            }
            if (P[nx].begincnt)
            {
                int num = P[nx].begincnt;
                lasbegincnt.push(num);
                while (num--)
                {
                    emptyid.push((int)result.size());
                    result.push_back("");
                }
            }
            for (int i = 0; i < sz1; ++i)
            {
                //cout<<"send�Ĳ�����"<<sendpar[i]<<" recv�Ĳ��� "<<recvpar[i]<<endl;
                string ass = recvpar[i] + " = " + sendpar[i];
                result.push_back(ass);
            }
            if (P[nx].endcontent.size() > 0)
            {
                sort(P[nx].endcontent.begin(), P[nx].endcontent.begin() + lasbegincnt.top(), cmp);
                lasbegincnt.pop();
                for (auto it : P[nx].endcontent)
                {
                    if (emptyid.empty())
                    {
                        cout << "Wrong Answer!place1!\n";
                        exit(0);
                    }
                    result[emptyid.top()] = it;
                    int desz = emptyid.size();
                    dep[emptyid.top()] = desz;
                    mxde = max(mxde, (int)emptyid.size());
                    emptyid.pop();
                    dep[result.size()] = desz;
                    result.push_back("Loop_End");
                }
            }
            if (P[stid].endcontent.size() > 0)
            {
                sort(P[stid].endcontent.begin(), P[stid].endcontent.begin() + lasbegincnt.top(), cmp);
                lasbegincnt.pop();
                for (auto it : P[stid].endcontent)
                {
                    if (emptyid.empty())
                    {
                        cout << "Wrong Answer!place1!\n";
                        exit(0);
                    }
                    result[emptyid.top()] = it;
                    int desz = emptyid.size();
                    dep[emptyid.top()] = desz;
                    mxde = max(mxde, (int)emptyid.size());
                    emptyid.pop();
                    dep[result.size()] = desz;
                    result.push_back("Loop_End");
                }
            }
            stid = nx;
        }
        else if (P[stid].contain("receive"))
            ;
        else
        {
            if (P[stid].begincnt)
            {
                int num = P[stid].begincnt;
                lasbegincnt.push(num);
                while (num--)
                {
                    emptyid.push((int)result.size());
                    result.push_back("");
                }
            }
            result.push_back(P[stid].content);
            if (P[stid].endcontent.size() > 0)
            {
                sort(P[stid].endcontent.begin(), P[stid].endcontent.begin() + lasbegincnt.top(), cmp);
                lasbegincnt.pop();
                for (auto it : P[stid].endcontent)
                {
                    if (emptyid.empty())
                    {
                        cout << "Wrong Answer!place1!\n";
                        exit(0);
                    }
                    result[emptyid.top()] = it;
                    int desz = emptyid.size();
                    dep[emptyid.top()] = desz;
                    mxde = max(mxde, (int)emptyid.size());
                    emptyid.pop();
                    dep[result.size()] = desz;
                    result.push_back("Loop_End");
                }
            }
        }

        stid = P[stid].nex;
    }
}
int findTheFirstSender()
{
    int id = -1;
    bool flag = 0;
    for (int i = 0; i < start_index.size() - 1; ++i)
    {
        for (int j = start_index[i]; j < start_index[i + 1]; ++j)
        {
            if (P[j].contain("send"))
            {
                if (id == -1)
                {
                    id = i;
                    break;
                }
                else
                {
                    flag = 1;
                    puts("Error:Repeated sending and receiving!Try to modify as follows:");
                }
            }
            if (P[j].contain("receive"))
            {
                if (flag) //����SSRR���
                {
                    //SSRR��������
                    int endid = start_index[i + 1] - 1;
                    int temj = j - start_index[i];
                    while (temj--)
                    {
                        node tem = P[start_index[i]];
                        for (int k = start_index[i]; k < endid; ++k)
                            P[k] = P[k + 1];
                        P[endid] = tem;
                    }
                    for (int T = start_index[i]; T < start_index[i + 1] - 1; ++T)
                    {
                        P[T].nex = T + 1;
                    }
                    //SSRR�������
                }
                break;
            }
        }
    }
    if (id == -1)
    {
        iserror = true;
        puts("Can not find the first sender!"); //û�е�һ�����͵Ĳ�����
    }
    return id;
}
void output()
{
    int cnt = 0;
    int lit = -1;
    string atomic;
    for (auto it : result)
    {
        if (it.find("Loop_Begin", 0) != it.npos && dep[cnt] == mxde)
        {
            int id1 = it.find("(", 0);
            int id2 = it.find(")", id1);
            atomic = "||" + it.substr(id1, id2 - id1 + 1);
            for (int i = 0;; i++)
            {
                if (result[i].find("Loop_Begin", 0) != result[i].npos && result[i] != "Loop_Begin(r:R)")
                {
                    lit = dep[i];
                    break;
                }
            }
            break;
        }
        cnt++;
    }
    cnt = 0;
    bool F=0;
    if (result[0].find("Loop_Begin", 0) == result[0].npos)
    {
        lit=-2;
        cout<<atomic<<"{"<<endl;
        F=1;
    }
    for (auto it : result)
    {
        if (it.find("Loop_Begin", 0) != it.npos)
        {
            if (dep[cnt] == lit)
                cout << atomic;
            if (dep[cnt] == mxde)
                cout << "Atomic_Begin" << endl;
            else
                cout << it << endl;
        }
        else if (it.find("Loop_End", 0) != it.npos && dep[cnt] == mxde)
        {
            cout << "Atomic_End" << endl;
            if (cnt + 1 < result.size() && result[cnt + 1].find("Loop_Begin", 0) != it.npos && dep[cnt + 1] == mxde)
                puts(";");
        }
        else
            cout << it << endl;
        cnt++;
    }
    if(F)puts("}");
}
void solve()
{
    firstSenderId = findTheFirstSender();
    if (firstSenderId == -1)
        return;
    link(firstSenderId);
    if (iserror)
        return;
    puts("Output Before Transform:");
    outputBeforeTransform(start_index[firstSenderId]);
}
int main()
{
    bool badcase1 = 0;
    //��ͼ
    string mc, nc;
    //�������������
    cin >> participate_num;
    //�����������
    par_name.resize(participate_num);
    ismulti.resize(participate_num);
    for (auto &it : par_name)
        cin >> it;
    //��������
    int lasto = -1;
    int loopheadnum = 0; //ѭ��ͷ��
    while (cin >> mc)
    {
        if (mc == "||" || start_index.size() == 0)
        {
            start_index.push_back(P.size());
            par_position.push_back(P.size());
            loopheadnum = 0;
        }
        if (mc == "||")
            continue;
        if (mc.find("Loop_Begin", 0) != mc.npos)
        {
            is_in_Loop = 1;
            int id1 = mc.find("(", 0);
            int id2 = mc.find(":", 0);
            int id3 = mc.find(")", 0);
            string key = mc.substr(id1 + 1, id2 - id1 - 1);
            string value = mc.substr(id2 + 1, id3 - id2 - 1);
            // cout<<key<<" "<<value<<endl;
            for (int i = 0; i < participate_num; i++)
            {
                if (value == par_name[i])
                {
                    mp[key] = i + 1;
                    ismulti[i] = 1;
                    break;
                }
            }
            loopheadnum++;
            Loopstmt.push(mc);
            continue;
        }
        else if (mc == "Loop_End" && P.size())
        {
            is_in_Loop = 0;
            lasrecv.clear();
            P.back().endcontent.push_back(Loopstmt.top());
            string loophead = Loopstmt.top();
            int id1 = loophead.find("(", 0);
            int id2 = loophead.find(":", 0);
            string key = loophead.substr(id1 + 1, id2 - id1 - 1);
            Loopstmt.pop();
            mp[key] = 0;
            continue;
        }
        node newm = node(mc);
        newm.begincnt = loopheadnum;
        loopheadnum = 0;
        if (newm.contain("send"))
        {
            int id1 = newm.content.find("(", 0);
            int id2 = newm.content.find(",", 0);
            string temp = newm.content.substr(id1 + 1, id2 - id1 - 1);
            if (is_in_Loop && lasrecv.size())
            {
                badcase1 = 1;
                for (auto I : lasrecv)
                {
                    if (I == temp)
                    {
                        badcase1 = 0;
                        break;
                    }
                }
                if (badcase1)
                    iserror = true;
            }
            if (mp[temp] == 0)
            {
                for (int i = 0; i < participate_num; i++)
                {
                    if (temp == par_name[i])
                    {
                        newm.to = i;
                        break;
                    }
                }
            }
            else
                newm.to = mp[temp] - 1;
        }
        else if (newm.contain("receive"))
        {
            vector<string> tempar = newm.getparam();
            if (is_in_Loop)
                lasrecv.push_back(tempar[0]);
            for (auto it : tempar)
            {
                if (it == "id")
                {
                    int id1 = newm.content.find("(", 0);
                    int id2 = newm.content.find(")", id1);
                    string recvfrom = newm.content.substr(id1 + 1, id2 - id1 - 1);
                    if (mp[recvfrom])
                        mp["id"] = mp[recvfrom];
                    for (int i = 0; i < participate_num; i++)
                    {
                        if (recvfrom == par_name[i])
                        {
                            mp["id"] = i + 1;
                            break;
                        }
                    }
                }
            }
        }
        if (P.size() && P.size() != start_index.back())
            P.back().nex = P.size();
        P.push_back(newm);
    }
    start_index.push_back(P.size());
    //��¼����ʱ��
    clock_t start, finish;
    start = clock();
    //������䲢ת���ɸ�ֵ
    solve();
    //�д���
    if (iserror)
    {
        if (badcase1)
            puts("Error:Communicate with different qs in Q in one iteration!");
    }
    else
    { //���ת��ǰ���
        output();
        result.clear();
        puts("");
        puts("Output After Transform:");
        transformToAssignment(start_index[firstSenderId]);
        //���ת������
        output();
    }
    finish = clock();
    double duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf("%lf seconds\n", duration);
}
////�ҵ���һ���������Ĳ�����
// Algorithm1 findTheFirstSender(Participant{Gp, Gqj, ..})
// {
//     for each G in Gp
//           for each instr in G's{instr1,instr2..instrn}
//              if isSend(instr)
//                  if isInLoop && instr.target!=lasRecv.from   //��ͬһѭ�����벻ͬ������
//                      Throw_Error2("Communicate with different qs in Q in one iteration!")
//                  end if
//              end if
//          end for
//      end for
//     for each G in Gp
// 		for each instr in G's{instr1,instr2..instrn}
// 			if isReceive(instr)
// 				break
// 			end if
// 			if isSend(instr)
// 				if firstSender ==null
// 					firstSender=G
// 					break
// 				else
// 					Throw_Error2("Error:Repeated sending and receiving!")
//                  SSRR_Reset
// 					break
// 				end if
// 			end if
// 		end for
// 	end for
// }
////������䲢�����
// Algorithm2 Link(G)//���������������
// {
// 	S=G.findNextSend()//�ҵ���һ��send���
// 	tgt=S.getTarget()//��ȡ��send���ķ���Ŀ��
// 	R=tgt.findNextReceive()//�ҵ���һ��receive���
// 	if msg(S)=msg(R)
// 		S.nex=R
// 		if S.AtomicBlock!=null
// 			S.AtomicBlock U R
// 		end if
// 		if R.AtomicBlock != null
// 			R.AtomicBlock U S
// 		end if
// 	end if
// 	Link(tgt)//�ݹ����
// }
////��send��receiveת������Ӧ�ĸ�ֵ���
// Algorithm3 transformToAssignment(instr) //�����������һ�����
// {
//     while (hasNext(instr))//hasNext(instr)��ʾ��ǰ����Ƿ��к����䣬���Ƿ������һ�����
//     {
//         if isSend (instr)
//             paramS=instr.getparam()//��ȡsend��䷢�͵�����
//     			paramR=instr.nex.getparam()//��ȡreceive����д洢send���ݱ���
//     			��send��receiveת���ɸ�ֵ���
//              for each ps in paramS , pr in paramR
//     				result U instr(ps=pr)
//     			end for
// 		    else
// 			result U instr
// 		end if
// 		instr=instr.nex()
//     }
// }