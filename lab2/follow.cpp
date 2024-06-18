/**
 * @author Furina
 * @date 2023.12.24
 * @brief 计算FOLLOW集
 */
#include "headers.h"
#include <iostream>

using namespace std;

// 对于形如A->aBb的产生式，要计算b的FIRST作为B的FOLLOW
// 获取b，返回B
Symbol getTail(const Production& production, vector<Symbol> &symbols){
    int i=0, size=production.right.size();
    for(;i<size;++i){
        if(production.right[i].type==NON_TERMINAL)break;
    }
    if(i==size)return {};
    Symbol sym = production.right[i];
    ++i;
    for(;i<size;++i){
        symbols.emplace_back(production.right[i]);
    }
    return sym;
}

// 遍历一次产生式，尝试更新FOLLOW
void updateFollow(const std::set<Production> &productions, const std::map<Symbol, std::set<Symbol>>& allFirst, std::map<Symbol, std::set<Symbol>>& allFollow){
    std::map<std::vector<Symbol>, std::set<Symbol>> partials;// 所有的b表达式的FIRST集
    for(auto &production:productions){// 遍历所有产生式
        vector<Symbol> symbols;
        Symbol target = getTail(production, symbols);
        if(target.type==EPSILON)continue;// 如果没有找到符合条件的产生式，直接跳过
        std::set<Symbol> first = partials[symbols];
        if(!symbols.empty()){// 处理形如A->aBb的产生式，将FIRST(b)\{ε}加入FOLLOW(B)
            calcRightFirst(symbols, allFirst, partials);
            first = partials[symbols];
            allFollow[target].insert(first.begin(), first.end());
            allFollow[target].erase(Symbol());// 去除ε
        }
        if(
                symbols.empty() ||                      // 形如A->aB的产生式
                first.find(Symbol())!=first.end()    // 形如A->aBb的产生式，且b的FIRST集包含ε
                ){
            set<Symbol> follow = allFollow[production.left];// 将A的FOLLOW集加入B
            allFollow[target].insert(follow.begin(), follow.end());
        }
    }
}

void calcFollow(const std::set<Production> &productions, const std::map<Symbol, std::set<Symbol>>& allFirst, std::map<Symbol, std::set<Symbol>>& allFollow){
    map<Symbol, set<Symbol>> temp;
    do{
        temp=allFollow;
        updateFollow(productions, allFirst, allFollow);
    }while(temp!=allFollow);
}