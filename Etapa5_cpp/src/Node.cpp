#include "Node.h"

Node::Node(Token *lexval_, Type type_, Statement statement_)
{
    this->lexval = lexval_;
    this->type = type_;
    this->statement = statement_;
    this->next_cmd = NULL;
    this->next_elem = NULL;
    this->code = NULL;
    this->address = false;

    // Initialize empty vector
    this->children.resize(0);
}

Node::~Node()
{
    // Traverse children deleting them
    for (auto i = this->children.begin(); i != this->children.end(); ++i)
        delete (*i);

    // Delete next element after this node, if it exists
    if (this->next_elem != NULL)
        delete (this->next_elem);

    // Delete next command after this node, if it exists
    if (this->next_cmd != NULL)
        delete (this->next_cmd);

    // Delete code for this command, if it exists
    if (this->code != NULL)
        delete (this->code);

    // Delete lexical value
    delete (this->lexval);
}

std::string Node::toString()
{
    std::stringstream identification;

    // Create string
    identification << this << ", [label=\"";

    // If this is a function call, add 'call' before name
    if (this->statement == ST_FUNCTION_CALL)
        identification << "call ";

    // Close identification
    identification << this->lexval->getName() << "\"];";

    // Return
    return identification.str();
}

std::string Node::reconstruct()
{
    std::stringstream node_string;

    // Based on node nature
    switch (this->getKind())
    {
    case ST_OPERAND: // IDs and literals
        node_string << this->getName();
        break;
    case ST_VECTOR_ACCESS: // Vector access nodes
        node_string << this->getChild(0)->getName()
                    << "[" << this->getChild(1)->reconstruct() << "]";
        break;
    case ST_UNOP:   // Unops
    case ST_IO:     // IOs
    case ST_RETURN: // Returns
        node_string << this->getName() << " "
                    << this->getChild(0)->reconstruct();
        break;
    case ST_BINOP:           // Binops
    case ST_INIT_VARIABLE:   // Variable initialization
    case ST_ATTRIB_VARIABLE: // Variable attributions
        node_string << this->getChild(0)->reconstruct()
                    << " " << this->getName() << " "
                    << this->getChild(1)->reconstruct();
        break;
    case ST_TERNOP: // Ternary operations
        node_string << this->getChild(1)->reconstruct() << " ? "
                    << this->getChild(0)->reconstruct() << " : "
                    << this->getChild(2)->reconstruct();
        break;
    case ST_FUNCTION_CALL: // Function calls
        node_string << this->getName()
                    << "( " << this->getChild(0)->reconstruct() << " )";
        break;
    case ST_IF: // If statements
        node_string << this->getName()
                    << "( " << this->getChild(0)->reconstruct() << " ) ...";
        break;
    case ST_FOR: // For statements
        node_string << this->getName()
                    << "( " << this->getChild(0)->reconstruct() << " : "
                    << this->getChild(1)->reconstruct() << " : "
                    << this->getChild(2)->reconstruct() << " ) ..";
        break;
    case ST_WHILE: // While statement
        node_string << this->getName()
                    << "( " << this->getChild(0)->reconstruct() << " ) ...";
        break;
    default:
        break;
    }

    // If node is part of a list
    if (this->next_elem != NULL)
        // Reconstruct next node, and add a ',' in between
        node_string << ", " << this->next_elem->reconstruct();

    // Return generated string
    return node_string.str();
}

std::string Node::exportNodes()
{
    std::stringstream output;

    // Export information about this node
    output << this->toString() << std::endl;

    // Export information about this node's children, from left to right
    for (auto i = this->children.begin(); i != this->children.end(); ++i)
        output << (*i)->exportNodes();

    // Export information abou this node's next in the list, if it exists
    if (this->next_elem != NULL)
        output << this->next_elem->exportNodes();

    // Export information about this node's next command, if it exists
    if (this->next_cmd != NULL)
        output << this->next_cmd->exportNodes();

    // Return generated string
    return output.str();
}

std::string Node::exportEdges()
{
    std::stringstream output;

    // Export children's edges
    for (auto i = this->children.begin(); i != this->children.end(); ++i)
        output << this << ", " << *i << std::endl;

    // Export next element's edges, if it exists
    if (this->next_elem != NULL)
        output << this << ", " << this->next_elem << std::endl;

    // If there is a next command, export is as well
    if (this->next_cmd != NULL)
        output << this << ", " << this->next_cmd << std::endl;

    // Return generated string
    return output.str();
}

std::string Node::exportAllEdges()
{
    std::stringstream output;

    // Export this node's edges
    output << this->exportEdges();

    // Export this node's children's edges
    for (auto i = this->children.begin(); i != this->children.end(); ++i)
        output << (*i)->exportAllEdges();

    // Export next element's edges, if it exists
    if (this->next_elem != NULL)
        output << this->next_elem->exportAllEdges();

    // Export next command's edges, if it exists
    if (this->next_cmd != NULL)
        output << this->next_cmd->exportAllEdges();

    // Return generated string
    return output.str();
}

std::string Node::exportCode()
{
    std::string code = "";

    // Export the code if there is any associated
    if (this->code != NULL)
        code = this->code->getCodeString();

    return code;
}

// SETTERS

void Node::insertChild(Node *child)
{
    // Insert child if it exists
    if (child != NULL)
        this->children.push_back(child);
}

void Node::insertCommand(Node *next_cmd)
{
    // Update next command
    this->next_cmd = next_cmd;
}

void Node::setType(Type type)
{
    this->type = type;
}

void Node::setKind(Statement statement)
{
    this->statement = statement;
}

void Node::insertNext(Node *next)
{
    this->next_elem = next;
}

void Node::setTemp(std::string temp)
{
    this->temp = temp;
}

void Node::generateCode()
{
    // DEBUG
    //std::cout << "Generating code for " << std::endl;
    //std::cout << this->getInfo() << std::endl;

    switch (this->statement)
    {
    case ST_OPERAND:
        this->code = generateLvalVariableTAC();
        break;
    case ST_VECTOR_ACCESS:
        this->code = generateLvalVectorTAC();
        break;
    case ST_ATTRIB_VARIABLE:
        this->code = generateAttributionTAC();
        break;
    case ST_BINOP:
        this->code = generateBinopTAC(binop_code.at(this->getName()));
        break;
    case ST_TERNOP:
        this->code = generateTernopTAC();
        break;
    default:
        break;
    }

    // Debug
    //if (this->code != NULL)
    //{
    //    std::cout << this->code->getCodeString() << std::endl;
    //}
}

void Node::setAddress()
{
    this->address = true;
}

// GETTERS

const char *Node::getName()
{
    return this->lexval->getName();
}

Type Node::getType()
{
    return this->type;
}

int Node::getLine()
{
    return this->lexval->getLine();
}

Token *Node::getValue()
{
    return this->lexval;
}

Statement Node::getKind()
{
    return this->statement;
}

Node *Node::getChild(int index)
{
    Node *child = NULL;

    if (this->children.size() > 0)
    {
        try
        {
            // Get child
            child = this->children.at(index);
        }
        catch (const std::out_of_range &e)
        {
            // If out of range, return last child
            child = this->children.at(this->children.size() - 1);
        }
    }
    return child;
}

Node *Node::getNextCommand()
{
    return this->next_cmd;
}

Node *Node::getNextElement()
{
    return this->next_elem;
}

std::string Node::getInfo()
{
    std::stringstream info;

    info << "+ Node name: " << this->getName() << std::endl;
    info << "| Type: " << getTypeName(this->getType()) << std::endl;
    info << "| Statement: " << this->getKind() << std::endl;
    info << "| Line: " << this->getLine() << std::endl;
    info << "| Child count: " << this->children.size() << std::endl;
    info << "| Category: " << this->getValue()->getCategory() << std::endl;

    return info.str();
}

std::string Node::getTemp()
{
    return this->temp;
}

Tac *Node::getCode()
{
    // Copy code
    Tac *copy = new Tac(*(this->code));

    // Return copy of the code
    return copy;
}

bool Node::hasAddress()
{
    return this->address;
}

bool Node::hasValue()
{
    return !this->address;
}

// TAC GENERATION

Tac *Node::generateLvalVariableTAC()
{
    // Instructions
    Tac *calc_base = NULL; // 1. t1 = rfp + base OR 1. t1 = rbss + base

    // Temporary register
    std::string t1 = Tac::newRegister();

    // Get variable base address from symbol table
    Symbol *var = Manager::getSymbol(this->getValue());
    std::string base = std::to_string(var->getAddress());

    // Generate code
    calc_base = new Tac(ILOC_LOADAI, var->isGlobal() ? "rbss" : "rfp", base, t1); // loadAI rpf, base => t1

    // Set this node's temporary register to the accessed variable address
    this->setTemp(t1);

    // Mark this node as having an address
    this->setAddress();

    // Return instruction
    return calc_base;
}

Tac *Node::generateLvalVectorTAC()
{
    // Instructions
    Tac *index_code = NULL;   // 1. i = ... calculate index ...
    Tac *intermed_1 = NULL;   //    1.1 If i has an address, i = Mem(i)
    Tac *calc_base = NULL;    // 2. t1 = rbss + base
    Tac *calc_offset = NULL;  // 3. t2 = i * w
    Tac *calc_address = NULL; // 4. t3 = t1 + t2

    // Temporary registers
    std::string t1 = Tac::newRegister();
    std::string t2 = Tac::newRegister();
    std::string t3 = Tac::newRegister();

    // Get vector base address from symbol table
    std::string base = std::to_string(Manager::getSymbol(this->getChild(0)->getValue())->getAddress());

    // Get indexing expression's corresponding temporary register
    std::string i = this->getChild(1)->getTemp();

    // Get type size
    std::string w = std::to_string(getSize(this->getChild(0)->getType()));

    // Get index calculation code
    index_code = this->getChild(1)->getCode(); // ... calculate index ...

    // Check if indexing expression has an address
    if (this->getChild(1)->hasAddress())
        // Load value into the register
        intermed_1 = new Tac(ILOC_LOAD, i, i); // load i => i

    // Generate code
    calc_base = new Tac(ILOC_ADDI, "rbss", base, t1); // addi  rbss, base => t1
    calc_offset = new Tac(ILOC_MULTI, i, w, t2);      // multi i,    w    => t2
    calc_address = new Tac(ILOC_ADD, t1, t2, t3);     // add   t1,   t2   => t3

    // Link code together
    index_code->addLast(intermed_1);
    index_code->addLast(calc_base);
    calc_base->addAfter(calc_offset);
    calc_offset->addAfter(calc_address);

    // Set current node's temporary register to the accessed element address
    this->setTemp(t3);

    // Mark this node as having an address
    this->setAddress();

    // Return first instruction of the code block
    return index_code;
}

Tac *Node::generateAttributionTAC()
{
    // Instructions
    Tac *addr_code = NULL;  // 1. t1 = ... calculate lval address ...
    Tac *expr_code = NULL;  // 2. t2 = ... calculate rval expression ...
    Tac *intermed_1 = NULL; //     2.1 If t2 has address, t2 = Mem(t2)
    Tac *store_val = NULL;  // 3. Mem(t1) = t2 OR Mem(t1) = Mem(t2)

    // Get temporary registers from lval and rval
    std::string t1 = this->getChild(0)->getTemp();
    std::string t2 = this->getChild(1)->getTemp();

    // Get rval expression calculation code and rval address code
    addr_code = this->getChild(0)->getCode(); // ... calculate lval address ..
    expr_code = this->getChild(1)->getCode(); // ... calculate rval expression ...

    // If rval has an address
    if (this->getChild(1)->hasAddress())
        intermed_1 = new Tac(ILOC_LOAD, t2, t2); // load t2 => t2

    // Generate code
    store_val = new Tac(ILOC_STORE, t2, t1); // store t2 => t1

    // Link code together
    addr_code->addLast(expr_code);
    expr_code->addLast(intermed_1);
    expr_code->addLast(store_val);

    // Return first instruction of the code block
    return addr_code;
}

Tac *Node::generateBinopTAC(ILOCop op)
{
    // Instructions
    Tac *left_code = NULL;      // 1. t1 = ... calculate left expression ...
    Tac *intermed_1 = NULL;     //     1.1 If t1 had an address, t1 = Mem(t1)
    Tac *right_code = NULL;     // 2. t2 = ... calculate riht expression ...
    Tac *intermed_2 = NULL;     //     2.1 If t2 had an address, t2 = Mem(t2)
    Tac *operation_code = NULL; // 3. t3 = t1 op t2

    // Temporary registers
    std::string t1 = this->getChild(0)->getTemp();
    std::string t2 = this->getChild(1)->getTemp();
    std::string t3 = Tac::newRegister();

    // Get left and right expressions' code
    left_code = this->getChild(0)->getCode();
    right_code = this->getChild(1)->getCode();

    // Check if left operand has an address
    if (this->getChild(0)->hasAddress())
        // Load value into the register
        intermed_1 = new Tac(ILOC_LOAD, t1, t1); // load t1 => t1

    // Check if right operand has an address
    if (this->getChild(1)->hasAddress())
        // Load value into the register
        intermed_2 = new Tac(ILOC_LOAD, t2, t2); // load t2 => t2

    // Generate code for the operation
    operation_code = new Tac(op, t1, t2, t3); // op t1, t2 => t3

    // Link code together
    left_code->addLast(intermed_1);
    left_code->addLast(right_code);
    right_code->addLast(intermed_2);
    right_code->addLast(operation_code);

    // Set this node's temporary register to the expression result
    this->setTemp(t3);

    // Return first instruction of the code block
    return left_code;
}

Tac *Node::generateTernopTAC()
{
    // Instructions
    Tac *condition_code = NULL; // 1. t1 = ... calculate condition value...
    Tac *intermed_1 = NULL;     //      1.1 If t1 has an address, t1 = Mem(t1)
    Tac *branch_code;           // 2. t1 == true ? jump L1 : jump L2
    // L1:
    Tac *then_code = NULL;       // 3. t2 = ... then command chain ...
    Tac *intermed_2 = NULL;      //      3.1 If t2 has an adddress, t2 = Mem(t2)
    Tac *update_reg_then = NULL; // 4. t4 = t2
    Tac *jump_leave = NULL;      // 5. jump L3
    // L2:
    Tac *else_code = NULL;       // 6. t3 = ... else commmand chain ...
    Tac *intermed_3 = NULL;      //      6.1 If t3 has an adddress, t3 = Mem(t3)
    Tac *update_reg_else = NULL; // 7. t4 = t3
    // L3:
    Tac *leave = NULL; // 8. nop

    // Temporary registers
    std::string t1 = this->getChild(1)->getTemp(); // cond
    std::string t2 = this->getChild(0)->getTemp(); // then
    std::string t3 = this->getChild(2)->getTemp(); // else
    std::string t4 = Tac::newRegister();

    // Labels
    std::string l1 = Tac::newLabel(); // Where to go if expression is true
    std::string l2 = Tac::newLabel(); // Where to go if expression is false
    std::string l3 = Tac::newLabel(); // Where to go after executing true expression code

    // Check if condition has an address
    if (this->getChild(1)->hasAddress())
        // Load value into the register
        intermed_1 = new Tac(ILOC_LOAD, t1, t1);

    // Check if condition has an address
    if (this->getChild(0)->hasAddress())
        // Load value into the register
        intermed_2 = new Tac(ILOC_LOAD, t2, t2);

    // Check if condition has an address
    if (this->getChild(2)->hasAddress())
        // Load value into the register
        intermed_3 = new Tac(ILOC_LOAD, t3, t3);

    // Get code from corresponding nodes
    condition_code = this->getChild(1)->getCode();
    then_code = this->getChild(0)->getCode();
    else_code = this->getChild(2)->getCode();

    // Generate code for all other operations
    branch_code = new Tac(ILOC_CBR, t1, l1, l2); // cbr   t1 => l1, l2
    update_reg_then = new Tac(ILOC_I2I, t2, t4); // i2i   t2 => t4
    jump_leave = new Tac(ILOC_JUMPI, l3);        // jumpI l3
    update_reg_else = new Tac(ILOC_I2I, t3, t4); // i2i   t3 => t4
    leave = new Tac(ILOC_NOP);                   // nop

    // Label code points
    then_code->setLabel(l1); // L1: then_code
    else_code->setLabel(l2); // L2: else_code
    leave->setLabel(l3);     // L3: nop

    // Link code together
    condition_code->addLast(intermed_1);
    condition_code->addLast(branch_code);
    condition_code->addLast(then_code);

    then_code->addLast(intermed_2);
    then_code->addLast(update_reg_then);
    then_code->addLast(jump_leave);
    then_code->addLast(else_code);

    else_code->addLast(intermed_3);
    else_code->addLast(update_reg_else);
    else_code->addLast(leave);

    // Set this node's temp
    this->setTemp(t4);

    // Return first command in the generated command block
    return condition_code;
}
