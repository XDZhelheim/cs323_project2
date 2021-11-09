#ifndef TYPE_HPP
#define TYPE_HPP

#include <string>
#include <vector>
#include "ErrorPrinter.hpp"

using std::string;
using std::vector;

enum Category
{
    INT_VAL,
    FLOAT_VAL,
    CHAR_VAL,
    INT_VAR,
    FLOAT_VAR,
    CHAR_VAR,
    ARRAY,
    STRUCTURE,
    FUNCTION
};

class Array;

class Type
{
public:
    string name;
    Category category;

    string primitive_value;

    Array *array;

    vector<Type> varlist;

    Type *getChild(int lineno);

    string getSigniture();
};

class Array
{
public:
    Type type;
    int size;

    Array(Type _type, int _size);
};

Type *Type::getChild(int lineno)
{

    if (category == Category::ARRAY)
    {
        return &array->type;
    }
    else
    {
        print_type_10(lineno);
        return nullptr;
    }
}

string Type::getSigniture()
{
    string base = "";
    Type t = *this;
    switch (category)
    {
    case Category::INT_VAL:
        return "int";

    case Category::FLOAT_VAL:
        return "float";

    case Category::CHAR_VAL:
        return "char";

    case Category::ARRAY:
        while (t.category == Category::ARRAY)
        {
            t = *t.getChild(-1);
            base += "*";
        }

        return t.getSigniture() + base;

    case Category::STRUCTURE:
        return "struct " + name;

    case Category::FUNCTION:
        return "Func " + name;

    default:
        break;
    }
}

Array::Array(Type _type, int _size)
{
    type = _type;
    size = _size;
}

#endif