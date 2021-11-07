#ifndef NODE_ANALYSER_HPP
#define NODE_ANALYSER_HPP

#include "TreeNode.hpp"
#include "type.hpp"
#include <map>

using std::map;

map<string, Type *> symbolTable;

class Analyser
{
private:
    TreeNode *root;
    ofstream out;

public:
    Analyser(TreeNode *node, string path)
    {
        root = node;
        out = ofstream(path);
    }

    // TODO Specifier & return 
    // TODO Exps

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
    Specifier: 
      TYPE
    | StructSpecifier
    */
    void analyzeExtDef(TreeNode *node)
    {
        if (node->child.size() == 2)
        { 
            // Specifier SEMI
            analyzeSpecifier(node->child[0]);
        }
        else if (node->child[2]->child.empty())
        { 
            // Specifier ExtDecList SEMI
            analyzeSpecifier(node->child[0]);
            analyzeExtDecList(node->child[1]);
        }
        else
        { 
            // Specifier FunDec CompSt
            analyzeSpecifier(node->child[0]);
            analyzeFunDec(node->child[1]);
            analyzeCompSt(node->child[2]);
        }
    }

    /*
    ExtDecList: 
      VarDec
    | VarDec COMMA ExtDecList
    */
    void analyzeExtDecList(TreeNode *node)
    {
        if (node->child.size() == 1)
        { 
            // VarDec
            analyzeVarDec(node->child[0]);
        }
        else
        { 
            // VarDec COMMA ExtDecList
            analyzeVarDec(node->child[0]);
            analyzeExtDecList(node->child[2]);
        }
    }

    /*
    Specifier: 
      TYPE
    | StructSpecifier
    */
    void analyzeSpecifier(TreeNode *node)
    {
        if (node->child[0]->child.empty())
        { 
            // TYPE
            // TODO type
        }
        else
        { 
            // StructSpecifier
            analyzeStructSpecifier(node->child[0]);
        }
    }

    /*
    StructSpecifier: 
      STRUCT ID LC DefList RC
    | STRUCT ID
    */
    void analyzeStructSpecifier(TreeNode *node)
    {
        // TODO ID
        if (node->child.size() == 5)
        { 
            // STRUCT ID LC DefList RC
            analyzeDefList(node->child[3]);
        }
    }

    /*
    VarDec:
      ID
    | VarDec LB INT RB
    */
    void analyzeVarDec(TreeNode *node)
    {
        if (node->child.size() == 1)
        { 
            // TODO ID
        }
        else
        { 
            // VarDec LB INT RB
            analyzeVarDec(node->child[0]);
            // TODO [index]
        }
    }

    /*
    FunDec: 
      ID LP VarList RP
    | ID LP RP
    */
    void analyzeFunDec(TreeNode *node)
    {
        // TODO ID
        if (node->child.size() == 4)
        { 
            // ID LP VarList RP
            analyzeVarList(node->child[2]);
        }
    }

    /*
    VarList: 
      ParamDec COMMA VarList
    | ParamDec
    */
    void analyzeVarList(TreeNode *node)
    {
        analyzeParamDec(node->child[0]);
        if (node->child.size() == 3)
        { 
            // ParamDec COMMA VarList
            analyzeVarList(node->child[2]);
        }
    }

    /*
    ParamDec: 
      Specifier VarDec
    */
    void analyzeParamDec(TreeNode *node)
    {
        analyzeSpecifier(node->child[0]);
        analyzeVarDec(node->child[1]);
    }

    /*
    CompSt: 
      LC DefList StmtList RC
    */
    void analyzeCompSt(TreeNode *node)
    {
        analyzeDefList(node->child[0]);
        analyzeStmtList(node->child[1]);
    }

    /*
    StmtList: 
      Stmt StmtList
    | %empty
    */
    void analyzeStmtList(TreeNode *node)
    {
        if (node->child.empty())
        { 
            // empty
            return;
        }
        else
        { 
            // Stmt StmtList
            analyzeStmt(node->child[0]);
            analyzeStmtList(node->child[1]);
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
    void analyzeStmt(TreeNode *node)
    {
        if (node->child.size() == 2)
        { 
            // Exp SEMI
            analyzeExp(node->child[0]);
        }
        else if (node->child.size() == 1)
        { 
            // CompSt
            analyzeCompSt(node->child[0]);
        }
        else if (node->child.size() == 3)
        { 
            // RETURN Exp SEMI
            // TODO return
            analyzeExp(node->child[2]);
        }
        else if (node->child.size() == 5)
        { 
            // IF LP Exp RP Stmt & WHILE LP Exp RP Stmt
            analyzeExp(node->child[2]);
            analyzeStmt(node->child[4]);
        }
        else
        { 
            // IF LP Exp RP Stmt ELSE Stmt
            analyzeExp(node->child[2]);
            analyzeStmt(node->child[4]);
            analyzeStmt(node->child[6]);
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
        analyzeSpecifier(node->child[0]);
        analyzeDecList(node->child[1]);
    }

    /*
    DecList: 
      Dec
    | Dec COMMA DecList
    */
    void analyzeDecList(TreeNode *node)
    {
        analyzeDec(node->child[0]);
        if (node->child.size() == 3)
        { 
            // Dec COMMA DecList
            analyzeDecList(node->child[2]);
        }
    }

    /*
    Dec: 
      VarDec
    | VarDec ASSIGN Exp
    */
    void analyzeDec(TreeNode *node)
    {
        analyzeVarDec(node->child[0]);
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
    void analyzeExp(TreeNode *node)
    {
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
            // TODO ID INT FLOAT CHAR
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