#ifndef TREENODE_HPP
#define TREENODE_HPP

#include <vector>
#include <initializer_list>
#include <string>
#include <fstream>
#include <unistd.h>

FILE *output_file;
int has_error;

using std::endl;
using std::initializer_list;
using std::ofstream;
using std::string;
using std::vector;

enum DataType
{
    INT_TYPE,
    FLOAT_TYPE,
    CHAR_TYPE,
    ID_TYPE,
    TYPE_TYPE,
    CHILD,
    OTHER
};

struct TreeNode
{
    string name;
    DataType type;
    int pos;
    TreeNode *parent;
    string data;
    vector<TreeNode *> child;
};

struct TreeNode *root;

TreeNode *create_node(string name, int position=0, DataType type = DataType::OTHER, string val = "")
{
    TreeNode *node = new TreeNode;
    node->name = name;
    node->type = type;
    node->pos = position;
    node->data = val;
    return node;
}

TreeNode *create_child_node(string name, int position, initializer_list<TreeNode *> child)
{
    TreeNode *node = new TreeNode;
    node->name = name;
    node->type = DataType::CHILD;
    node->pos = position;
    for (auto c : child)
    {
        c->parent = node;
        node->child.push_back(c);
    }
    return node;
}

class Printer
{
private:
    TreeNode *root;
    ofstream out;

public:
    Printer(TreeNode *node, string path)
    {
        root = node;
        out = ofstream(path);
    }

    void print()
    {
        recursive_print(root, 0);
    }

    void recursive_print(TreeNode *node, int depth)
    {
        if (node->type == DataType::CHILD && node->child.size() == 0)
        {
            return;
        }
        for (int i = 0; i < depth; i++)
        {
            out << "  ";
        }
        if (node->type == DataType::TYPE_TYPE)
        {
            out << "TYPE: " << node->data << endl;
        }
        else if (node->type == DataType::CHAR_TYPE)
        {
            out << "CHAR: " << node->data << endl;
        }
        else if (node->type == DataType::INT_TYPE)
        {
            out << "INT: " << strtol(node->data.c_str(), NULL, 0) << endl;
        }
        else if (node->type == DataType::FLOAT_TYPE)
        {
            out << "FLOAT: " << node->data << endl;
        }
        else if (node->type == DataType::ID_TYPE)
        {
            out << "ID: " << node->data << endl;
        }
        else if (node->type == DataType::OTHER)
        {
            out << node->name << endl;
        }
        else
        {
            out << node->name << " (" << node->pos << ")" << endl;
            for (auto c : node->child)
            {
                recursive_print(c, depth + 1);
            }
        }
    }
};

void PrintTreeNode(char *file_path)
{
    string path = file_path;
    string out_path = "/dev/stdout";
    if (path.substr(path.length() - 4) == ".spl")
    {
        out_path = path.substr(0, path.length() - 4) + ".out";
    }
    Printer(root, out_path).print();
}

#endif