#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

enum TokenType
{
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    STRING,
    OPERATOR,
    COMMENT,
    SEPARATOR
};

const vector<char> separators
{
    '.', ',', ' ', '\t',
    '\r', '\n', ';', ':',
    '{', '}',
    '[', ']',
    '(', ')'
};

const vector<string> comment
{
    "/*", "*/", "//"
};

const vector<string> keywords
{
    "break", "case", "class", "catch", "const", "continue",
    "debugger", "default", "delete", "do", "else", "export"
    "extends", "finally", "for", "function" "if", "import", "in",
    "instanceof", "let", "new", "return", "super", "switch",
    "this", "throw", "try", "typeof", "var", "void", "while",
    "with", "yield", "enum", "await", "implements", "package",
    "protected", "static", "interface", "private", "public", "const"
};

const vector<string> operators
{
    "+", "-", "*", "/", "%", "=",
    "+=", "-=", "*=", "/=", "%=",
    "==", "===", ">=", ">", "<=", "??",
    "<", "!=", "!==", "&&", "||", "|",
    "!", "^", ">>>", "&", "^", ">>",
    "+", "=>", "++", "--", "?", "**"
};


bool IsSeparator(char word) 
{
    if (find(separators.begin(), separators.end(), word) == separators.end()) return false;
    else return true;
}

bool IsNumber(string word)
{
    regex pattern("[+-]?\\d*\\.?\\d+");

    return regex_match(word, pattern);
}

bool IsKeyword(string word)
{
    if (find(keywords.begin(), keywords.end(), word) == keywords.end()) return false;
    else return true;
}

bool IsOperator(string word)
{
    if (find(operators.begin(), operators.end(), word) == operators.end()) return false;
    else return true;
}


class Token 
{
public:
    Token(TokenType type, string lexem)
    {
        this->type = SetType(type);
        this->lexem = lexem;
    }
    string SetType(TokenType type)
    {
        switch (type)
        {
            case KEYWORD:
                return "KEYWORD";
                break;
            case  IDENTIFIER:
                return "IDENTIFIER";
                break;
            case NUMBER:
                return "NUMBER";
                break;
            case STRING:
                return "STRING";
                break;
            case OPERATOR:
                return "OPERATOR";
                break;
            case COMMENT:
                return "COMMENT";
                break;
            case SEPARATOR:
                return "SEPARATOR";
                break;
            default:
                return "";
                break;
        }
    }
    string type;
    string lexem;
};

class Tokener
{
private:
    string path;
public:
    Tokener(const string filePath)
    {
        this->path = filePath;
    }
    
    vector<Token> TokenizeFile()
    {
        vector<Token> result;
        bool isSingleComment = false;
        bool isNumerousComment = false;
        vector<char> word;
        bool isString = false;
        ifstream fin;
        fin.open(path);
        if (fin.is_open())
        {
            char fch; 
            char ch; 
            fin.get(fch);
            while (fin.get(ch))
            {
                if (!isSingleComment && !isNumerousComment && !isString)
                {
                    if (fch == '/') 
                    { 
                        if (result.back().lexem == "/") 
                        { 
                            result.back().lexem = "//";
                            result.back().type = COMMENT;
                            isSingleComment = true; 
                        }
                    }
                    if (fch == '*') 
                    { 
                        if (result.back().lexem == "/") 
                        {
                            result.back().lexem = "/*";
                            result.back().type = COMMENT;
                            isNumerousComment = true; 
                        }
                    }
                }
                if (isSingleComment || isNumerousComment)  
                {
                    if (isSingleComment) 
                    {
                        if (fch == '\n') 
                        {
                            if (!word.empty())
                            {
                                string newWord(word.begin(), word.end());

                                word.clear();

                                Token newToken(STRING, newWord);
                                
                                result.push_back(newToken);
                            }

                            isSingleComment = false;

                        }
                        else
                        {
                            word.push_back(fch);
                        }
                    }

                    if (isNumerousComment) 
                    {
                        if (fch == '*' && ch == '/') 
                        {
                            string newWord(word.begin(), word.end());

                            word.clear();

                            Token newToken(STRING, newWord);

                            Token endCommentToken(COMMENT, "*/");
                            result.push_back(newToken);
                        }
                        else
                        {
                            word.push_back(fch);
                        }
                    }
                }
                else
                {
                    if (!isString && (fch == '"' || fch == '\''))
                    {
                        isString = true;
                        string s(1, fch);
                        Token separatorToken(SEPARATOR, s);
                        result.push_back(separatorToken);
                        fch = ch;
                        continue;
                    }
                    if (isString)
                    {
                        if (fch != '"' && fch != '\'')
                        {
                            word.push_back(fch);
                        }
                        else
                        {
                            string s(1, fch);

                            Token separatorToken(SEPARATOR, s);

                            
                            string newWord(word.begin(), word.end());

                            word.clear();

                            Token newToken(STRING, newWord);

                            result.push_back(newToken);
                            
                            result.push_back(separatorToken);

                            isString = false;
                        }
                    }
                    else
                    {
                        string s(1, fch);

                        if (IsSeparator(fch)) 
                        {
                            if (fch == '.' && word.back() >= '0' && word.back() <= '9')
                            {
                                word.push_back(fch);
                                fch = ch;
                                continue;
                            }
                            string newWord(word.begin(), word.end());
                            if (!newWord.empty())
                            {
                                if (IsKeyword(newWord))
                                {
                                    Token keywordToken(KEYWORD, newWord);
                                    result.push_back(keywordToken);
                                }
                                else if (IsNumber(newWord))
                                {
                                    Token numberToken(NUMBER, newWord);
                                    result.push_back(numberToken);
                                }
                                else
                                {
                                    Token identifierToken(IDENTIFIER, newWord);
                                    result.push_back(identifierToken);
                                }
                                word.clear();
                            }
                            string separator(1, fch);
                            if (separator != " " && separator != "\n" && separator != "\t")
                            {
                                Token separatorToken(SEPARATOR, separator);
                                result.push_back(separatorToken);
                            }                            
                        }
                        else if (IsOperator(s)) 
                        {
                            string newWord(word.begin(), word.end());
                            if (!newWord.empty())
                            {
                                if (IsKeyword(newWord))
                                {
                                    Token keywordToken(KEYWORD, newWord);
                                    result.push_back(keywordToken);
                                }
                                else if (IsNumber(newWord))
                                {
                                    Token numberToken(NUMBER, newWord);
                                    result.push_back(numberToken);
                                }
                                else
                                {
                                    Token identifierToken(IDENTIFIER, newWord);
                                    result.push_back(identifierToken);
                                }
                                word.clear();
                            }
                            switch (fch)
                            {
                            case '+': {
                                string operatorString = "+";
                                if (result.back().lexem == "++") break;
                                if (ch == '+') operatorString = "++";
                                if (ch == '=') operatorString = "+=";
                                Token operatorToken(OPERATOR, operatorString);
                                result.push_back(operatorToken);
                                break;
                            }
                            case '-': {
                                string operatorString = "-";
                                if (result.back().lexem == "--") break;
                                if (ch == '-') operatorString = "--";
                                if (ch == '=') operatorString = "-=";
                                Token operatorToken(OPERATOR, operatorString);
                                result.push_back(operatorToken);
                                break;
                            }
                            case '/': {
                                string operatorString = "/";
                                if (ch == '=') operatorString = "/=";
                                Token operatorToken(OPERATOR, operatorString);
                                result.push_back(operatorToken);
                                break;
                            }
                            case '%': {
                                string operatorString = "%";
                                if (ch == '=') operatorString = "%=";
                                Token operatorToken(OPERATOR, operatorString);
                                result.push_back(operatorToken);
                                break;
                            }
                            case '*': {
                                string operatorString = "*";
                                if (result.back().lexem == "**") break;
                                if (ch == '*') operatorString = "**";
                                if (ch == '=') operatorString = "*=";
                                Token operatorToken(OPERATOR, operatorString);
                                result.push_back(operatorToken);
                                break;
                            }
                                
                            case '=': {
                                string operatorString = "=";
                                if (result.back().lexem == "===") break;
                                if (result.back().lexem == "==" && ch != '=') break;
                                if (result.back().lexem == "+=") break;
                                if (result.back().lexem == "-=") break;
                                if (result.back().lexem == "*=") break;
                                if (result.back().lexem == "/=") break;
                                if (result.back().lexem == "%=") break;
                                if (result.back().lexem == ">=") break;
                                if (result.back().lexem == "<=") break;
                                if (result.back().lexem == "!=") break;
                                if (ch == '>') operatorString = "=>";
                                if (ch == '=') operatorString = "===";
                                Token operatorToken(OPERATOR, operatorString);
                                result.push_back(operatorToken);
                                break;
                            }
                            case '>': {
                                string operatorString = ">";
                                if (result.back().lexem == ">>") break;
                                if (ch == '>') operatorString = ">>";
                                if (ch == '=') operatorString = ">=";
                                Token operatorToken(OPERATOR, operatorString);
                                result.push_back(operatorToken);
                                break;
                            }
                            case '<': {
                                string operatorString = "<";
                                if (result.back().lexem == "<<") break;
                                if (ch == '<') operatorString = "<<";
                                if (ch == '=') operatorString = "<=";
                                Token operatorToken(OPERATOR, operatorString);
                                result.push_back(operatorToken);
                                break;
                            }
                            case '&': {
                                string operatorString = "&";
                                if (result.back().lexem == "&&") break;
                                if (ch == '&') operatorString = "&&";
                                Token operatorToken(OPERATOR, operatorString);
                                result.push_back(operatorToken);
                                break;
                            }
                                break;
                            case '|': {
                                string operatorString = "|";
                                if (result.back().lexem == "||") break;
                                if (ch == '|') operatorString = "||";
                                Token operatorToken(OPERATOR, operatorString);
                                result.push_back(operatorToken);
                                break;
                            }
                                break;
                            case '!': {
                                string operatorString = "!";
                                if (ch == '=') operatorString = "!=";
                                Token operatorToken(OPERATOR, operatorString);
                                result.push_back(operatorToken);
                                break;
                            }
                            case '?': {
                                string operatorString = "?";
                                if (result.back().lexem == "??") break;
                                if (ch == '?') operatorString = "??";
                                Token operatorToken(OPERATOR, operatorString);
                                result.push_back(operatorToken);
                                break;
                            }
                            default:
                                break;
                            }
                        }
                        else {
                            word.push_back(fch);
                        }
                    }
                }
                fch = ch;
            }
            if (!isSingleComment && !isNumerousComment && !isString)
            {
                if (fch == '/')
                {
                    if (result.back().lexem == "/")
                    {
                        result.back().lexem = "//";
                        result.back().type = COMMENT;
                        isSingleComment = true;
                    }
                }
                if (fch == '*')
                {
                    if (result.back().lexem == "/")
                    {
                        result.back().lexem = "/*";
                        result.back().type = COMMENT;
                        isNumerousComment = true;
                    }
                }
            }
            if (isSingleComment || isNumerousComment) 
            {
                if (isSingleComment) 
                {
                    if (fch == '\n') 
                    {
                        if (!word.empty())
                        {
                            string newWord(word.begin(), word.end());

                            word.clear();

                            Token newToken(STRING, newWord);

                            result.push_back(newToken);
                        }

                        isSingleComment = false;

                    }
                    else
                    {
                        word.push_back(fch);
                    }
                }

                if (isNumerousComment) 
                {
                    if (fch == '*' && ch == '/') 
                    {
                        string newWord(word.begin(), word.end());

                        word.clear();

                        Token newToken(STRING, newWord);

                        Token endCommentToken(COMMENT, "*/");
                        result.push_back(newToken);
                    }
                    else
                    {
                        word.push_back(fch);
                    }
                }
            }
            else
            {
                if (!isString && (fch == '"' || fch == '\''))
                {
                    isString = true;
                    string s(1, fch);
                    Token separatorToken(SEPARATOR, s);
                    result.push_back(separatorToken);
                    fch = ch;
                }
                if (isString)
                {
                    if (fch != '"' && fch != '\'')
                    {
                        word.push_back(fch);
                    }
                    else
                    {
                        string s(1, fch);

                        Token separatorToken(SEPARATOR, s);


                        string newWord(word.begin(), word.end());

                        word.clear();

                        Token newToken(STRING, newWord);

                        result.push_back(newToken);

                        result.push_back(separatorToken);

                        isString = false;
                    }
                }
                else
                {
                    string s(1, fch);

                    if (IsSeparator(fch))// ÿךשמ נמחה³כüםטך, עמ ןונוג³נÿ÷למ, שמ חא סכמגמ
                    {
                        if (fch == '.' && word.back() >= '0' && word.back() <= '9')
                        {
                            word.push_back(fch);
                            fch = ch;
                        }
                        string newWord(word.begin(), word.end());
                        if (!newWord.empty())
                        {
                            if (IsKeyword(newWord))
                            {
                                Token keywordToken(KEYWORD, newWord);
                                result.push_back(keywordToken);
                            }
                            else if (IsNumber(newWord))
                            {
                                Token numberToken(NUMBER, newWord);
                                result.push_back(numberToken);
                            }
                            else
                            {
                                Token identifierToken(IDENTIFIER, newWord);
                                result.push_back(identifierToken);
                            }
                            word.clear();
                        }
                        string separator(1, fch);
                        if (separator != " " && separator != "\n" && separator != "\t")
                        {
                            Token separatorToken(SEPARATOR, separator);
                            result.push_back(separatorToken);
                        }
                    }
                    else if (IsOperator(s))
                    {
                        string newWord(word.begin(), word.end());
                        if (!newWord.empty())
                        {
                            if (IsKeyword(newWord))
                            {
                                Token keywordToken(KEYWORD, newWord);
                                result.push_back(keywordToken);
                            }
                            else if (IsNumber(newWord))
                            {
                                Token numberToken(NUMBER, newWord);
                                result.push_back(numberToken);
                            }
                            else
                            {
                                Token identifierToken(IDENTIFIER, newWord);
                                result.push_back(identifierToken);
                            }
                            word.clear();
                        }
                        switch (fch)
                        {
                        case '+': {
                            string operatorString = "+";
                            if (result.back().lexem == "++") break;
                            if (ch == '+') operatorString = "++";
                            if (ch == '=') operatorString = "+=";
                            Token operatorToken(OPERATOR, operatorString);
                            result.push_back(operatorToken);
                            break;
                        }
                        case '-': {
                            string operatorString = "-";
                            if (result.back().lexem == "--") break;
                            if (ch == '-') operatorString = "--";
                            if (ch == '=') operatorString = "-=";
                            Token operatorToken(OPERATOR, operatorString);
                            result.push_back(operatorToken);
                            break;
                        }
                        case '/': {
                            string operatorString = "/";
                            if (ch == '=') operatorString = "/=";
                            Token operatorToken(OPERATOR, operatorString);
                            result.push_back(operatorToken);
                            break;
                        }
                        case '%': {
                            string operatorString = "%";
                            if (ch == '=') operatorString = "%=";
                            Token operatorToken(OPERATOR, operatorString);
                            result.push_back(operatorToken);
                            break;
                        }
                        case '*': {
                            string operatorString = "*";
                            if (result.back().lexem == "**") break;
                            if (ch == '*') operatorString = "**";
                            if (ch == '=') operatorString = "*=";
                            Token operatorToken(OPERATOR, operatorString);
                            result.push_back(operatorToken);
                            break;
                        }

                        case '=': {
                            string operatorString = "=";
                            if (result.back().lexem == "===") break;
                            if (result.back().lexem == "==" && ch != '=') break;
                            if (result.back().lexem == "+=") break;
                            if (result.back().lexem == "-=") break;
                            if (result.back().lexem == "*=") break;
                            if (result.back().lexem == "/=") break;
                            if (result.back().lexem == "%=") break;
                            if (result.back().lexem == ">=") break;
                            if (result.back().lexem == "<=") break;
                            if (result.back().lexem == "!=") break;
                            if (ch == '>') operatorString = "=>";
                            if (ch == '=') operatorString = "===";
                            Token operatorToken(OPERATOR, operatorString);
                            result.push_back(operatorToken);
                            break;
                        }
                        case '>': {
                            string operatorString = ">";
                            if (result.back().lexem == ">>") break;
                            if (ch == '>') operatorString = ">>";
                            if (ch == '=') operatorString = ">=";
                            Token operatorToken(OPERATOR, operatorString);
                            result.push_back(operatorToken);
                            break;
                        }
                        case '<': {
                            string operatorString = "<";
                            if (result.back().lexem == "<<") break;
                            if (ch == '<') operatorString = "<<";
                            if (ch == '=') operatorString = "<=";
                            Token operatorToken(OPERATOR, operatorString);
                            result.push_back(operatorToken);
                            break;
                        }
                        case '&': {
                            string operatorString = "&";
                            if (result.back().lexem == "&&") break;
                            if (ch == '&') operatorString = "&&";
                            Token operatorToken(OPERATOR, operatorString);
                            result.push_back(operatorToken);
                            break;
                        }
                                break;
                        case '|': {
                            string operatorString = "|";
                            if (result.back().lexem == "||") break;
                            if (ch == '|') operatorString = "||";
                            Token operatorToken(OPERATOR, operatorString);
                            result.push_back(operatorToken);
                            break;
                        }
                                break;
                        case '!': {
                            string operatorString = "!";
                            if (ch == '=') operatorString = "!=";
                            Token operatorToken(OPERATOR, operatorString);
                            result.push_back(operatorToken);
                            break;
                        }
                        case '?': {
                            string operatorString = "?";
                            if (result.back().lexem == "??") break;
                            if (ch == '?') operatorString = "??";
                            Token operatorToken(OPERATOR, operatorString);
                            result.push_back(operatorToken);
                            break;
                        }
                        default:
                            break;
                        }
                    }
                    else {
                        word.push_back(fch);
                    }
                }
            }
            fch = ch;
        }
        
        fin.close();
        
        return result;
    }
};

vector<Token> tokens;




void PrintResut(vector<Token> tokens)
{
    cout << setw(15) << "LEXEM" << setw(30) << "TOKEN TYPE" << endl;
    for (auto token : tokens)
    {
        string start = "", end = "";
        if (token.type == "KEYWORD") { start = "\033[31m"; end = "\033[0m"; }
        else if(token.type == "IDENTIFIER") { start = "\033[33m"; end = "\033[0m"; }
        else if (token.type == "NUMBER") { start = "\033[32m"; end = "\033[0m"; }
        else if (token.type == "STRING") { start = "\033[31m"; end = "\033[0m"; }
        else if (token.type == "OPERATOR") { start = "\033[34m"; end = "\033[0m"; }
        else if (token.type == "COMMENT") { start = "\033[36m"; end = "\033[0m"; }
        else { start = "\033[37m"; end = "\033[0m"; }
        cout << start << setw(15) << token.lexem << setw(30) << token.type << end <<  endl;
    }
}

int main()
{
    string path = "code.txt";
    Tokener tokener(path);
    vector<Token> result = tokener.TokenizeFile();
    PrintResut(result);
    return 0;
}
