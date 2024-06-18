/**
 * @author Furina
 * @date 2023.12.24
 * @brief 包含各个工具函数的头文件
 */
#ifndef LAB2_HEADERS_H
#define LAB2_HEADERS_H

#include <string>
#include <vector>
#include <set>
#include <map>
// 符号的类型，用于区分终结符、非终结符与空串
enum SymbolType{
    TERMINAL,
    NON_TERMINAL,
    EPSILON,
};

// 符号
struct Symbol{
    SymbolType type;
    std::string symbol;
    explicit Symbol(SymbolType t, const std::string &s){type=t; symbol=s;}
    Symbol(const Symbol &s){type = s.type; symbol = s.symbol;}
    Symbol(){type = EPSILON;}

    bool operator<(const Symbol& s2) const{
        if(type==EPSILON) return s2.type!=EPSILON;
        return symbol<s2.symbol;
    }
    bool operator>(const Symbol& s2) const{
        if(type==EPSILON) return false;
        return symbol>s2.symbol;
    }
    bool operator==(const Symbol& s2) const{
        return type==s2.type&&symbol==s2.symbol;
    }
};

// 产生式
// 不使用'|'来分隔多个产生式
struct Production{
    Symbol left;
    std::vector<Symbol> right;
    explicit Production(const Symbol &l, const std::vector<Symbol> &r){left=l; right=r;}
    Production()= default;
    bool operator<(const Production& p2) const{
        if(left<p2.left)return true;
        else if(left>p2.left)return false;
        int len1 = right.size(), len2 = p2.right.size();
        int min = len1>len2?len2:len1;
        for(int i=0;i<min;++i){
            if(right[i]<p2.right[i])return true;
            else if(right[i]>p2.right[i])return false;
        }
        return len1<len2;
    }
};

/**
 * 计算单个符号的FIRST
 * @param symbol : 需要计算的符号
 * @param productions : 文法的全部产生式
 * @param allFirst : 所有符号的FIRST集
 */
void calcFirst(const Symbol &symbol, const std::set<Production> &productions, std::map<Symbol, std::set<Symbol>>& allFirst);

/**
 * 计算一个多符号表达式的FIRST
 * @param symbols : 需要计算的符号表达式
 * @param firstLeft : 已经计算完成的单个符号的FIRST
 * @param firstRight : 所有多符号表达式的FIRST
 */
void calcRightFirst(const std::vector<Symbol>& symbols, const std::map<Symbol, std::set<Symbol>>& firstLeft, std::map<std::vector<Symbol>, std::set<Symbol>>& firstRight);

/**
 * 计算单个符号的FOLLOW集
 * @param productions : 文法的全部产生式
 * @param allFirst : 计算好的FIRST集
 * @param allFollow : 所有符号的FOLLOW集
 */
void calcFollow(const std::set<Production> &productions, const std::map<Symbol, std::set<Symbol>>& allFirst, std::map<Symbol, std::set<Symbol>>& allFollow);

/**
 * 计算预测分析表
 * @param parsing_table : 预测分析表对象
 * @param productions : 所有产生式的集合
 * @param firstRight : 所有右侧表达式的FIRST集合
 * @param allFollow : 所有非终结符的FOLLOW集
 */
void getParsingTable(std::map<std::pair<Symbol, Symbol>, Production> &parsing_table, const std::set<Production> &productions, const std::map<std::vector<Symbol>, std::set<Symbol>> &firstRight, const std::map<Symbol, std::set<Symbol>>& allFollow);

void splitStr(std::string str, std::vector<Symbol> &inputStream, const std::set<Symbol> &terminals);

/**
 * 分析输入的串，【打印】它的一个最左推导
 * @param inputStream : 输入串
 * @param parsing_table : 预测分析表
 * @param start : 文法开始符号
 */
void analyze(const std::vector<Symbol> &inputStream, const std::map<std::pair<Symbol, Symbol>, Production> &parsing_table, const Symbol &start);
#endif //LAB2_HEADERS_H
