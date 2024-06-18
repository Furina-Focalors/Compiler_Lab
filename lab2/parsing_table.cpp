/**
 * @author Furina
 * @date 2023.12.24
 * @brief 计算parsing table
 */
#include "headers.h"

using namespace std;

void getParsingTable(std::map<std::pair<Symbol, Symbol>, Production> &parsing_table, const std::set<Production> &productions, const std::map<std::vector<Symbol>, std::set<Symbol>> &firstRight, const std::map<Symbol, std::set<Symbol>>& allFollow){
    for(auto &production:productions){
        set<Symbol>first = (*firstRight.find(production.right)).second;// 获取产生式右侧的FIRST集
        for(auto &sym:first){
            if(sym.type==TERMINAL){// 对每个终结符a，将该产生式加入parsing_table(A,a)，A是产生式的左部
                parsing_table[{production.left, sym}]=production;
            }else if(sym.type==EPSILON){// 空串在FIRST中
                set<Symbol>follow = (*allFollow.find(production.left)).second;// FOLLOW(A)
                for(auto & ele:follow){
                    if(ele.type==TERMINAL){// 对于每个终结符b，将该产生式加入parsing_table(A,b)
                        parsing_table[{production.left, ele}]=production;// 由于$的类型也被设置为TERMINAL，此处已计算
                    }
                }
            }
        }

    }
}