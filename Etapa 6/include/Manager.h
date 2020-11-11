/**
 * Envinronment Manager for the compiler as a whole for syntatic and semantic checks
 * This file contains methods for managing the current static scope for the program,
 * methods for manipulating the symbol tables associated with each scope and methods
 * for creating the AST used for the program composition, as well as methods that 
 * generate intermediate code according to ILOC standards.
 * 
 * Author: Fábio de Azevedo Gomes
 */
#ifndef MANAGER_H
#define MANAGER_H

// Forward declaration of domain classes
class Token;
class Node;
class Symbol;
class SymbolTable;
class Tac;
class ASM;

#include "Node.h"
#include "SymbolTable.h"
#include "errors.h"
#include "Tac.h"
#include "ASM.h"

#include <stack>
#include <list>
#include <iostream>
#include <cstring>

// PROVIDED EXTERN FUNCTIONS FOR MAIN.C
extern void libera(void *arvore);
extern void exporta(void *arvore);
extern void export_code(void *arvore);
extern void generate_ASM(void *arvore);

class Manager
{

private:
    static std::stack<SymbolTable *> stack; // The Stack used for maintaining scope-specific symbol tables

    static Symbol *function; // The function representing the current scope
    int depth;               // Scope "depth", 0  when inside global scope

    static Node *main_node;     // Pointer to Node containing main function code
    static Symbol *main_symbol; // Pointer to Symbol containing main function information

    std::vector<Symbol *> vars; // Variables being saved for bulk delcaration
    std::vector<Node *> nodes;  // Nodes being saved for bulk typing

public:
    // CONSTRUCTOR AND DESTRUCTOR

    /**
     * @brief Initializes the symbol table for the global scope
     */
    Manager();

    /**
     * @brief Class destructor 
     */
    ~Manager();

    // SCOPE MANAGEMENT

    /**
     * @brief Enters a new scope
     */
    void enterScope();

    /**
     * @brief Leaves the current scope
     */
    void leaveScope();

    /**
     * @brief Leaves a function scope, finally removing 
     * it's symbol table from the stack
     */
    void leaveFunction();

    // CODE MANAGEMENT

    /**
     * @brief Inserts the initial loading code in the root node of the AST 
     * @param root Root of the AST
     */
    static void addDriverCode(void *root);

    /**
     * @brief Updates the main function references to the given ones
     * @param main_node   Pointer to the main function node
     * @param main_symbol Pointer to the main function symbol
     */
    static void updateMain(Node *main_node, Symbol *main_symbol);

    /**
     * @brief Returns the main function symbol 
     */
    static Symbol *getMain();

    /**
     * @brief Returns the current function symbol 
     */
    static Symbol *getCurrentFunction();

    // SYMBOL TABLE MANAGEMENT

    /**
     * @brief Returns the currently active symbol table
     */
    static SymbolTable *getActiveSymbolTable();

    /**
     * @brief Declares a symbol with the given type
     * @param symbol Symbol being declared
     * @param globally Whether the symbol should be declared to the global symbol table (No by default)
     */
    void declareSymbol(Symbol *symbol, bool globally = false);

    /**
     * @brief Declares the current list of symbols (vars) in the current
     * scope's symbol table. If there are nodes related to these symbols,
     * they are also typed and type checked accordingly
     * @param type Type being given to these variables
     */
    void declareVariables(Type type);

    /**
     * @brief Declares the given function with the given type. The current
     * variable list is used as the function's parameters
     * @param symbol Function node
     * @param type   Function return type
     */
    void declareFunction(Node *symbol, Type type);

    /**
     * @brief Searches for the given symbol in the current and higher scopes
     * @param lexval Symbol Lexical value
     * @returns Reference to the symbol 
     */
    static Symbol *getSymbol(Token *lexval);

    /**
     * @brief Adds a symbol to the variables list, and a node to the nodes
     * list if this is an initialization
     */
    void addToVarList(Symbol *symbol, Node *node = NULL);

    // AST MANAGEMENT

    // OPERAND NODES

    /**
     * @brief Creates an identifier declaration node
     * @param lexval    Identifier token
     * @param type      Identifier type
     * @param statement Identifier statement types
     * @returns Pointer to the node
     */
    Node *createDeclaration(Token *lexval, Type type, Statement statement);

    /**
     * @brief Creates a node for an identifier
     * @param lexval    Identifier token
     * @param statement How it's being used
     * @param lval If this id is an lval or not, true by default
     * @returns Pointer to the node
     */
    Node *createId(Token *lexval, Statement statement, bool lval = true);

    /**
     * @brief Creates a node for a literal value
     * @param lexval Literal token 
     * @param type   Value type
     * @returns Pointer to the node
     */
    Node *createLiteral(Token *lexval, Type type);

    /**
     * @brief Creates a command node from data
     * @param category Token category
     * @param value    Token value
     * @param type     Node type
     * @param statment Statement kind
     * @param line     Line where the token occurs
     * @returns Pointer to the node
     */
    Node *createCommand(Category category, Value value, Type type, Statement statement, int line);

    // INSTRUCTION NODES

    /**
     * @brief Creates a node for an IF statement
     * @param condition Node containing the condition expression
     * @param then_node Node containing the executed statement if expression is true
     * @param else_node Node containing the executed statement if expression is false
     * @returns Pointer to the node
     */
    Node *createIf(Node *condition, Node *then_node, Node *else_node = NULL);

    /**
     * @brief Creates a node for a FOR Statement
     * @param init_attrib Node containing the initial attribution
     * @param condition   Node containing loop stop condition
     * @param loop_attrib Node containing attribution executed every iteration
     * @param body        Node containing the loop body
     * @returns Pointer to the node
     */
    Node *createFor(Node *init_attrib, Node *condition, Node *loop_attrib, Node *body);

    /**
     * @brief Creates a node for a WHILE statement
     * @param condition Node containing the loop stop condition
     * @param body      Node containing the loop body
     * @returns Pointer to the node
     */
    Node *createWhile(Node *condition, Node *body);

    /**
     * @brief Creates a node for a VECTOR ACCESS operation
     * @param id    Node containing the id being indexed
     * @param index Node containing expression used to index the vector
     * @returns Pointer to the node
     */
    Node *createVectorAccess(Node *id, Node *index);

    /**
     * @brief Creates a node for an INPUT statement
     * @param id Node containing the id being input to
     * @returns Pointer to the node
     */
    Node *createInput(Node *id);

    /**
     * @brief Creates a node for an OUTPUT statement
     * @param out Node containing the id or literal being output 
     * @returns Pointer to the node
     */
    Node *createOutput(Node *out);

    /**
     * @brief Creates a node for a BREAK statement
     * @param line Line where the statement is present
     * @returns Pointer to the node
     */
    Node *createBreak(int line);

    /**
     * @brief Creates a node for a CONTINUE statement 
     * @param line Line where the statement is present
     * @returns Pointer to the node
     */
    Node *createContinue(int line);

    /**
     * @brief Creates a node for a RETURN statement
     * @param retval Node containing the returned expression
     * @returns Pointer to the node
     */
    Node *createReturn(Node *retval);

    /**
     * @brief Creates a node for a SHIFT statement
     * @param id     Node containing the identifier being shifted
     * @param op     Node containig the shift operation (left or right)
     * @param amount Node containing the shift amount
     * @returns Pointer to the node 
     */
    Node *createShift(Node *id, Node *op, Node *amount);

    /**
     * @brief Creates a node for an ATTRIBUTION statement
     * @param lval Node containing the left-hand expression of the attribution
     * @param op   Node containing the attribution operation
     * @param rval Node containing the right-hand expression of the attirbution
     * @returns Pointer to the node
     */
    Node *createAttribution(Node *lval, Node *op, Node *rval);

    /**
     * @brief Creates a node for a VARIABLE INITIALIZATION statement.
     * Does not perform any checks for type correctness, as type
     * has not yet been assigned
     * @param lval Node containing the left-hand expression of the initialization
     * @param op   Node containing the initialization operation
     * @param rval Node containing the right-hand expression of the initialization
     * @returns Pointer to the node
     */
    Node *createInitialization(Node *lval, Node *op, Node *rval);

    /**
     * @brief Creates a node for a UNARY OPERATION expression
     * @param operation Node containing the unary operator
     * @param operand   Node containing the operand expression
     * @returns Pointer to the node
     */
    Node *createUnop(Node *operation, Node *operand);

    /**
     * @brief Creates a node for a BINARY OPERATION expression
     * @param l_operand Node containing the left operand
     * @param operation Node containing the binary operator
     * @param r_operand Node containing the right operand
     * @returns Pointer to the node
     */
    Node *createBinop(Node *l_operand, Node *operation, Node *r_operand);

    /**
     * @brief Partially creates a node for a TERNARY OPERATION expression
     * @param then_node Node containing expression executade if condition is true
     * @returns Pointer to the partially created node 
     */
    Node *createTernop(Node *then_node);

    /**
     * @brief Creates a node for a FUNCTION DECLARATION statement
     * @param id   Node containing the function identifier
     * @param body Node containing the first command of the function body
     * @returns Pointer to the node
     */
    Node *createFunctionDeclaration(Node *id, Node *body);

    /**
     * @brief Creates a node for a FUNCTION CALL statement
     * @param id   Node containing the function identifier
     * @param args Node containing a list of arguments passed to function call
     */
    Node *createFunctionCall(Node *id, Node *args);

private:
    /**
     * @brief Checks if an identifier node is being used correctly
     * @param id_node Node being checked
     * @param usage How it's being used (variable by default)
     */
    void checkId(Node *id_node, Nature usage = NAT_IDENTIFIER);

    /**
     * @brief Checks compatibility between types, and generates an error if
     * they are not compatible 
     * @param type1 First type
     * @param type2 Second type
     * @param node  Node where they are being used (for the error report)
     */
    void checkCompatibility(Type type1, Type type2, Node *node);

    /**
     * @brief Check compatibility between types for parameters, and generates an
     * error if they are not compatible
     * @param expected Expected type
     * @param given   Given type
     * @param node     Node where this occurs (for error report)
     */
    void checkParamCompatibility(Type expected, Type given, Node *node);

    /**
     * @brief Checks if a given list of arguments matches up with
     * the given function definition
     * @param func Function call node
     */
    void checkArguments(Node *func);

    /**
     * @brief Returns the size of the string being created by this node,
     * if it is a string concatenation operation
     * @param node The node creating the string
     */
    int getStringSize(Node *node);
};

#endif
