#ifndef TYPE_HPP
#define TYPE_HPP

enum Category
{
    PRIMITIVE,
    ARRAY,
    STRUCTURE
};

enum Primitive
{
    INT,
    FLOAT,
    CHAR
};

class Type
{
public:
    string name;
    Category category;
    union
    {
        Primitive primitive;
        Array *array;
        vector<FieldList> structure;
    };
};

class Array
{
public:
    Type *base;
    int size;

    Array(Type *_base, int _size)
    {
        base = _base;
        size = _size;
    }
};

class FieldList
{
public:
    string name;
    Type *type;

    FieldList(string _name, Type *_type)
    {
        name = _name;
        type = _type;
    }
};

#endif