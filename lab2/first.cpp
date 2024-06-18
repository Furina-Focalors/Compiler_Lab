/**
 * @author Furina
 * @date 2023.12.24
 * @brief 计算FIRST集
 */

#include "headers.h"

using namespace std;

void calcFirst(const Symbol &symbol, const set<Production> &productions, map<Symbol, set<Symbol>>& allFirst){
    if(symbol.type == TERMINAL){// 终结符的FIRST只包含它自身
        allFirst[symbol].insert(symbol);
        return;
    }else if(symbol.type == EPSILON){
        allFirst[symbol].insert(Symbol());
        return;
    }
    // 非终结符
    for(const Production& p:productions){
        if(p.left==symbol){
            if(p.right[0].type==EPSILON) { allFirst[symbol].insert(Symbol());continue; }// symbol能产生空串，将空串加入FIRST
            // X->Y1Y2Y3...
            bool add_epsilon = true;
            for(const Symbol& s:p.right){// 遍历产生式右部的每个符号
                calcFirst(s, productions, allFirst);// 递归计算FIRST
                set<Symbol> s_first = allFirst[s];
                bool has_epsilon = false;// s的FIRST中有空串吗？
                for(const Symbol& a: s_first){// 将s的FIRST集中所有非空串符号加入
                    if(a.type==EPSILON)has_epsilon = true;
                    else allFirst[symbol].insert(a);
                }
                if(!has_epsilon){// s的FIRST中不包含空串，不再加入任何串
                    add_epsilon = false;
                    break;
                }
            }
            if(add_epsilon){// 这个产生式的Y1Y2Y3...中每个符号的FIRST都包含空串，加入空串
                allFirst[symbol].insert(Symbol());
            }
        }
    }
}

void calcRightFirst(const std::vector<Symbol>& symbols, const std::map<Symbol, std::set<Symbol>>& firstLeft, std::map<vector<Symbol>, std::set<Symbol>>& firstRight){
    bool add_epsilon = true;
    for(auto& sym:symbols){// 遍历每个符号，按照法则2构造FIRST集
        set<Symbol> first = (*firstLeft.find(sym)).second;
        bool has_epsilon = false;
        for(auto& a:first){
            if(a.type==EPSILON)has_epsilon=true;
            else firstRight[symbols].insert(a);
        }
        if(!has_epsilon){
            add_epsilon = false;
            break;
        }
    }
    if(add_epsilon)firstRight[symbols].insert(Symbol());
}