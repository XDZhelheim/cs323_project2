#ifndef NODE_ANALYSER_HPP
#define NODE_ANALYSER_HPP

#include "TreeNode.hpp"
#include "type.hpp"
#include <map>

using std::map;

map<string, Type> symbolTable;

bool isArithmatic(string name)
{
    return name == "PLUS" || name == "MINUS" || name == "MUL" || name == "DIV";
}

bool check_func_signature(vector<Type> func, vector<Type> varList)
{
    if (func.size() != varList.size()) 
    {
        return false;
    }
    for (int i = 0; i < func.size(); i++)
    {
        if (func[i].getSigniture() != varList[i].getSigniture())
        {
            return false;
        }
    }
    return true;
}

Type find_structure_mem(vector<Type> varList, string name)
{
    for (auto var : varList)
    {
        if (var.name == name)
        {
            return var;
        }
    }
    return Type(Category::ERROR_VAL);
}

class Analyser
{
private:
    TreeNode *root;

public:
    Analyser(TreeNode *node, string path)
    {
        root = node;
        out = ofstream(path);
    }

    void analyze()
    {
        analyzeProgram(root);
    }

    /*
    Program: 
      ExtDefList 
    */
    void analyzeProgram(TreeNode *node)
    {
        analyzeExtDefList(node->child[0]);
    }

    /*
    ExtDefList: 
      ExtDef ExtDefList
    | %empty
    */
    void analyzeExtDefList(TreeNode *node)
    {
        if (node->child.empty())
        {
            // empty
            return;
        }
        else
        {
            // ExtDef ExtDefList
            analyzeExtDef(node->child[0]);
            analyzeExtDefList(node->child[1]);
        }
    }

    /*
    ExtDef: 
      Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    */
    void analyzeExtDef(TreeNode *node)
    {
        if (node->child.size() == 2)
        {
            // Specifier SEMI
            analyzeSpecifier(node->child[0]);
        }
        else
        {
            Type specifier = analyzeSpecifier(node->child[0]);
            // TODO check type
            if (node->child[2]->child.empty())
            {
                // Specifier ExtDecList SEMI
                analyzeExtDecList(node->child[1], specifier);
            }
            else
            {
                // Specifier FunDec CompSt
                Type funcType(Category::FUNCTION);
                funcType.returnType=&specifier;
                analyzeFunDec(node->child[1], funcType);
                analyzeCompSt(node->child[2], funcType);
            }
        }
    }

    /*
    ExtDecList: 
      VarDec
    | VarDec COMMA ExtDecList
    */
    void analyzeExtDecList(TreeNode *node, Type specifier)
    {
        analyzeVarDec(node->child[0], specifier);

        if (node->child.size() == 3)
        {
            // VarDec COMMA ExtDecList
            analyzeExtDecList(node->child[2], specifier);
        }
    }

    /*
    Specifier: 
      TYPE
    | StructSpecifier
    */
    Type analyzeSpecifier(TreeNode *node)
    {
        if (node->child[0]->child.empty())
        {
            // TYPE
            Category category;
            if (node->child[0]->data=="int") {
                category=Category::INT_VAR;
            }
            else if (node->child[0]->data=="float") {
                category=Category::FLOAT_VAR;
            }
            else if (node->child[0]->data=="char") {
                category=Category::CHAR_VAR;
            }
            return Type(category);
        }
        else
        {
            // StructSpecifier
            return analyzeStructSpecifier(node->child[0]);
        }
    }

    /*
    StructSpecifier: 
      STRUCT ID LC DefList RC
    | STRUCT ID
    */
    Type analyzeStructSpecifier(TreeNode *node)
    {
        // TODO symbol table
        Type structSpecifier(Category::STRUCTURE);

        TreeNode* id=node->child[1];
        if (symbolTable.count(id->data)>0) {
            print_type_15(id->pos);
            return structSpecifier;
        }
        structSpecifier.name=id->data;
        symbolTable[id->data]=structSpecifier;

        if (node->child.size() == 5)
        {
            // STRUCT ID LC DefList RC
            analyzeDefList(node->child[3], structSpecifier.varlist);
        }

        return structSpecifier;
    }

    /*
    VarDec:
      ID
    | VarDec LB INT RB
    */
    void analyzeVarDec(TreeNode *node, Type &specifier)
    {
        if (node->child.size() == 1)
        {
            // TODO symbol table
            // ID
            TreeNode* id = node->child[0];

            if (symbolTable.count(id->data)>0) {
                print_type_3(id->pos);
                return;
            }

            symbolTable[id->data]=specifier;
            specifier.name=id->data;
        }
        else
        {
            // VarDec LB INT RB
            // TODO recursive
            Type arrSpecifier;
            arrSpecifier.category = Category::ARRAY;
            arrSpecifier.array = new Array(specifier, strtol(node->child[2]->data.c_str(), NULL, 0));
            analyzeVarDec(node->child[0], arrSpecifier);
        }
    }

    /*
    FunDec: 
      ID LP VarList RP
    | ID LP RP
    */
    void analyzeFunDec(TreeNode *node, Type &funcType)
    {
        // TODO symbol table

        TreeNode* id=node->child[0];
        if (symbolTable.count(id->data)>0) {
            print_type_4(id->pos);
            return;
        }
        symbolTable[id->data]=funcType;
        funcType.name=id->data;

        if (node->child.size() == 4)
        {
            // ID LP VarList RP
            analyzeVarList(node->child[2], funcType.varlist);
        }
    }

    /*
    VarList: 
      ParamDec COMMA VarList
    | ParamDec
    */
    void analyzeVarList(TreeNode *node, vector<Type> &args)
    {
        analyzeParamDec(node->child[0], args);
        if (node->child.size() == 3)
        {
            // ParamDec COMMA VarList
            analyzeVarList(node->child[2], args);
        }
    }

    /*
    ParamDec: 
      Specifier VarDec
    */
    void analyzeParamDec(TreeNode *node, vector<Type> &args)
    {
        Type specifier = analyzeSpecifier(node->child[0]);
        analyzeVarDec(node->child[1], specifier);
        args.push_back(specifier);
    }

    /*
    CompSt: 
      LC DefList StmtList RC
    */
    void analyzeCompSt(TreeNode *node, Type &funcType)
    {
        vector<Type> trash;
        analyzeDefList(node->child[0], trash);
        analyzeStmtList(node->child[1], funcType);
    }

    /*
    StmtList: 
      Stmt StmtList
    | %empty
    */
    void analyzeStmtList(TreeNode *node, Type funcType)
    {
        if (node->child.empty())
        {
            // empty
            return;
        }
        else
        {
            // Stmt StmtList
            analyzeStmt(node->child[0], funcType);
            analyzeStmtList(node->child[1], funcType);
        }
    }

    /*
    Stmt: 
      Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
    */
    void analyzeStmt(TreeNode *node, Type specifier)
    {
        if (node->child.size() == 2)
        {
            // Exp SEMI
            analyzeExp(node->child[0]);
        }
        else if (node->child.size() == 1)
        {
            // CompSt
            analyzeCompSt(node->child[0], specifier);
        }
        else if (node->child.size() == 3)
        {
            // RETURN Exp SEMI
            Type exp = analyzeExp(node->child[2]);
            if (specifier.getSigniture()!=exp.getSigniture()) {
                print_type_8(node->pos);
                return;
            }
            // TODO check type
        }
        else if (node->child.size() == 5)
        {
            // IF LP Exp RP Stmt & WHILE LP Exp RP Stmt
            analyzeExp(node->child[2]);
            analyzeStmt(node->child[4], specifier);
        }
        else
        {
            // IF LP Exp RP Stmt ELSE Stmt
            analyzeExp(node->child[2]);
            analyzeStmt(node->child[4], specifier);
            analyzeStmt(node->child[6], specifier);
        }
    }

    /*
    DefList: 
      Def DefList
    | %empty
    */
    void analyzeDefList(TreeNode *node, vector<Type>& varlist)
    {
        if (node->child.size() == 2)
        {
            // Def DefList
            analyzeDef(node->child[0], varlist);
            analyzeDefList(node->child[1], varlist);
        }
    }

    /*
    Def: 
      Specifier DecList SEMI 
    */
    void analyzeDef(TreeNode *node, vector<Type>& varlist)
    {
        Type specifier = analyzeSpecifier(node->child[0]);
        analyzeDecList(node->child[1], specifier);
        varlist.push_back(specifier);
    }

    /*
    DecList: 
      Dec
    | Dec COMMA DecList
    */
    void analyzeDecList(TreeNode *node, Type &specifier)
    {
        analyzeDec(node->child[0], specifier);
        if (node->child.size() == 3)
        {
            // Dec COMMA DecList
            analyzeDecList(node->child[2], specifier);
        }
    }

    /*
    Dec: 
      VarDec
    | VarDec ASSIGN Exp
    */
    void analyzeDec(TreeNode *node, Type &specifier)
    {
        // TODO symbol table
        analyzeVarDec(node->child[0], specifier);
        if (node->child.size() == 3)
        {
            // VarDec ASSIGN Exp
            // TODO type check
            analyzeExp(node->child[2]);
        }
    }

    /*
    Exp: 
      Exp ASSIGN Exp                type check
    | Exp AND Exp                   int, int -> int
    | Exp OR Exp                    int, int -> int
    | Exp LT Exp                    int, int -> int
    | Exp LE Exp                    int, int -> int
    | Exp GT Exp                    int, int -> int
    | Exp GE Exp                    int, int -> int
    | Exp NE Exp                    int, int -> int
    | Exp EQ Exp                    int, int -> int
    | Exp PLUS Exp                  T, T -> T
    | Exp MINUS Exp                 T, T -> T
    | Exp MUL Exp                   T, T -> T
    | Exp DIV Exp                   T, T -> T
    | LP Exp RP                     T -> T
    | MINUS Exp                     T -> -T
    | NOT Exp                       int -> int
    | ID LP Args RP                 func
    | ID LP RP
    | Exp LB Exp RB
    | Exp DOT ID
    | ID
    | INT
    | FLOAT
    | CHAR
    */
    Type analyzeExp(TreeNode *node)
    {
        if (node->child.size() == 3)
        {
            // yidadui
            if (node->child[0]->child.empty() && node->child[1]->child.empty() && node->child[2]->child.empty())
            {
                // ID LP RP
                if (symbolTable.count(node->child[0]->data))
                {
                    print_type_2(node->pos);
                    return Type(Category::ERROR_VAL);
                }
                Type exp = symbolTable[node->child[0]->data];
                if (exp.category!=Category::FUNCTION)
                {
                    print_type_11(node->pos);
                    return Type(Category::ERROR_VAL);
                }
                if (exp.varlist.size() != 0)
                {
                    print_type_9(node->pos);
                    return Type(ERROR_VAL);
                }
                return *exp.returnType;
            }
            else if (node->child[0]->child.empty() && !node->child[1]->child.empty() && node->child[2]->child.empty())
            {
                // LP Exp RP
                return analyzeExp(node->child[1]);
            }
            else if (!node->child[0]->child.empty() && node->child[1]->child.empty() && node->child[2]->child.empty())
            {
                // Exp DOT ID
                Type exp = analyzeExp(node->child[0]);
                if (exp.category != Category::STRUCTURE)
                {
                    print_type_13(node->pos);
                    return Type(ERROR_VAL);
                }
                Type t = find_structure_mem(exp.varlist, node->child[2]->data);
                if (t.category == Category::ERROR_VAL) 
                {
                    print_type_14(node->pos);
                    return Type(ERROR_VAL);
                }
                return t;
            }
            else
            {
                Type exp1 = analyzeExp(node->child[0]);
                Type exp2 = analyzeExp(node->child[2]);
                if (exp1.getSigniture() != exp2.getSigniture() || exp1.category == Category::ERROR_VAL)
                {
                    if (node->child[1]->name == "ASSIGN")
                    {
                        print_type_5(node->pos);
                    }
                    else
                    {
                        print_type_7(node->pos);
                    }
                    return Type(Category::ERROR_VAL);
                }
                if (isArithmatic(node->child[1]->name))
                {
                    return Type(exp1.category, exp1.name);
                }
                return Type(Category::INT_VAL);
            }
        }
        else if (node->child.size() == 2)
        {
            return analyzeExp(node->child[1]);
        }
        else if (node->child.size() == 4)
        {
            if (node->child[0]->child.empty())
            {
                // ID LP Args RP
                vector<Type> varList;
                analyzeArgs(node->child[2], varList);
                if (symbolTable.count(node->child[0]->data))
                {
                    print_type_2(node->pos);
                    return Type(Category::ERROR_VAL);
                }
                Type exp = symbolTable[node->child[0]->data];
                if (exp.category!=Category::FUNCTION)
                {
                    print_type_11(node->pos);
                    return Type(Category::ERROR_VAL);
                }
                if (!check_func_signature(exp.varlist, varList))
                {
                    print_type_9(node->pos);
                    return Type(ERROR_VAL);
                }
                return *exp.returnType;
            }
            else
            {
                // Exp LB Exp RB
                Type exp1 = analyzeExp(node->child[0]);
                Type exp2 = analyzeExp(node->child[2]);
                if (exp2.getSigniture() != "int")
                {
                    print_type_10(node->pos);
                    return Type(Category::ERROR_VAL);
                }
                return exp1.getChild(node->pos);
            }
        }
        else
        {
            switch (node->type)
            {
            case DataType::INT_TYPE:
                return Type(Category::INT_VAL);

            case DataType::FLOAT_TYPE:
                return Type(Category::FLOAT_VAL);

            case DataType::CHAR_TYPE:
                return Type(Category::CHAR_VAL);

            default:
                if (symbolTable.count(node->child[0]->data))
                {
                    Type t = symbolTable[node->child[0]->data];
                    if (t.category == Category::STRUCTURE && node->child[0]->data == t.name) 
                    {
                        return Type(Category::ERROR_VAL);
                    }
                    return t;
                }
                print_type_1(node->pos);
                return Type(Category::ERROR_VAL);
            }
        }
    }

    /*
    Args: 
      Exp COMMA Args
    | Exp
    */
    void analyzeArgs(TreeNode *node, vector<Type> &varList)
    {
        varList.push_back(analyzeExp(node->child[0]));
        if (node->child.size() == 3)
        {
            // Exp COMMA Args
            analyzeArgs(node->child[2], varList);
        }
    }
};

void analysisTreeNode(char *file_path)
{
    string path = file_path;
    string out_path = "/dev/stdout";
    if (path.substr(path.length() - 4) == ".spl")
    {
        out_path = path.substr(0, path.length() - 4) + ".out";
    }
    Analyser(root, out_path).analyze();
}

#endif