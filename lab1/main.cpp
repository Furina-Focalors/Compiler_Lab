#include <iostream>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <string>

struct attribute{
    explicit attribute(const std::string &_token, int val=-1);
    attribute(struct attribute const &attr);

    std::string token;
    int value;// 如果有非负值，代表符号表中的对应下标
};

attribute::attribute(const std::string &_token, int val) {
    token = _token;
    value = val;
}

attribute::attribute(const struct attribute &attr) {
    token = attr.token;
    value = attr.value;
}

std::string relop_values[6]={"LT","GT","LE","GE","EQ","NE"};
std::string operator_values[]={"ADD","SUB","MUL","DIV"};
std::string assignment_values[]={"ASSIGN", "ADDASSI", "SUBASSI", "MULASSI", "DIVASSI"};

// 自定义标识符的符号表
int top = 0, cur_size = 5;
std::string *sign_table = new std::string[cur_size];

// 将符号表的大小扩大一倍
void resize(){
    auto *temp = new std::string[2*cur_size];
    cur_size *= 2;
    for(int i=0;i<top;++i){
        temp[i] = sign_table[i];
    }
    delete[] sign_table;
    sign_table = temp;
}
// 向符号表插入新的项
void insertToSignTable(std::string &id){
    if(top==cur_size)resize();
    sign_table[top]=id;
    ++top;
}

// 所有保留字在这里直接初始化
std::unordered_map<std::string, struct attribute> attributes={
        {"int", attribute("INT")},
        {"char", attribute("CHAR")},
        {"return", attribute("RETURN")},
        {"unsigned", attribute("UNSIGNED")},
        {"const", attribute("CONST")},
        {"for", attribute("FOR")},
        {"if", attribute("IF")},
        {"else", attribute("ELSE")},
};

// functions
bool isDigit(const char &c){return c>='0'&&c<='9';}
bool isLetterOrUnderline(const char &c){return c>='A'&&c<='Z' || c>='a'&&c<='z' || c=='_';}
bool isBlank(const char &c){return c==' '||c=='\t'||c=='\n';}
void printInfo(const char* lexeme, const char* token, int value=-1);
void readFile(std::string &filePath, std::string &target);

int main() {
    std::string fp = "mycode.txt";
    std::string commentedCode;
    readFile(fp,commentedCode);
    if(commentedCode.empty()){
        std::cerr << "Cannot read file.\n";
        return -1;
    }

    int len = commentedCode.length(), cur = -1;
    // 去除注释
    std::string code;
    for(int i=0;i<len;++i){
        if(commentedCode[i]=='/'){
            if(i+1<len&&commentedCode[i+1]=='/'){// comment lines
                while(commentedCode[i]!='\n')++i;
            }else if(i+1<len&&commentedCode[i+1]=='*'){// comment blocks
                while(commentedCode[i]!='*'||commentedCode[i+1]!='/'){
                    if(i+1>=len){
                        std::cout<<"Unclosed comment block."<< std::endl;
                        return -1;
                    }
                    ++i;
                }
                i+=2;
            }
        }
        if(i<len)code.push_back(commentedCode[i]);
    }
    //std::cout<<code<<'\n'<<code.length();

    int state = 0;
    std::cout<<"===============Start Analyzing===============\n";
    std::cout<<std::setw(15)<<std::left<<"Lexeme"<<std::setw(15)<<std::left<<"Token"<<std::setw(15)<<std::left<<"Value"<<std::endl;
    while(cur<len){
        char next;
        if(cur+1<len)next=code[cur+1];
        else next='\0';
        // 跳过空格和制表符
        if(isBlank(next)){
            ++cur;
            if(cur+1<len)next = code[cur+1];
            else next = '\0';
            continue;
        }
        switch(state){
            case 0:// START
                if(!next) { ++cur;break; }// finished
                if(next=='\'') { state = 1;++cur; }
                else if(next=='+') { state = 7;++cur; }
                else if(next=='-') { state = 9;++cur; }
                else if(next=='*') { state = 12;++cur; }
                else if(next=='/') { state = 14;++cur; }
                else if(next=='<') { state = 16;++cur; }
                else if(next=='>') { state = 18;++cur; }
                else if(next=='=') { state = 20;++cur; }
                else if(next=='!') { state = 22;++cur; }
                else if(next=='(') { printInfo("(", "OPAREN");++cur; }
                else if(next==')') { printInfo(")", "CPAREN");++cur; }
                else if(next==',') { printInfo(",", "COMMA");++cur; }
                else if(next=='{') { printInfo("{", "OCURLY");++cur; }
                else if(next=='}') { printInfo("}", "CCURLY");++cur; }
                else if(next==';') { printInfo(";", "SEMI");++cur; }
                else if(isDigit(next)){// NUMINT
                    std::string num;
                    // repetitively read in number
                    num.push_back(next);
                    ++cur;
                    if(cur+1<len)next = code[cur+1];
                    else next = '\0';
                    while(isDigit(next)){
                        num.push_back(next);
                        ++cur;
                        if(cur+1<len)next = code[cur+1];
                        else next = '\0';
                    }
                    printInfo(num.c_str(), "NUMINT");
                }
                else if(isLetterOrUnderline(next)){// ID or RESERVED
                    std::string lexeme;
                    // repetitively read in id
                    lexeme.push_back(next);
                    ++cur;
                    if(cur+1<len)next = code[cur+1];
                    else next = '\0';
                    while(isLetterOrUnderline(next) || isDigit(next)){
                        lexeme.push_back(next);
                        ++cur;
                        if(cur+1<len)next = code[cur+1];
                        else next = '\0';
                    }
                    // 查找属性表
                    auto iter = attributes.find(lexeme);
                    if(iter != attributes.end()){// An existing lexeme
                        struct attribute attr = iter->second;
                        printInfo(lexeme.c_str(), attr.token.c_str(), attr.value);
                    }else{// A new identifier
                        struct attribute attr("ID",top);
                        attributes.insert({lexeme,attr});
                        printInfo(lexeme.c_str(), "ID", top);
                        insertToSignTable(lexeme);
                    }
                }else{
                    std::cout<<"Invalid input. Failed at len="<<cur<<std::endl;
                    return -1;
                }
                break;
            case 1:
                if(next=='\\') { state = 4;++cur; }
                else if(!next||next=='\n'||next=='\t'||next=='\''){
                    std::cout<<"Invalid char. Failed at len="<<cur<<std::endl;
                    return -1;
                }else { state = 2;++cur; }
                break;
            case 2:
                if(next=='\'') { state = 3;++cur; }
                else{
                    std::cout<<"Invalid char. Failed at len="<<cur<<std::endl;
                    return -1;
                }
                break;
            case 3:// CHARACTER
                state=0;
                char lexeme[4];
                lexeme[0]=lexeme[2]='\'';lexeme[1]=code[cur-1];
                printInfo(lexeme,"CHARACTER");
                break;
            case 4:
                if(!next||next=='\n'||next=='\t') { state = 5;++cur; }
                else{
                    std::cout<<"Invalid char. Failed at len="<<cur<<std::endl;
                    return -1;
                }
                break;
            case 5:
                if(next=='\'') { state = 3;++cur; }
                else{
                    std::cout<<"Invalid char. Failed at len="<<cur<<std::endl;
                    return -1;
                }
                break;
            case 7:// OP_0
                if(next=='=') { state = 10;++cur; }
                else if(next=='+') { state = 24;++cur; }
                else{
                    state=0;
                    printInfo("+", "OP", 0);
                }
                break;
            case 9:// OP_1
                if(next=='=') { state = 11;++cur; }
                else{
                    state=0;
                    printInfo("-", "OP", 1);
                }
                break;
            case 10: // ASSIGN_0
                state=0;
                printInfo("+=", "ASSIGN", 0);
                break;
            case 11: // ASSIGN_1
                state=0;
                printInfo("-=", "ASSIGN", 1);
                break;
            case 12: // OP_2
                if(next=='=') { state = 13;++cur; }
                else{
                    state=0;
                    printInfo("*", "OP", 2);
                }
                break;
            case 13: // ASSIGN_2
                state=0;
                printInfo("*=", "ASSIGN", 2);
                break;
            case 14: // OP_3
                if(next=='=') { state = 15;++cur; }
                else{
                    state=0;
                    printInfo("/", "OP", 3);
                }
                break;
            case 15: // ASSIGN_3
                state=0;
                printInfo("/=", "ASSIGN", 3);
                break;
            case 16: // RELOP_0
                if(next=='=') { state = 17;++cur; }
                else{
                    state=0;
                    printInfo("<", "RELOP", 0);
                }
                break;
            case 17: // RELOP_1
                state=0;
                printInfo("<=", "RELOP", 1);
                break;
            case 18: // RELOP_2
                if(next=='=') { state = 19;++cur; }
                else{
                    state=0;
                    printInfo(">", "RELOP", 2);
                }
                break;
            case 19: // RELOP_3
                state=0;
                printInfo(">=", "RELOP", 3);
                break;
            case 20: // ASSIGN_4
                if(next=='=') { state = 21;++cur; }
                else{
                    state=0;
                    printInfo("=", "ASSIGN", 4);
                }
                break;
            case 21: // RELOP_4
                state=0;
                printInfo("==", "RELOP", 4);
                break;
            case 22:
                if(next=='=') { state = 23;++cur; }
                else{
                    std::cout<<"Unexpected input. Failed at len="<<cur<<std::endl;
                    return -1;
                }
                break;
            case 23: // RELOP_5
                state=0;
                printInfo("!=", "RELOP", 4);
                break;
            case 24: // OP_4
                state=0;
                printInfo("++", "OP", 4);
                break;
            default:
                std::cerr<<"System failed."<<std::endl;
                return -1;
                break;
        }
    }
    return 0;
}

// 读取文件
void readFile(std::string &filePath, std::string &target){
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    std::string line;
    while(std::getline(inputFile, line)){
        target.append(line+'\n');
    }

    inputFile.close();
}

void printInfo(const char* lexeme, const char* token, int value){
    std::cout<<std::left<<std::setw(15)<<lexeme<<std::left<<std::setw(15)<<token;
    if(value==-1)std::cout<<std::left<<std::setw(15)<<"-"<<std::endl;
    else std::cout<<std::left<<std::setw(15)<<value<<std::endl;
}
