#ifndef NODE_ANALYSER_HPP
#define NODE_ANALYSER_HPP

#include "TreeNode.hpp"
#include "type.hpp"
#include <map>

using std::map;

map<string, Type> symbolTable;

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
                analyzeFunDec(node->child[1], specifier);
                analyzeCompSt(node->child[2], specifier);
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
            // TODO check type
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
        if (node->child.size() == 5)
        {
            // TODO symbol table
            // STRUCT ID LC DefList RC
            analyzeDefList(node->child[3]);
        }
        else
        {
            // TODO check struct
        }
    }

    /*
    VarDec:
      ID
    | VarDec LB INT RB
    */
    void analyzeVarDec(TreeNode *node, Type specifier)
    {
        if (node->child.size() == 1)
        {
            // TODO symbol table
            // ID
            TreeNode* id = node->child[0];
            symbolTable[id->data]=specifier;
        }
        else
        {
            // VarDec LB INT RB
            // TODO recursive
            Type arrSpecifier;
            arrSpecifier.category=Category::ARRAY;
            arrSpecifier.array=new Array(specifier, strtol(node->child[2]->data.c_str(), NULL, 0));
            analyzeVarDec(node->child[0], arrSpecifier);
        }
    }

    /*
    FunDec: 
      ID LP VarList RP
    | ID LP RP
    */
    void analyzeFunDec(TreeNode *node, Type specifier)
    {
        // TODO symbol table
        Type func;
        if (node->child.size() == 4)
        {
            // ID LP VarList RP
            analyzeVarList(node->child[2], func.varlist);
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
        // TODO recursive
        analyzeVarDec(node->child[1], specifier);
    }

    /*
    CompSt: 
      LC DefList StmtList RC
    */
    void analyzeCompSt(TreeNode *node, Type specifier)
    {
        analyzeDefList(node->child[0]);
        analyzeStmtList(node->child[1], specifier);
    }

    /*
    StmtList: 
      Stmt StmtList
    | %empty
    */
    void analyzeStmtList(TreeNode *node, Type specifier)
    {
        if (node->child.empty())
        {
            // empty
            return;
        }
        else
        {
            // Stmt StmtList
            analyzeStmt(node->child[0], specifier);
            analyzeStmtList(node->child[1], specifier);
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
            // TODO check type
        }
        else if (node->child.size() == 5)
        {
            // IF LP Exp RP Stmt & WHILE LP Exp RP Stmt
            analyzeExp(node->child[2], 1);
            analyzeStmt(node->child[4], specifier);
        }
        else
        {
            // IF LP Exp RP Stmt ELSE Stmt
            analyzeExp(node->child[2], 1);
            analyzeStmt(node->child[4], specifier);
            analyzeStmt(node->child[6], specifier);
        }
    }

    /*
    DefList: 
      Def DefList
    | %empty
    */
    void analyzeDefList(TreeNode *node)
    {
        if (node->child.size() == 2)
        {
            // Def DefList
            analyzeDef(node->child[0]);
            analyzeDefList(node->child[1]);
        }
        else
        {
            // empty
            return;
        }
    }

    /*
    Def: 
      Specifier DecList SEMI 
    */
    void analyzeDef(TreeNode *node)
    {
        Type specifier = analyzeSpecifier(node->child[0]);
        analyzeDecList(node->child[1], specifier);
    }

    /*
    DecList: 
      Dec
    | Dec COMMA DecList
    */
    void analyzeDecList(TreeNode *node, Type specifier)
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
    void analyzeDec(TreeNode *node, Type specifier)
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
      Exp ASSIGN Exp
    | Exp AND Exp
    | Exp OR Exp
    | Exp LT Exp
    | Exp LE Exp
    | Exp GT Exp
    | Exp GE Exp
    | Exp NE Exp
    | Exp EQ Exp
    | Exp PLUS Exp
    | Exp MINUS Exp
    | Exp MUL Exp
    | Exp DIV Exp
    | LP Exp RP
    | MINUS Exp
    | NOT Exp
    | ID LP Args RP
    | ID LP RP
    | Exp LB Exp RB
    | Exp DOT ID
    | ID
    | INT
    | FLOAT
    | CHAR
    */
    Type analyzeExp(TreeNode *node, bool needBool = 0)
    {
        // TODO type check needBool check
        if (node->child.size() == 3)
        {
            // yidadui
            if (node->child[0]->child.empty() && node->child[1]->child.empty() && node->child[2]->child.empty())
            {
                // ID LP RP
                return;
            }
            else if (node->child[0]->child.empty() && !node->child[1]->child.empty() && node->child[2]->child.empty())
            {
                // LP Exp RP
                analyzeExp(node->child[1]);
            }
            else if (!node->child[0]->child.empty() && node->child[1]->child.empty() && node->child[2]->child.empty())
            {
                // Exp DOT ID
                analyzeExp(node->child[0]);
            }
            else
            {
                analyzeExp(node->child[0]);
                analyzeExp(node->child[2]);
            }
        }
        else if (node->child.size() == 2)
        {
            analyzeExp(node->child[1]);
        }
        else if (node->child.size() == 4)
        {
            if (node->child[0]->child.empty())
            {
                // ID LP Args RP
                analyzeArgs(node->child[2]);
            }
            else
            {
                // Exp LB Exp RB
                analyzeExp(node->child[0]);
                analyzeExp(node->child[2]);
            }
        }
        else
        {
            Type t;
            switch (node->type)
            {
            case DataType::INT_TYPE:
                t.category = Category::INT_VAL;
                break;
                
            case DataType::FLOAT_TYPE:
                t.category = Category::FLOAT_VAL;
                break;
                
            case DataType::CHAR_TYPE:
                t.category = Category::CHAR_VAL;
                break;
            
            default:
                // TODO ID
                break;
            }
            return t;
        }
    }

    /*
    Args: 
      Exp COMMA Args
    | Exp
    */
    void analyzeArgs(TreeNode *node)
    {
        analyzeExp(node->child[0]);
        if (node->child.size() == 3)
        {
            // Exp COMMA Args
            analyzeArgs(node->child[2]);
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