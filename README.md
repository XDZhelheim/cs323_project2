# CS323 Project 2 Report

11812804 董正

11813225 王宇辰

## 1 Introduction

### 1.1 Project Requirements

This project is to implement a semantic analyzer to analyze SPL programs. Report if there is any error; output nothing if no semantic error in the program.

* Print out error type and line number
* Clarify and recognize 15 types of error
* Perform semantic analysis in one-pass or separate these two stages (build the tree and then traverse it to analyze semantics) for better modularity and flexibility.

We did not implement the bonus part, i.e. there is only the **global scope** and we used **name equivalence**.

### 1.2 Development and Test Environment

* Linux

  * `Ubuntu 18.04.6 LTS x86_64` with `Linux 5.4.0-87-generic`

  * `gcc (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0`

  * `g++ (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0`

  * `flex 2.6.4`

  * `bison (GNU Bison) 3.0.4`

  * `GNU Make 4.1`

* macOS (Arm)

  * `macOS 12.0.1 21A559 arm64` with `Darwin Kernel Version 21.1.0`
  * `gcc-11 (Homebrew GCC 11.2.0_1) 11.1.0`
  * `g++-11 (Homebrew GCC 11.2.0_1) 11.1.0`
  * `flex 2.6.4`
  * `bison (GNU Bison) 3.8.2`
  * `GNU Make 3.81`

## 2 Design and Implementation

### 2.1 Type class

In `Type.hpp`, we defined a class to represent the type of each variable. The fields are:

* `string name`

  The name of the variable

* `Category category`

  The data type of the variable or data (e.g. `INT_VAL, FUNCTION, ARRAY`)

* `Array *array`

  Store array data for array type variables.

  ```c++
  class Array
  {
  public:
      Type *type;
      int size;
  
      Array(Type *_type, int _size);
  };
  ```

* `vector<Type *> varlist`

  Store variable that belongs to the variable (e.g. parameters of a function, variable defined in a struct, ...)

* `Type *structType`

  Store the struct information, typically struct name, of a struct variable.

* `Type *returnType`

  Store the return type of the function. (e.g. for `int f()`, `f`'s type is function, and `f`'s return type is int).

This is the core part of our program, since almost all information is stored in the instance of this class. Therefore, it is essential to design it properly.

### 2.2 Parse Tree Analyzer

The idea of our project is to build the tree (project 1's code) and then traverse it to analyze semantics. Therefore, we designed a class with several functions to do the tree traversal and analyze semantic errors.

For symbol table, we just defined it globally: `map<string, Type *> symbolTable;`

Then, from root node, start top-down analysis. To achieve this, we created functions for each productions. 

And we will introduce some important functions in the following:

* `void analyzeExtDef(TreeNode *node)`

  Almost everything starts with this function. In this function, we handle struct definitions, function definitions and many other expressions.

* `Type *analyzeSpecifier(TreeNode *node)`

  In this function, we determine the type of each variable. The construction of symbol table also relies on this.

* `Type *analyzeStructSpecifier(TreeNode *node)`

  In this function, we find out the name of the struct and add it to the symbol table, and check error type 15. After that, start analyze the contents of the struct.

* `Type *analyzeVarDec(TreeNode *node, Type *specifier)`

  This is one of the core functions of the analyzer. Every variable definitions will go through this function. We add new elements to symbol table and handle type 3 error here. What's more, the analysis of arrays and high-dim arrays is also coded in this function.

* `void analyzeStmt(TreeNode *node, Type *specifier)`

  We handle all statements here, such as if, while, and return. So the logic of this function is important.

* `Type *analyzeExp(TreeNode *node)`

  Longest function in the whole project. Exp has lots of productions, and they are the foundation of the parse tree, therefore, if the logic of any is wrong, the whole program will be broken. And naturally most of the bugs are in this function.

## 3 Conclusion

In this project, we implemented an analyzer of parse tree to analyze semantic errors. The most difficulity we met is how to design and use `Type` class. We encountered many problems such as pass-by-pointers, handling symbol table, error recovery and so on. In conclusion, the most important thing is to keep a clear mind, understand the inner logic, especially `Exp`. Through this project, we got a deeper comprehension of grammar, and learnt basic methods to analyze it.
