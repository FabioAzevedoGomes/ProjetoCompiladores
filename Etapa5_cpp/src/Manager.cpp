#include "Manager.h"

std::stack<SymbolTable *> Manager::stack;

// EXTERN FUNCTIONS

extern void exporta(void *arvore)
{
    if (arvore != NULL)
    {
        // Export connections between all nodes
        std::cout << ((Node *)arvore)->exportAllEdges();

        // Export node labels
        std::cout << ((Node *)arvore)->exportNodes();
    }
}

extern void libera(void *arvore)
{
    delete (Node *)arvore;
}

extern void export_code(void *arvore)
{
    std::cout << ((Node *)arvore)->exportCode() << std::endl;
}

// CONSTRUCTOR AND DESTRUCTOR

Manager::Manager()
{
    // Create a symbol table for the global scope
    SymbolTable *global_scope = new SymbolTable(0, true);

    // Insert in the stack base
    Manager::stack.push(global_scope);

    this->function = NULL;
    this->depth = 0;
}

Manager::~Manager()
{
    // Leave global scope
    Manager::leaveScope();
}

// SCOPE MANAGEMENT

void Manager::enterScope()
{
    // New symbol table
    SymbolTable *new_scope = NULL;

    // If entering function
    if (this->depth == 0)
    {
        // Create symbol table for new scope, where activation frame starts at 0
        new_scope = new SymbolTable(0);
    }
    // If not entering function
    else
    {
        // Create symbol table with last scope's address
        new_scope = new SymbolTable(Manager::stack.top()->getAddress());
    }

    // Add it to the stack
    Manager::stack.push(new_scope);

    // If entering a function
    if (this->depth == 0)
    {
        // Declare parameters
        for (auto i = this->vars.begin(); i != this->vars.end(); ++i)
        {
            this->declareSymbol(*i);
        }

        // Remove them from list
        vars.clear();
    }

    // Icrease depth
    this->depth++;
}

void Manager::leaveScope()
{
    if (!Manager::stack.empty())
    {
        // Top symbol table (Current scope)
        SymbolTable *top = Manager::stack.top();

        // Remove the symbol table from the top
        Manager::stack.pop();

        // If leaving a function
        if (this->depth == 1)
        {
            // Reser function pointer
            function = NULL;
        }
        // If leaving unnamed scope that is not global
        else if (this->depth > 0)
        {
            // Update previous scope's current address to leaving scope's address
            Manager::stack.top()->setAddress(top->getAddress());
        }

        // If leaving scope went ok
        if (top != NULL)
        {
            // Debug
            //std::cout << " = Leaving scope with symbol table = " << std::endl
            //          << top->toString() << std::endl;

            // Free it's memory
            delete (top);
        }

        // Lower current scope depth
        this->depth--;
    }
}

// SYMBOL TABLE MANAGEMENT

void Manager::declareSymbol(Symbol *symbol, bool globally)
{
    // Current scope's symbol table
    SymbolTable *current = Manager::stack.top();
    try
    {
        // If declaring to local symbol table
        if (!globally)
        {
            // Try to delcare symbol
            current->insertSymbol(symbol, symbol->getName());
        }
        // If declaring to global symbol table
        else
        {
            // If at global symbol table
            if (Manager::stack.size() == 1)
            {
                // Try to declare symbol
                current->insertSymbol(symbol, symbol->getName());
            }
            // If not, go deeper
            else
            {
                // Take off stack top (Saved in 'current')
                Manager::stack.pop();

                // 'Go down' a level
                declareSymbol(symbol, true);

                // Replace stack top on return
                Manager::stack.push(current);
            }
        }
    }
    catch (DeclaredException *e)
    {
        // If symbol existed, output message and exit
        std::cerr << e->what() << std::endl;
        exit(ERR_DECLARED);
    }
}

void Manager::declareVariables(Type type)
{
    // Iterate variable list
    for (auto i = vars.begin(); i != vars.end(); ++i)
    {
        // Update it's type
        (*i)->setType(type);

        // Declare it
        declareSymbol(*i);

        // Check if any nodes are associated with this symbol
        for (auto j = nodes.begin(); j != nodes.end(); ++j)
        {
            // Check if node is the same as the current variable being declared
            if (!strcmp((*j)->getChild(0)->getName(), (*i)->getName()))
            {
                // If so, check for type compatibility with initializer
                if (!isCompatible(type, (*j)->getChild(1)->getType()))
                {
                    std::cerr << WrongTypeException(*j).what() << std::endl;

                    // If incompatible, output error and exit
                    exit(ERR_WRONG_TYPE);
                }

                // Check if this is a string assignment and rval is a literal
                if (type == TYPE_STRING)
                {
                    // If initialized with a literal
                    if ((*j)->getChild(1)->getValue()->getCategory() == CAT_LITERAL)
                    {
                        // Update string size according to string length
                        (*i)->updateSize(strlen((*j)->getChild(1)->getValue()->getValue().name));
                    }
                    // If initialized with an identifier
                    else
                    {
                        // Get symbol for rval
                        Symbol *rval_symbol = getSymbol((*j)->getChild(1)->getValue());

                        // Update string size with rval size
                        (*i)->updateSize(rval_symbol->getOccupiedSize());
                    }
                }

                // Assign type to identifier node
                (*j)->getChild(0)->setType(type);

                // Assign type to initialization node
                (*j)->setType(type);
            }
        }
    }

    // Clear variable and node list
    vars.clear();
    nodes.clear();
}

void Manager::declareFunction(Node *function, Type type)
{
    // Create symbol
    Symbol *symbol = new Symbol(function->getValue(), NAT_FUNCTION, type, 1, true);

    // Update function return type
    symbol->setType(type);

    // Insert function in the symbol table
    declareSymbol(symbol);

    // Traverse parameter list
    for (auto i = vars.begin(); i != vars.end(); ++i)
        // Add parameter to function
        symbol->addParameter(*i);

    // Sets current function
    this->function = symbol;
}

Symbol *Manager::getSymbol(Token *lexval)
{
    SymbolTable *table = NULL;
    Symbol *symbol = NULL;

    try
    {
        // If stack is not empty
        if (!Manager::stack.empty())
        {
            // Try getting the symbol from the local scope
            symbol = Manager::stack.top()->getSymbol(lexval->getName());
        }
        else
        {
            if (lexval->getCategory() != CAT_LITERAL)
            {
                // Output error and exit
                std::cerr << UndeclaredException(lexval->getName()).what() << std::endl;
                exit(ERR_UNDECLARED);
            }
            else
            {
                symbol = NULL;
            }
        }
    }
    catch (UndeclaredException *e)
    {
        // Free exception memory
        delete e;

        // Get reference to top
        table = Manager::stack.top();

        // Pop top
        Manager::stack.pop();

        // Check on lower scopes
        symbol = Manager::getSymbol(lexval);

        // When returning, replace stack top
        Manager::stack.push(table);
    }

    // Return symbol found (or NULL)
    return symbol;
}

void Manager::addToVarList(Symbol *symbol, Node *node)
{
    this->vars.push_back(symbol);

    if (node != NULL)
        this->nodes.push_back(node);
}

// AST MANAGEMENT

// OPERAND NODES

Node *Manager::createDeclaration(Token *lexval, Type type, Statement statement)
{
    // Create and return node for declaration
    Node *declaration_node = new Node(lexval, type, statement);

    return declaration_node;
}

Node *Manager::createId(Token *lexval, Statement statement)
{
    // Check for id existance
    Symbol *id = Manager::getSymbol(lexval);

    // Create node
    Node *id_node = new Node(lexval, id->getType(), statement);

    // Generate intermediate code for this node
    id_node->generateCode();

    return id_node;
}

Node *Manager::createLiteral(Token *lexval, Type type)
{
    // Check if literal is already in the symbol table
    Symbol *lit = NULL;

    // Search for symbol
    if ((lit = Manager::getSymbol(lexval)) != NULL)
    {
        // Symbol already exists, update line only
        lit->setLine(lexval->getLine());
    }
    else
    {
        // If symbol does not exist, create it
        lit = new Symbol(lexval, NAT_NONE, type, type == TYPE_STRING ? 0 : 1, true);

        // And declare it in global scope
        Manager::declareSymbol(lit, true);
    }

    // Create node
    Node *lit_node = new Node(lexval, type, ST_OPERAND);

    // Generate intermediate code for this node
    lit_node->generateCode();

    return lit_node;
}

Node *Manager::createCommand(Category category, Value value, Type type, Statement statement, int line)
{
    // Create token for this node
    Token *tok = new Token(line, category, value, 0);

    // Create node
    Node *command_node = new Node(tok, type, statement);

    // Return node
    return command_node;
}

// INSTRUCTION NODES

Node *Manager::createIf(Node *condition, Node *then_node, Node *else_node)
{
    // Create value
    Value v;
    v.name = (char *)"if";

    // Create if node
    Node *if_node = createCommand(CAT_RESERVED, v, TYPE_ANY, ST_IF, condition->getLine());

    // Insert condition
    if_node->insertChild(condition);

    // Insert then
    if_node->insertChild(then_node);

    // Insert else
    if_node->insertChild(else_node);

    // Check if condition is being used correctly
    checkId(condition);

    // Check if types are compatible for the condition
    checkCompatibility(TYPE_BOOL, condition->getType(), if_node);

    return if_node;
}

Node *Manager::createFor(Node *init_attrib, Node *condition, Node *loop_attrib, Node *body)
{
    // Create value
    Value v;
    v.name = (char *)"for";

    // Create if node
    Node *for_node = createCommand(CAT_RESERVED, v, TYPE_ANY, ST_FOR, init_attrib->getLine());

    // Insert initial attribution as first child
    for_node->insertChild(init_attrib);

    // Insert stop condition as second child
    for_node->insertChild(condition);

    // Insert loop attribution as third child
    for_node->insertChild(loop_attrib);

    // Insert loop body as fourth child
    for_node->insertChild(body);

    // Check condition for correct usage
    checkId(condition);

    // Check if types are compatible for condition
    checkCompatibility(TYPE_BOOL, condition->getType(), for_node);

    return for_node;
}

Node *Manager::createWhile(Node *condition, Node *body)
{
    // Create value
    Value v;
    v.name = (char *)"while";

    // Create if node
    Node *while_node = createCommand(CAT_RESERVED, v, TYPE_ANY, ST_WHILE, condition->getLine());

    // Insert condition as first child
    while_node->insertChild(condition);

    // Insert loop body as second child
    while_node->insertChild(body);

    // Check if condition is used correctly
    checkId(condition);

    // Check if types are compatible for condition
    checkCompatibility(TYPE_BOOL, condition->getType(), while_node);

    return while_node;
}

Node *Manager::createVectorAccess(Node *id, Node *index)
{
    // Create value
    Value v;
    v.name = (char *)"[]";

    // Create vector access node
    Node *vec_access_node = createCommand(CAT_RESERVED, v, TYPE_ANY, ST_VECTOR_ACCESS, id->getLine());

    // Insert id as first child
    vec_access_node->insertChild(id);

    // Insert index as second child
    vec_access_node->insertChild(index);

    // Check id for correct usage
    checkId(vec_access_node);

    // Check for indexing expression type compatibility
    checkCompatibility(TYPE_INT, index->getType(), vec_access_node);

    // Create intermediate code for the node
    vec_access_node->generateCode();

    return vec_access_node;
}

Node *Manager::createInput(Node *id)
{
    // Create value
    Value v;
    v.name = (char *)"input";

    // Create input node
    Node *input_node = createCommand(CAT_RESERVED, v, TYPE_ANY, ST_IO, id->getLine());

    // Insert id as only child
    input_node->insertChild(id);

    // Check for correct id usage
    checkId(id);

    // Check if id has the correct type
    checkParamCompatibility(TYPE_INT, id->getType(), input_node);

    return input_node;
}

Node *Manager::createOutput(Node *out)
{
    // Create value
    Value v;
    v.name = (char *)"output";

    // Create output node
    Node *output_node = createCommand(CAT_RESERVED, v, TYPE_ANY, ST_IO, out->getLine());

    // Insert operand as only child
    output_node->insertChild(out);

    // Check for correct operand usage
    checkId(out);

    // Check if id has the correct type
    checkParamCompatibility(TYPE_INT, out->getType(), output_node);

    return output_node;
}

Node *Manager::createBreak(int line)
{
    // Create value
    Value v;
    v.name = (char *)"break";

    // Create break node
    Node *break_node = createCommand(CAT_RESERVED, v, TYPE_ANY, ST_BREAK_CONTINUE, line);

    return break_node;
}

Node *Manager::createContinue(int line)
{
    // Create value
    Value v;
    v.name = (char *)"continue";

    // Create continue node
    Node *continue_node = createCommand(CAT_RESERVED, v, TYPE_ANY, ST_BREAK_CONTINUE, line);

    return continue_node;
}

Node *Manager::createReturn(Node *retval)
{
    // Create value
    Value v;
    v.name = (char *)"return";

    // Create return node
    Node *return_node = createCommand(CAT_RESERVED, v, retval->getType(), ST_RETURN, retval->getLine());

    // Insert retval expression as only child
    return_node->insertChild(retval);

    // Check for correct retval usage
    checkId(retval);

    // Check if id has the correct type
    checkParamCompatibility(function->getType(), retval->getType(), return_node);

    return return_node;
}

Node *Manager::createShift(Node *id, Node *op, Node *amount)
{
    WrongShiftAmountException *e = NULL;

    // Insert identifier as first child
    op->insertChild(id);

    // Insert shift amount as second child
    op->insertChild(amount);

    // Update type
    op->setType(id->getType());

    // Check for correct id usage
    checkId(id);

    // Check if shift amout is correct
    if (amount->getValue()->getValue().integer > 16)
    {
        e = new WrongShiftAmountException(op);

        // Create error
        std::cerr << e->what() << std::endl;

        // Exit
        exit(ERR_WRONG_PAR_SHIFT);
    }

    return op;
}

Node *Manager::createAttribution(Node *lval, Node *op, Node *rval)
{
    InvalidStringSizeException *e = NULL;

    // Insert lval as first child
    op->insertChild(lval);

    // Insert rval as second child
    op->insertChild(rval);

    // Set operation type
    op->setType(lval->getType());

    // Check for correct id usage on both sides
    checkId(lval);
    checkId(rval);

    // Check if types are compatible
    if (!isCompatible(lval->getType(), rval->getType()))
    {
        std::cerr << "On line: " << std::to_string(op->getLine()) << " incompatible types in assignment\n"
                  << op->reconstruct() << "\n"
                  << "Lval has type " << getTypeName(lval->getType()) << " but rval has type " << getTypeName(rval->getType()) << std::endl;

        // Exit
        exit(ERR_WRONG_TYPE);
    }

    // Check if this is a string concatenation
    if (lval->getType() == TYPE_STRING)
    {
        // Get lval symbol reference
        Symbol *lval_symbol = getSymbol(lval->getValue());

        // Calculate rval resulting string size
        int resulting_size = getStringSize(rval);

        // Check if size is fine with lval
        if (!lval_symbol->updateSize(resulting_size))
        {
            // Create exception
            e = new InvalidStringSizeException(op, lval_symbol, resulting_size);

            // Output error
            std::cerr << e->what() << std::endl;

            //Exit
            exit(ERR_STRING_SIZE);
        }
    }

    // Create intermediate code for this node
    op->generateCode();

    return op;
}

Node *Manager::createInitialization(Node *lval, Node *op, Node *rval)
{
    // Insert identifier as first child
    op->insertChild(lval);

    // Insert initializer as second child
    op->insertChild(rval);

    // Check for rval correct usage
    checkId(rval);

    return op;
}

Node *Manager::createUnop(Node *operation, Node *operand)
{
    // Insert operand as first child
    operation->insertChild(operand);

    // Check for correct id usage
    checkId(operand);

    // Check for compatible types
    checkCompatibility(operation->getType(), operand->getType(), operation);

    // Update type
    operation->setType(inferType(operation->getType(), operand->getType()));

    return operation;
}

Node *Manager::createBinop(Node *l_operand, Node *operation, Node *r_operand)
{
    // Insert left operand as first child
    operation->insertChild(l_operand);

    // Insert right operand as second child
    operation->insertChild(r_operand);

    // Check for correct id usage
    checkId(l_operand);
    checkId(r_operand);

    // If creating a ternary operation
    if (operation->getKind() == ST_TERNOP)
    {
        // Check if condition is compatible with BOOL
        checkCompatibility(TYPE_BOOL, l_operand->getType(), operation);

        // Check if expression types are compatible
        checkCompatibility(operation->getChild(0)->getType(), r_operand->getType(), operation);

        // Update type
        operation->setType(inferType(operation->getChild(0)->getType(), r_operand->getType()));
    }
    else
    {
        // Check if lval type is compatible with rval type
        checkCompatibility(l_operand->getType(), r_operand->getType(), operation);

        // Check if those types are compatible with operation type
        checkCompatibility(inferType(l_operand->getType(), r_operand->getType()), operation->getType(), operation);

        // Upate type
        operation->setType(inferType(l_operand->getType(), r_operand->getType()));
    }

    // Generate intermediate code for this node
    operation->generateCode();

    return operation;
}

Node *Manager::createTernop(Node *then_node)
{
    // Create value
    Value v;
    v.name = (char *)"?:";

    // Create ternop node
    Node *ternop_node = createCommand(CAT_RESERVED, v, TYPE_ANY, ST_TERNOP, then_node->getLine());

    // Add then node as first child
    ternop_node->insertChild(then_node);

    // Check for correct id usage
    checkId(then_node);

    return ternop_node;
}

Node *Manager::createFunctionDeclaration(Node *id, Node *body)
{
    // Insert function body as only child
    id->insertChild(body);

    // Return function node
    return id;
}

Node *Manager::createFunctionCall(Node *id, Node *args)
{
    Symbol *func_symbol = NULL; // Function symbol

    // Insert arguments as function call child
    id->insertChild(args);

    // Update node statement kind to "function call"
    id->setKind(ST_FUNCTION_CALL);

    // Check for correct id usage
    checkId(id);

    // Get function id from symbol table
    func_symbol = getSymbol(id->getValue());

    // Update the node's type
    id->setType(func_symbol->getType());

    // Check if function arguments match parameters
    checkArguments(id);

    // Return function call node
    return id;
}

// TYPE AND IDENTIFIER CHECKS

void Manager::checkId(Node *id_node, Nature usage)
{
    WrongUsageException *e = NULL;
    Symbol *symbol = NULL;

    // Based on node statement
    switch (id_node->getKind())
    {
    case ST_OPERAND:
        // If node is an ID
        if (id_node->getValue()->getCategory() == CAT_IDENTIFIER)
        {
            // Search for id
            symbol = getSymbol(id_node->getValue());

            if (symbol->getNature() != NAT_IDENTIFIER)
            {
                e = new WrongUsageException(symbol, id_node);

                std::cerr << e->what() << std::endl;

                exit(e->getCode());
            }
        }

        break;
    case ST_FUNCTION_CALL:

        // If node is an ID
        if (id_node->getValue()->getCategory() == CAT_IDENTIFIER)
        {
            // Search for function
            symbol = getSymbol(id_node->getValue());

            if (symbol->getNature() != NAT_FUNCTION)
            {
                e = new WrongUsageException(symbol, id_node);

                std::cerr << e->what() << std::endl;

                exit(e->getCode());
            }
        }

        break;
    case ST_VECTOR_ACCESS:

        // If first child is an ID
        if (id_node->getChild(0)->getValue()->getCategory() == CAT_IDENTIFIER)
        {
            // Search for vector
            symbol = getSymbol(id_node->getChild(0)->getValue());

            if (symbol->getNature() != NAT_VECTOR)
            {
                e = new WrongUsageException(symbol, id_node);

                std::cerr << e->what() << std::endl;

                exit(e->getCode());
            }
        }

        break;
    default:
        break;
    }
}

void Manager::checkCompatibility(Type type1, Type type2, Node *node)
{
    InvalidConversionException *e = NULL;

    // Check if types are compatible for indexing expression
    if (!isCompatible(type1, type2))
    {
        e = new InvalidConversionException(node, type1, type2);

        std::cerr << e->what() << std::endl;

        exit(e->getCode());
    }
}

void Manager::checkParamCompatibility(Type expected, Type given, Node *node)
{
    WrongParameterException *e = NULL;

    if (!isCompatible(expected, given))
    {
        e = new WrongParameterException(node, expected, given, function);

        std::cerr << e->what() << std::endl;

        exit(e->getCode());
    }
}

void Manager::checkArguments(Node *func)
{
    WrongArgumentsException *e = NULL; // Possible generated excepetion
    uint arg_count = 0;                // Passed argument count
    uint par_at = 0;                   // Current parameter being checked
    Symbol *symbol = NULL;             // Function symbol

    // Get symbol reference for that function
    symbol = getSymbol(func->getValue());

    // Get parameter list
    std::vector<Symbol *> params = symbol->getParams();

    // Count arguments
    for (Node *aux = func->getChild(0); aux != NULL; aux = aux->getNextElement())
        arg_count++;

    // Check for correct count
    if (arg_count > params.size())
    {
        // Create exception
        e = new WrongArgumentsException(func, symbol, ERR_EXCESS_ARGS);

        // Output error and exit
        std::cerr << e->what() << std::endl;

        exit(ERR_EXCESS_ARGS);
    }
    else if (arg_count < params.size())
    {
        // Create exception
        e = new WrongArgumentsException(func, symbol, ERR_MISSING_ARGS);

        // Output error and exit
        std::cerr << ((WrongArgumentsException *)e)->what() << std::endl;

        exit(ERR_MISSING_ARGS);
    }

    // Iterate argument list
    for (Node *i = func->getChild(0); i != NULL; i = i->getNextElement(), par_at++)
    {
        // Check correct type for each argument and parameter
        if (!isCompatible(i->getType(), (params.at(par_at))->getType()))
        {
            // Create exception
            e = new WrongArgumentsException(func, symbol, ERR_WRONG_TYPE_ARGS, par_at);

            // Output error and exit
            std::cerr << e->what() << std::endl;

            exit(ERR_WRONG_TYPE_ARGS);
        }
    }
}

int Manager::getStringSize(Node *node)
{
    int size = 0;
    Symbol *symbol = NULL;

    // If this is a leaf
    if (node->getKind() == ST_OPERAND)
    {
        // If this is a literal
        if (node->getValue()->getCategory() == CAT_LITERAL)
        {
            // Return own size
            size = strlen(node->getValue()->getValue().name);
        }
        // If this is an identifier
        else
        {
            // Search symbol table
            symbol = getSymbol(node->getValue());

            // Return symbol size
            size = symbol->getOccupiedSize();
        }
    }
    else
    {
        // Return sum of children
        size = getStringSize(node->getChild(0)) + getStringSize(node->getChild(1));
    }

    // Return calculated size
    return size;
}
