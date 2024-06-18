/**
 * @author Furina
 * @date 2023.12.24
 * @brief 主函数
 */

/*
 * 需要计算：
 * 1）所有终结符和非终结符的FIRST集
 * 2）所有右侧产生式的FIRST集
 * 3）对于所有形如A->aBb的产生式，b的FIRST集（用于计算FOLLOW）
 * 4）所有非终结符的FOLLOW集
 * */

#include <iostream>
#include "headers.h"

using namespace std;

int main(){
    /* 识别任意ab个数相等的字符串
     * S->aBS
     * S->bAS
     * S->ε
     * A->a
     * A->bAA
     * B->b
     * B->aBB
     * FIRST(S)={a,b,ε}
     * FIRST(A)={a,b}
     * FIRST(B)={a,b}
     * */
    map<Symbol, set<Symbol>> firstLeft;// 所有终结符和非终结符的FIRST
    map<vector<Symbol>, set<Symbol>> firstRight;// 所有右侧产生式的FIRST
    map<Symbol, set<Symbol>> follow;// 所有非终结符的FOLLOW
    map<pair<Symbol, Symbol>, Production> parsing_table;// 预测分析表

    Symbol start;// 文法开始符号
    set<Production> productions;// 产生式
    set<Symbol> terminals;// 所有的终结符
    set<Symbol> non_terminals;// 所有的非终结符

    cout<<"Enter Start Symbol of your grammar:"<<endl;
    string startSymName;
    cin>>startSymName;
    start = Symbol(NON_TERMINAL,startSymName);
    follow[start].insert(Symbol(TERMINAL, "$"));// 将$加入文法开始符号的FOLLOW集
    cout<<"Enter Terminals:(Furina to exit):"<<endl;
    terminals.insert(Symbol());// 空串
    string termName;
    while(cin>>termName){
        if(termName=="Furina") { break; }
        terminals.insert(Symbol(TERMINAL, termName));
    }
    cout<<"Enter Non-terminals:(Furina to exit):"<<endl;
    string nonTermName;
    while(cin>>nonTermName){
        if(nonTermName=="Furina") { break; }
        non_terminals.insert(Symbol(NON_TERMINAL, nonTermName));
    }
    cout<<"Enter Productions:(Sample:S->aBA, Use '@' to represent epsilon, Furina to exit):"<<endl;
    string productionStr;
    while(cin>>productionStr){
        if(productionStr=="Furina"){ break; }
        // 截取产生式左部
        int end = 0, size = productionStr.size();
        for(;end<size;++end){
            if(productionStr[end]=='-')break;
        }
        //cout<<"END="<<end<<endl;
        if(end==size||end+2>=size){
            cout<<"Error production. Please try again."<<endl;
            continue;
        }
        string leftStr = productionStr.substr(0,end);// 产生式左部
        productionStr = productionStr.substr(end+2);// 去除产生式左部，用于匹配右部
        //cout<<"Leftstr= "<<leftStr<<", production="<<productionStr<<endl;
        Symbol left;
        for(auto& sym: non_terminals){
            if(leftStr==sym.symbol){
                left = sym;
                break;
            }
        }
        if(left.type!=NON_TERMINAL){
            cout<<"Error production. Please try again."<<endl;
            continue;
        }
        //cout<<"Left is "<<left.type<<", "<<left.symbol<<endl;
        // 匹配产生式右部
        if(productionStr=="@"){// 这是一个空串产生式
            productions.insert(Production(left, {Symbol()}));
            continue;
        }
        vector<Symbol> right;
        for(int i=0;i<productionStr.size();++i){
            string rightStr = productionStr.substr(0,i+1);// 截取前i+1个
            //cout<<"RightStr is "<<rightStr<<endl;
            bool is_non_terminal = false;
            for(auto &sym:non_terminals){
                if(rightStr==sym.symbol){
                    right.emplace_back(sym);
                    productionStr = productionStr.substr(i+1);// 截去已经匹配的部分
                    i=-1;
                    is_non_terminal = true;
                    break;
                }
            }
            if(!is_non_terminal){// 继续查找终结符
                for(auto &sym:terminals){
                    if(rightStr==sym.symbol){
                        right.emplace_back(sym);
                        productionStr = productionStr.substr(i+1);// 截去已经匹配的部分
                        i=-1;
                        break;
                    }
                }
            }
        }
        if(right.empty()){
            cout<<"Error production. Please try again."<<endl;
            continue;
        }
        //cout<<"Right is ";for(auto&s:right)cout<<s.symbol;
        //cout<<endl;
        // 构造产生式
        productions.insert(Production(left, right));
    }


//    // 构造符号和产生式
//    Symbol S = Symbol(NON_TERMINAL,"S");
//    start = S;
//    string endMark = "$";
//    follow[start].insert(Symbol(TERMINAL, endMark));
//    Symbol A = Symbol(NON_TERMINAL,"A");
//    Symbol B = Symbol(NON_TERMINAL,"B");
//    Symbol a = Symbol(TERMINAL, "a");
//    Symbol b = Symbol(TERMINAL, "b");
//    Symbol epsilon = Symbol();
//    non_terminals.insert(S);
//    non_terminals.insert(A);
//    non_terminals.insert(B);
//    terminals.insert(a);
//    terminals.insert(b);
//    terminals.insert(epsilon);
//
//    productions.insert(Production(S, {a, B, S}));
//    productions.insert(Production(S,{b, A, S}));
//    productions.insert(Production(S, {epsilon}));
//    productions.insert(Production(A, {a}));
//    productions.insert(Production(A, {b, A, A}));
//    productions.insert(Production(B, {b}));
//    productions.insert(Production(B, {a, B, B}));

//    cout<<"Terminals: ";
//    for(auto& term:terminals){
//        cout<<term.symbol<<' ';
//    }cout<<endl;
//    cout<<"Non terminals:";
//    for(auto& nonterm: non_terminals){
//        cout<<nonterm.symbol<<' ';
//    }cout<<endl;
//    cout<<"Productions: "<<endl;
//    for(auto& production:productions){
//        cout<<production.left.symbol<<"->";
//        for(auto& sym: production.right)cout<<sym.symbol;
//        cout<<endl;
//    }

    // 计算所有终结符和非终结符的FIRST集
    for(auto& sym: non_terminals){
        calcFirst(sym, productions, firstLeft);
    }
    for(auto& sym: terminals){
        calcFirst(sym, productions, firstLeft);
    }

    // 计算所有右侧产生式的FIRST（用于计算parsing table）
    for(auto &production:productions){
        calcRightFirst(production.right, firstLeft, firstRight);
    }
//    for(auto &set:firstRight){
//        cout<<"First of ";
//        for(auto &sym:set.first){
//            cout<<sym.symbol;
//        }cout<<": ";
//        for(auto &sym:set.second){
//            if(sym.type==EPSILON)cout << "eps" << ' ';
//            else cout << sym.symbol << ' ';
//        }
//        cout<<endl;
//    }

    // 打印所有终结符和非终结符的FIRST集
    cout<<"==================FIRST set=================="<<endl;
    for(const pair<const Symbol, set<Symbol>>& firstset: firstLeft){
        if(firstset.first.type==EPSILON) continue;
        cout << "First of " << firstset.first.symbol<<" : ";
        for(auto &sym: firstset.second){
            if(sym.type==EPSILON)cout << "eps" << ' ';
            else cout << sym.symbol << ' ';
        }
        cout<<endl;
    }

    // 计算FOLLOW集
    calcFollow(productions, firstLeft, follow);
    // 打印所有非终结符的FOLLOW集
    cout<<"==================FOLLOW set=================="<<endl;
    for(const pair<const Symbol, set<Symbol>>& set: follow){
        cout << "Follow of " << set.first.symbol<<" : ";
        for(auto &sym: set.second){
            if(sym.type==EPSILON)cout << "eps" << ' ';
            else cout << sym.symbol << ' ';
        }
        cout<<endl;
    }

    // 计算parsing table
    getParsingTable(parsing_table, productions, firstRight, follow);
    // 打印parsing table
    for(auto &ele:parsing_table){
        cout<<"("<<ele.first.first.symbol<<", "<<ele.first.second.symbol<<"): "
            <<ele.second.left.symbol<<"->";
        for(auto &sym:ele.second.right){
            if(sym.type==EPSILON)cout<<"eps";
            else cout<<sym.symbol;
        }
        cout<<endl;
    }

    string w;
    cout<<"Enter a string to start analysis:(Furina to exit)"<<endl;
    while(cin>>w){
        if(w=="Furina")break;
        vector<Symbol>input;
        splitStr(w, input, terminals);
        analyze(input, parsing_table, start);
        cout<<"Enter a string to start analysis:(Furina to exit)"<<endl;
    }

    return 0;
}
