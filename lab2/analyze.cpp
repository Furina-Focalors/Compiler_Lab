/**
 * @author Furina
 * @date 2023.12.24
 * @brief 分析输入串
 */
#include "headers.h"
#include <stack>
#include <iostream>
#include <iomanip>

using namespace std;

// 将输入序列转换为终结符的数组用于分析
void splitStr(string str, vector<Symbol> &inputStream, const set<Symbol> &terminals){
    for(int i=0;i<str.length();++i){
        string terminal = str.substr(0, i+1);
        for(auto& t: terminals){// 匹配已有的终结符
            if(terminal==t.symbol){
                inputStream.emplace_back(t);
                str = str.substr(i+1);
                i=-1;
                break;
            }
        }
    }
    inputStream.emplace_back(TERMINAL,"$");
}

string printStack(stack<Symbol>s){
    string res;
    while(!s.empty()) {
        res.append(s.top().symbol).append(" ");
        s.pop();
    }
    return res;
}

string printInput(int pos, const vector<Symbol> &input){
    string res;
    int size = input.size();
    for(int i=pos;i<size;++i){
        res.append(input[i].symbol);
    }
    return res;
}

string printProduction(const Production &p){
    string res;
    res.append(p.left.symbol).append("->");
    for(auto& sym:p.right) {
        if(sym.type==EPSILON)res.append("eps");
        else res.append(sym.symbol);
    }
    return res;
}

void analyze(const std::vector<Symbol> &inputStream, const std::map<std::pair<Symbol, Symbol>, Production> &parsing_table, const Symbol &start){
    stack<Symbol> stk;
    Symbol acc = Symbol(TERMINAL,"$");
    stk.emplace(acc);
    stk.emplace(start);
    int cur = 0;// 指向input的指针
    Symbol top=stk.top();// 栈顶元素
    cout<<"====================Start analysis===================="<<endl;
    cout<<setw(30)<<right<<"Stack"<<setw(30)<<right<<"Input"<<setw(30)<<right<<"Production"<<endl;
    cout<<setw(30)<<right<<printStack(stk)
        <<setw(30)<<right<<printInput(cur, inputStream)
        <<setw(30)<<right<<"Start"<<endl;
    while(!(top==acc)){
        if(top==inputStream[cur]){
            cout<<setw(30)<<right<<printStack(stk)
                <<setw(30)<<right<<printInput(cur, inputStream)
                <<setw(30)<<right<<"Match "+inputStream[cur++].symbol<<endl;
            stk.pop();
        }else if(top.type==TERMINAL || parsing_table.find({top,inputStream[cur]})==parsing_table.end()){
            cout<<"Error!"<<endl;
            return;
        }else{
            Production p = (*parsing_table.find({top,inputStream[cur]})).second;
            cout<<setw(30)<<right<<printStack(stk)
                <<setw(30)<<right<<printInput(cur, inputStream)
                <<setw(30)<<right<<printProduction(p)<<endl;
            stk.pop();
            // 倒序入栈
            int size = p.right.size();
            for(int i=size-1;i>=0;--i){
                if(p.right[i].type!=EPSILON)stk.emplace(p.right[i]);
            }
        }
        top=stk.top();
    }
    cout<<setw(30)<<right<<printStack(stk)
        <<setw(30)<<right<<printInput(cur, inputStream)
        <<setw(30)<<right<<"Accept"<<endl;
}
