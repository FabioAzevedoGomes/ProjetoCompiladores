#include "Node.h"

Node::Node(Token *lexval_, Type type_, Statement statement_, bool lval_)
{
    this->lexval = lexval_;
    this->type = type_;
    this->statement = statement_;
    this->next_cmd = NULL;
    this->next_elem = NULL;
    this->code = NULL;
    this->address = false;
    this->value = false;
    this->lval = lval_;

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
    if (next_cmd != NULL)
    {
        // Update next command
        this->next_cmd = next_cmd;

        // Get next command's code
        Tac *next_code = next_cmd->getCode();

        if (this->code != NULL)
            // Link to this node's code
            this->code->addLast(next_code);
        else
            this->code = next_code;
    }
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

void Node::setTemp(std::string *temp)
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
        if (this->lval)
        {
            // std::cout << "Making an lval variable TAC sequence" << std::endl;
            this->code = generateLvalVariableTAC();
        }
        else
        {
            // std::cout << "Making an rval variable TAC sequence" << std::endl;
            this->code = generateRvalVariableTAC();
        }
        break;
    case ST_VECTOR_ACCESS:
        if (this->lval)
        {
            // std::cout << "Making an lval vector access TAC sequence" << std::endl;
            this->code = generateLvalVectorTAC();
        }
        else
        {
            // std::cout << "Making an rval variable TAC sequence" << std::endl;
            this->code = generateRvalVectorTAC();
        }
        break;
    case ST_INIT_VARIABLE:
        // std::cout << "Making an initialization TAC sequence" << std::endl;
        this->code = generateInitializationTAC();
        break;
    case ST_ATTRIB_VARIABLE:
        // std::cout << "Making an attribution TAC sequence" << std::endl;
        this->code = generateAttributionTAC();
        break;
    case ST_UNOP:
        // std::cout << "Making a unop TAC sequence" << std::endl;
        this->code = generateUnopTAC(this->getName());
        break;
    case ST_BINOP:
        // std::cout << "Making a binop TAC sequence" << std::endl;
        this->code = generateBinopTAC(this->getName());
        break;
    case ST_TERNOP:
        // std::cout << "Making a ternary operator TAC sequence" << std::endl;
        this->code = generateTernopTAC();
        break;
    case ST_IF:
        if (this->children.size() == 2)
        {
            // std::cout << "Making an if TAC sequence" << std::endl;
            this->code = generateIfTAC();
        }
        else
        {
            // std::cout << "Making an if-else TAC sequence " << std::endl;
            this->code = generateIfElseTAC();
        }
        break;
    case ST_FOR:
        // std::cout << "Making a For TAC sequence " << std::endl;
        this->code = generateForTAC();
        break;
    case ST_WHILE:
        // std::cout << "Making a While TAC sequence " << std::endl;
        this->code = generateWhileTAC();
        break;
    case ST_SHIFT:
        // std::cout <<  Making a shift (" << this->getName() << ") TAC sequence" << std::endl
        this->code = generateShiftTAC(!strcmp(this->getName(), "<<") ? ILOC_LSHIFT : ILOC_RSHIFT);
        break;
    case ST_FUNCTION_CALL:
        //std::cout << "Making a function call TAC sequence" << std::endl;
        this->code = generateFunctionCallTAC();
        break;
    case ST_FUNCTION_DECLARATION:
        //std::cout << "Making a function declaration TAC sequence" << std::endl;
        this->code = generateFunctionDeclarationTAC();
        break;
    case ST_RETURN:
        //std::cout << "Making a return TAC sequence" << std::endl;
        this->code = generateReturnTAC();
        break;
    default:
        std::cerr << "// Unknown node type for TAC creation, no code will be generated for it: " << std::endl;
        std::cerr << this->getInfo() << std::endl;
        break;
    }

    // Debug
    if (this->code != NULL)
    {
        //std::cout << this->code->getCodeString() << std::endl;
    }
}

void Node::setAddress()
{
    this->value = false;
    this->address = true;
}

void Node::setValue()
{
    this->address = false;
    this->value = true;
}

void Node::setRval()
{
    this->lval = false;

    // If this node's code was already generated as an address, update it (happens in vector access)
    if (this->hasAddress())
    {
        // DEBUG
        //std::cout << "Updating vector to rval access " << std::endl;

        // Instructions
        Tac *load_actual_value = NULL; // t1 = mem(t1)

        // Generate code for loading actual value
        load_actual_value = new Tac(ILOC_LOAD, this->temp, this->temp); // load t1 => t1

        // Link instructions together
        this->code->addLast(load_actual_value);

        //  Mark this node as having a value
        this->setValue();
    }
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

std::string *Node::getTemp()
{
    return this->temp;
}

Tac *Node::getCode()
{
    Tac *copy = NULL;

    if (this->code != NULL)
        // Copy code
        copy = new Tac(*(this->code));

    // Return copy of the code
    return copy;
}

bool Node::hasAddress()
{
    return this->address;
}

bool Node::hasValue()
{
    return this->value;
}

std::list<std::string *> Node::getTrueList()
{
    return this->true_list;
}

std::list<std::string *> Node::getFalseList()
{
    return this->false_list;
}

void Node::clearTrueList()
{
    this->true_list.clear();
}

void Node::clearFalseList()
{
    this->false_list.clear();
}

// TAC GENERATION

Tac *Node::generateLvalVariableTAC()
{
    // Instructions
    Tac *calculate_base_address = NULL; // 1. t1 = rfp : rbss + base

    // Get variable base address from symbol table
    Symbol *var = Manager::getSymbol(this->getValue());
    std::string *base = Tac::getName(std::to_string(var->getAddress()));

    // Temporary register
    std::string *t1 = Tac::newRegister();

    // Frame or data segment register
    std::string *rb = Tac::getRegister(var->isGlobal());

    // Generate code
    calculate_base_address = new Tac(ILOC_ADDI, rb, base, t1); // addI rb, base => t1

    // Set node temp to t1
    this->setTemp(t1);

    // Mark it as having an address
    this->setAddress();

    return calculate_base_address;
}

Tac *Node::generateRvalVariableTAC()
{
    // Instructions
    Tac *calculate_base_address = NULL; // 1. t1 = mem(rfp : rbss + base)
    Tac *load_actual_value = NULL;      // 2. t1 = mem(t1)

    // Calculate the variable's address
    calculate_base_address = this->generateLvalVariableTAC();

    // Load actual value into the register
    load_actual_value = new Tac(ILOC_LOAD, this->temp, this->temp); // load t1 => t1

    // Link instructions together
    calculate_base_address->addLast(load_actual_value);

    // Mark this node as having a value
    this->setValue();

    // Return first element in the code block
    return calculate_base_address;
}

Tac *Node::generateLvalVectorTAC()
{
    // Instructions

    Tac *calculate_index_code = NULL;       // t1 = calculate index
    Tac *calculate_vector_base = NULL;      // t2 = rbss + base
    Tac *calculate_offset = NULL;           // t3 = t1 * w
    Tac *calculate_indexed_position = NULL; // t4 = t2 + t3

    // Get vector base address from symbol table
    Symbol *vec = Manager::getSymbol(this->getChild(0)->getValue());
    std::string *base = Tac::getName(std::to_string(vec->getAddress()));

    // Get vector type size
    std::string *w = Tac::getName(std::to_string(getSize(vec->getType())));

    // Get index expression temp (accessed index)
    std::string *t1 = this->getChild(1)->getTemp();

    // Get rbss name (Vectores are always global)
    std::string *rbss = Tac::getName("rbss");

    // Create new temporary registers
    std::string *t2 = Tac::newRegister();
    std::string *t3 = Tac::newRegister();
    std::string *t4 = Tac::newRegister();

    //

    // Get index calculation code
    calculate_index_code = this->getChild(1)->getCode();

    // Generate code for acessing the vector
    calculate_vector_base = new Tac(ILOC_ADDI, rbss, base, t2); // addI rbss, base => t2
    calculate_offset = new Tac(ILOC_MULTI, t1, w, t3);          // multI t1, w => t3
    calculate_indexed_position = new Tac(ILOC_ADD, t2, t3, t4); // add t2, t3 => t4

    // Link instructions together
    calculate_index_code->addLast(calculate_vector_base);
    calculate_vector_base->addLast(calculate_offset);
    calculate_offset->addLast(calculate_indexed_position);

    //

    // Set node temp to t4
    this->setTemp(t4);

    // Mark this node as having an address
    this->setAddress();

    // Return first instruction in the generated code block
    return calculate_index_code;
}

Tac *Node::generateRvalVectorTAC()
{
    // Instructions
    Tac *calculate_address = NULL; // t1 = calculate indexed address
    Tac *load_actual_value = NULL; // t1 = mem(t1)

    // Calculate base address for the indexed position
    calculate_address = this->generateLvalVectorTAC();

    // Generate code for loading actual value
    load_actual_value = new Tac(ILOC_LOAD, this->temp, this->temp); // load t1 => t1

    // Link instructions together
    calculate_address->addLast(load_actual_value);

    //  Mark this node as having a value
    this->setValue();

    // Return first element in the code block
    return calculate_address;
}

Tac *Node::generateInitializationTAC()
{
    // Instructions
    Tac *calculate_lval = NULL; // t1 = lval address
    Tac *calculate_rval = NULL; // t2 = rval value (literal)
    Tac *store_result = NULL;   // Mem(t1) = t2

    // Generate code for lval child
    this->getChild(0)->generateCode();

    // Get code for lval and rval
    calculate_lval = this->getChild(0)->getCode();
    calculate_rval = this->getChild(1)->getCode();

    // Temporary registers
    std::string *t1 = this->getChild(0)->getTemp();
    std::string *t2 = this->getChild(1)->getTemp();

    // Generate code for store instruction
    store_result = new Tac(ILOC_STORE, t2, t1); // store t2 => t1

    // Link code together
    calculate_lval->addLast(calculate_rval);
    calculate_rval->addLast(store_result);

    // Return first instruction in the code block
    return calculate_lval;
}

Tac *Node::generateAttributionTAC()
{
    // Common Instructions
    Tac *calculate_lval = NULL; // t1 = lval address
    Tac *calculate_rval = NULL; // t2 = rval value
    Tac *store_result = NULL;   // Mem(t1) = t2

    // Additional instructions (Used if rval is a logic expression)
    Tac *attribute_true = NULL;  // loadI 1 => t2
    Tac *attribute_false = NULL; // loadI 0 => t2
    Tac *jump_exit = NULL;       // jumpI -> l3
    Tac *nop = NULL;             // nop

    // Strings for true and false literal (Used if rval is a logic expression)
    std::string *true_literal = NULL;
    std::string *false_literal = NULL;

    // Labels (Used if rval is a logic expression)
    std::string *l1 = NULL;
    std::string *l2 = NULL;
    std::string *l3 = NULL;

    // Get temporary registers for lval and rval
    std::string *t1 = this->getChild(0)->getTemp();
    std::string *t2 = NULL;

    // Get code for lval and rval
    calculate_lval = this->getChild(0)->getCode();
    calculate_rval = this->getChild(1)->getCode();

    // If rval is a logic operation
    if (logic_binop_code.find(this->getChild(1)->getName()) != logic_binop_code.end())
    {
        // Generate labels for attribution
        l1 = Tac::newLabel(); // Where to go if expession is true
        l2 = Tac::newLabel(); // Where to go if expression is false
        l3 = Tac::newLabel(); // Where to go at the end

        // Generate names for true and false literals
        true_literal = Tac::getName(std::to_string(1));
        false_literal = Tac::getName(std::to_string(0));

        // Create new temporary register for logic expression result
        t2 = Tac::newRegister();

        // Generate additional code
        attribute_true = new Tac(ILOC_LOADI, true_literal, t2);   // L1: loadI 1 => t2
        jump_exit = new Tac(ILOC_JUMPI, l3);                      //     jumpI -> L3
        attribute_false = new Tac(ILOC_LOADI, false_literal, t2); // L2: loadI 0 => t2
        nop = new Tac(ILOC_NOP);                                  // L3: nop

        // Label code
        attribute_true->setLabel(l1);
        attribute_false->setLabel(l2);
        nop->setLabel(l3);

        // Backpatch expression true and false list holes
        Tac::patch(this->getChild(1)->getTrueList(), l1);
        Tac::patch(this->getChild(1)->getFalseList(), l2);

        // Clear lists
        this->getChild(1)->clearTrueList();
        this->getChild(1)->clearFalseList();

        // Link additional code together
        attribute_true->addLast(jump_exit);
        jump_exit->addLast(attribute_false);
        attribute_false->addLast(nop);
    }
    else
    {
        // If not just get t2
        t2 = this->getChild(1)->getTemp();
    }

    // Generate common code (store)
    store_result = new Tac(ILOC_STORE, t2, t1); // store t2 => t1

    // Link code common to both cases
    calculate_lval->addLast(calculate_rval);
    calculate_rval->addLast(attribute_true);
    calculate_rval->addLast(store_result);

    // Return first instruction in the code block
    return calculate_lval;
}

Tac *Node::generateUnopTAC(std::string op)
{
    Tac *code = NULL;

    // If code is for a logic NOT
    if (!strcmp(op.c_str(), "!"))
    {
        // Get child code
        code = this->getChild(0)->getCode();

        // Swap true and false lists
        this->true_list = this->getChild(0)->getFalseList();
        this->false_list = this->getChild(0)->getTrueList();
    }

    // If code is for a signal inversion
    if (!strcmp(op.c_str(), "-"))
    {
        // Operand code
        Tac *operand_code = this->getChild(0)->getCode();

        // Names
        std::string *const_0 = Tac::getName(std::to_string(0));

        // Temporary registers
        std::string *t1 = this->getChild(0)->getTemp();
        std::string *t2 = Tac::newRegister();

        // Generate code
        code = new Tac(ILOC_RSUBI, t1, const_0, t2); // rsubI t1, 0 => t2

        // Link code together
        code->addLast(operand_code);

        // Set this node's temp
        this->setTemp(t2);

        // Mark this node as having a value
        this->setValue();
    }

    // Return generated code
    return code;
}

Tac *Node::generateBinopTAC(std::string op)
{
    Tac *code = NULL;

    // If code is in the binary arithmetic operation list
    if (arithmetic_binop_code.find(op) != arithmetic_binop_code.end())
    {
        // std::cout << "Making an arithmetic binop TAC sequence" << std::endl;

        // Generate arithmetic expression
        code = generateArithmeticBinopTAC(arithmetic_binop_code.at(op));
    }
    else if (logic_binop_code.find(op) != logic_binop_code.end())
    {
        // std::cout << "Making a logic binop TAC sequence" << std::endl;

        // If not, generate boolean expression
        code = generateLogicBinopTAC(logic_binop_code.at(op));
    }
    else
    {
        // DEBUG
        std::cerr << "Operation is neither arithmetic nor logic: " << op << std::endl;
    }

    // Return generated code
    return code;
}

Tac *Node::generateArithmeticBinopTAC(ILOCop op)
{
    // Instructions
    Tac *left_operand_code = NULL;     // t1 = calculate left expression
    Tac *right_operand_code = NULL;    // t2 = calculate right expression
    Tac *operation_instruction = NULL; // t3 = t1 op t2

    // Generate temporary registers from left and right operand
    std::string *t1 = this->getChild(0)->getTemp();
    std::string *t2 = this->getChild(1)->getTemp();

    // Get new temporary register for result
    std::string *t3 = Tac::newRegister();

    //

    // Get left and right operation's code
    left_operand_code = this->getChild(0)->getCode();
    right_operand_code = this->getChild(1)->getCode();

    // Generate code for arithmetic operation
    operation_instruction = new Tac(op, t1, t2, t3); // op t1, t2 => t3

    //

    // Link code together
    left_operand_code->addLast(right_operand_code);
    right_operand_code->addLast(operation_instruction);

    // Set t3 as this node's temp
    this->setTemp(t3);

    // Mark this node as having a value
    this->setValue();

    // Return first instruction in the generated code block
    return left_operand_code;
}

Tac *Node::generateAndTAC()
{
    // Instructions from the operands
    Tac *left_operand_code = NULL;
    Tac *right_operand_code = NULL;

    //

    // New label used for backpatching
    std::string *l1 = Tac::newLabel();

    // Patch true list from left operand
    Tac::patch(this->getChild(0)->getTrueList(), l1);

    // Clear true list
    this->getChild(0)->clearTrueList();

    // Set this node's true list as the right operand's true list (left was patched)
    this->true_list = this->getChild(1)->getTrueList();

    // Set this node's false list as the left and right operand's false list
    this->false_list = this->getChild(0)->getFalseList();
    this->false_list.merge(this->getChild(1)->getFalseList());

    //

    // Get operand's code
    left_operand_code = this->getChild(0)->getCode();
    right_operand_code = this->getChild(1)->getCode();

    // Link codes together
    left_operand_code->addLast(right_operand_code);

    // Label right operand code with new label
    right_operand_code->setLabel(l1);

    // Return first instruction in the code block
    return left_operand_code;
}

Tac *Node::generateOrTAC()
{
    // Instructions from the operands
    Tac *left_operand_code = NULL;
    Tac *right_operand_code = NULL;

    //

    // New label used for backpatching
    std::string *l1 = Tac::newLabel();

    // Patch false list from left operand
    Tac::patch(this->getChild(0)->getFalseList(), l1);

    // Clear false list
    this->getChild(0)->clearFalseList();

    // Set this node's false list as the right operand's false list (left was patched)
    this->false_list = this->getChild(1)->getFalseList();

    // Set this node's true list as the left and right operand's true list
    this->true_list = this->getChild(0)->getTrueList();
    this->true_list.merge(this->getChild(1)->getTrueList());

    //

    // Get operand's code
    left_operand_code = this->getChild(0)->getCode();
    right_operand_code = this->getChild(1)->getCode();

    // Link codes together
    left_operand_code->addLast(right_operand_code);

    // Label right operand code with new label
    right_operand_code->setLabel(l1);

    // Return first instruction in the code block
    return left_operand_code;
}

Tac *Node::generateComparisonTAC(ILOCop op)
{
    // Instructions
    Tac *left_operand_code = NULL;
    Tac *right_operand_code = NULL;
    Tac *compare_instruction = NULL;
    Tac *branch_instruction = NULL;

    // Holes to-be-patched
    std::string *h1 = Tac::newHole(); // true
    std::string *h2 = Tac::newHole(); // false

    // Get operands' temporary registers
    std::string *t1 = this->getChild(0)->getTemp();
    std::string *t2 = this->getChild(1)->getTemp();

    // Generate temporary register for comparison result
    std::string *t3 = Tac::newRegister();

    //

    // Get code from operands
    left_operand_code = this->getChild(0)->getCode();
    right_operand_code = this->getChild(1)->getCode();

    // Generate code for comparing operands and branching
    compare_instruction = new Tac(op, t1, t2, t3);      // op t1, t2 => t3
    branch_instruction = new Tac(ILOC_CBR, t3, h1, h2); // cbr t3 -> H1, H2

    // Link instructions together
    left_operand_code->addLast(right_operand_code);
    right_operand_code->addLast(compare_instruction);
    compare_instruction->addLast(branch_instruction);

    // Add holes to true and false lists
    this->true_list.push_back(h1);
    this->false_list.push_back(h2);

    // Set temporary register
    this->setTemp(t3);

    // Mark this node as having a value
    this->setValue();

    // Return first instruction in the generated code block
    return left_operand_code;
}

Tac *Node::generateLogicBinopTAC(ILOCop op)
{
    Tac *code = NULL;

    switch (op)
    {
    case ILOC_AND:
        code = generateAndTAC();
        break;
    case ILOC_OR:
        code = generateOrTAC();
        break;
    case ILOC_CMP_NE:
    case ILOC_CMP_GE:
    case ILOC_CMP_LE:
    case ILOC_CMP_EQ:
    case ILOC_CMP_GT:
    case ILOC_CMP_LT:
        code = generateComparisonTAC(op);
        break;
    default:
        break;
    }

    return code;
}

Tac *Node::generateTernopTAC()
{
    // Common instructions
    Tac *condition_code = NULL;
    Tac *then_code = NULL;
    Tac *else_code = NULL;
    Tac *jump_exit = NULL;
    Tac *attribute_true = NULL;
    Tac *attribute_false = NULL;
    Tac *nop = NULL;

    // Additional instructions (Used if then statement is a logic expression)
    Tac *then_attribute_true = NULL;  // loadI 1 => t2
    Tac *then_attribute_false = NULL; // loadI 0 => t2
    Tac *then_jump_exit = NULL;       // jumpI -> l3
    Tac *then_nop = NULL;             // nop

    // Additional instructions (Used if else statement is a logic expression)
    Tac *else_attribute_true = NULL;  // loadI 1 => t2
    Tac *else_attribute_false = NULL; // loadI 0 => t2
    Tac *else_jump_exit = NULL;       // jumpI -> l3
    Tac *else_nop = NULL;             // nop

    // Temporary registers
    std::string *t1 = NULL;
    std::string *t2 = NULL;
    std::string *t3 = Tac::newRegister();

    // Strings for true and false literals (If true or false statment is a logic expression)
    std::string *then_true_literal = NULL;
    std::string *then_false_literal = NULL;
    std::string *else_true_literal = NULL;
    std::string *else_false_literal = NULL;

    // Labels
    std::string *l1 = Tac::newLabel();
    std::string *l2 = Tac::newLabel();
    std::string *l3 = Tac::newLabel();

    // Labels if true statement is a logic operation
    std::string *l4 = NULL;
    std::string *l5 = NULL;
    std::string *l6 = NULL;

    // Labels if false statement is a logic operation
    std::string *l7 = NULL;
    std::string *l8 = NULL;
    std::string *l9 = NULL;

    // Backpatch condition's true and false statements
    Tac::patch(this->getChild(1)->getTrueList(), l1);
    Tac::patch(this->getChild(1)->getFalseList(), l2);

    // Clear lists
    this->getChild(1)->clearTrueList();
    this->getChild(1)->clearFalseList();

    // Get condition, then-statment and else-statement codes
    then_code = this->getChild(0)->getCode();
    condition_code = this->getChild(1)->getCode();
    else_code = this->getChild(2)->getCode();

    // If then statement is a logic operation
    if (logic_binop_code.find(this->getChild(0)->getName()) != logic_binop_code.end())
    {
        // Generate new labels
        l4 = Tac::newLabel();
        l5 = Tac::newLabel();
        l6 = Tac::newLabel();

        // Generate names for true and false literals
        then_true_literal = Tac::getName(std::to_string(1));
        then_false_literal = Tac::getName(std::to_string(0));

        // Create temporary register for result
        t1 = Tac::newRegister();

        // Generate additional code
        then_attribute_true = new Tac(ILOC_LOADI, then_true_literal, t1);   // L4: loadI 1 => t1
        then_jump_exit = new Tac(ILOC_JUMPI, l6);                           //     jumpI -> l6
        then_attribute_false = new Tac(ILOC_LOADI, then_false_literal, t1); // L5: loadI 0 => t1
        then_nop = new Tac(ILOC_NOP);                                       // L6: nop

        // Label code
        then_attribute_true->setLabel(l4);
        then_attribute_false->setLabel(l5);
        then_nop->setLabel(l6);

        // Backpatch expression true and false list holes
        Tac::patch(this->getChild(0)->getTrueList(), l4);
        Tac::patch(this->getChild(0)->getFalseList(), l5);

        // Clear lists
        this->getChild(0)->clearTrueList();
        this->getChild(0)->clearFalseList();

        // Link additional code together
        then_attribute_true->addLast(then_jump_exit);
        then_jump_exit->addLast(then_attribute_false);
        then_attribute_false->addLast(then_nop);
    }
    // If not
    else
    {
        // Just get temporary register
        t1 = this->getChild(0)->getTemp();
    }

    // If else statement is a logic operation
    if (logic_binop_code.find(this->getChild(2)->getName()) != logic_binop_code.end())
    {
        // Generate new labels
        l7 = Tac::newLabel();
        l8 = Tac::newLabel();
        l9 = Tac::newLabel();

        // Generate names for true and false literals
        else_true_literal = Tac::getName(std::to_string(1));
        else_false_literal = Tac::getName(std::to_string(0));

        // Create temporary register for result
        t2 = Tac::newRegister();

        // Generate additional code
        else_attribute_true = new Tac(ILOC_LOADI, else_true_literal, t2);   // L7: loadI 1 => t2
        else_jump_exit = new Tac(ILOC_JUMPI, l9);                           //     jumpI -> l9
        else_attribute_false = new Tac(ILOC_LOADI, else_false_literal, t2); // L8: loadI 0 => t2
        else_nop = new Tac(ILOC_NOP);                                       // L9: nop

        // Label code
        else_attribute_true->setLabel(l7);
        else_attribute_false->setLabel(l8);
        else_nop->setLabel(l9);

        // Backpatch expression true and false list holes
        Tac::patch(this->getChild(2)->getTrueList(), l7);
        Tac::patch(this->getChild(2)->getFalseList(), l8);

        // Clear lists
        this->getChild(2)->clearTrueList();
        this->getChild(2)->clearFalseList();

        // Link additional code together
        else_attribute_true->addLast(else_jump_exit);
        else_jump_exit->addLast(else_attribute_false);
        else_attribute_false->addLast(else_nop);
    }
    // If not
    else
    {
        // Just get temporary register
        t2 = this->getChild(2)->getTemp();
    }

    // Create jump, attribution and nop codes
    attribute_true = new Tac(ILOC_I2I, t1, t3);  //     i2i t1 -> t3
    jump_exit = new Tac(ILOC_JUMPI, l3);         //     jump -> l3
    attribute_false = new Tac(ILOC_I2I, t2, t3); //     i2i t2 -> t3
    nop = new Tac(ILOC_NOP);                     // L3: nop

    // Link code together
    condition_code->addLast(then_code);

    then_code->addLast(then_attribute_true); // Convert logic expression to 1 or 0
    then_code->addLast(attribute_true);
    then_code->addLast(jump_exit);

    jump_exit->addLast(else_code);

    else_code->addLast(else_attribute_true); // Convert logic expression to 1 or 0
    else_code->addLast(attribute_false);
    else_code->addLast(nop);

    // Label code
    then_code->setLabel(l1);
    else_code->setLabel(l2);
    jump_exit->setLabel(l3);

    // Set this node's temp
    this->setTemp(t3);

    // Mark this node as having a value
    this->setValue();

    // Return first instruction in the code block
    return condition_code;
}

Tac *Node::generateIfTAC()
{
    // Instructions
    Tac *condition_code = NULL;
    Tac *then_code = NULL;
    Tac *nop = NULL;

    // Labels
    std::string *l1 = Tac::newLabel(); // Where to go if true
    std::string *l2 = Tac::newLabel(); // Where to go if false and when done

    // Backpatch condition's true and false list
    Tac::patch(this->getChild(0)->getTrueList(), l1);
    Tac::patch(this->getChild(0)->getFalseList(), l2);

    // Clear lists
    this->getChild(0)->clearTrueList();
    this->getChild(0)->clearFalseList();

    // Get condition and then-statement codes
    condition_code = this->getChild(0)->getCode();
    then_code = this->getChild(1)->getCode();

    // Create the nop code
    nop = new Tac(ILOC_NOP);

    // Link instructions together
    condition_code->addLast(then_code);
    condition_code->addLast(nop);

    // Label instructions
    then_code->setLabel(l1);
    nop->setLabel(l2);

    // Return first instruction in the code block
    return condition_code;
}

Tac *Node::generateIfElseTAC()
{
    // Instructions
    Tac *condition_code = NULL;
    Tac *then_code = NULL;
    Tac *else_code = NULL;
    Tac *jump_exit = NULL;
    Tac *nop = NULL;

    // Labels
    std::string *l1 = Tac::newLabel(); // Where to go if true
    std::string *l2 = Tac::newLabel(); // Where to go if false
    std::string *l3 = Tac::newLabel(); // Where to go when done

    // Backpatch condition's true and false list
    Tac::patch(this->getChild(0)->getTrueList(), l1);
    Tac::patch(this->getChild(0)->getFalseList(), l2);

    // Clear lists
    this->getChild(0)->clearTrueList();
    this->getChild(0)->clearFalseList();

    //

    // Get condition, then-statement and else-statement codes
    condition_code = this->getChild(0)->getCode();
    then_code = this->getChild(1)->getCode();
    else_code = this->getChild(2)->getCode();

    // Create the jump and nop codes
    nop = new Tac(ILOC_NOP);             // nop
    jump_exit = new Tac(ILOC_JUMPI, l3); // jumpI -> l3

    // Link instructions together
    condition_code->addLast(then_code);
    condition_code->addLast(jump_exit);
    jump_exit->addLast(else_code);
    else_code->addLast(nop);

    // Label instructions
    then_code->setLabel(l1);
    else_code->setLabel(l2);
    nop->setLabel(l3);

    // Return first instruction in the generated code block
    return condition_code;
}

Tac *Node::generateForTAC()
{
    // Instructions
    Tac *initial_attribution_code = NULL;
    Tac *condition_code = NULL;
    Tac *do_code = NULL;
    Tac *loop_attribution_code = NULL;
    Tac *jump_back = NULL;
    Tac *nop = NULL;

    // Labels
    std::string *l1 = Tac::newLabel(); // Where to go if true
    std::string *l2 = Tac::newLabel(); // Where to go if false
    std::string *l3 = Tac::newLabel(); // Where to go after a loop

    // Backpatch condition's true and false statements
    Tac::patch(this->getChild(1)->getTrueList(), l1);
    Tac::patch(this->getChild(1)->getFalseList(), l2);

    //  Clear lists
    this->getChild(1)->clearTrueList();
    this->getChild(1)->clearFalseList();

    //

    // Get initial attribution, condition, loop attribution and do-statement codes
    initial_attribution_code = this->getChild(0)->getCode();
    condition_code = this->getChild(1)->getCode();
    loop_attribution_code = this->getChild(2)->getCode();
    do_code = this->getChild(3)->getCode();

    // Create jump and nop codes
    jump_back = new Tac(ILOC_JUMPI, l3); // jumpI l3
    nop = new Tac(ILOC_NOP);             // nop

    // Link code together
    initial_attribution_code->addLast(condition_code);
    condition_code->addLast(do_code);

    do_code->addLast(loop_attribution_code);
    do_code->addLast(jump_back);

    jump_back->addLast(nop);

    // Label code
    do_code->setLabel(l1);
    nop->setLabel(l2);
    condition_code->setLabel(l3);

    // Return first instruction in the generated code block
    return initial_attribution_code;
}

Tac *Node::generateWhileTAC()
{
    // Instructions
    Tac *condition_code = NULL;
    Tac *do_code = NULL;
    Tac *jump_back = NULL;
    Tac *nop = NULL;

    // Labels
    std::string *l1 = Tac::newLabel(); // Where to go if true
    std::string *l2 = Tac::newLabel(); // Where to go if false
    std::string *l3 = Tac::newLabel(); // Where to go after a loop

    // Backpatch condition's true and false lists
    Tac::patch(this->getChild(0)->getTrueList(), l1);
    Tac::patch(this->getChild(0)->getFalseList(), l2);

    // Clear lists
    this->getChild(0)->clearTrueList();
    this->getChild(0)->clearFalseList();

    //

    // Get condition and do-statement codes
    condition_code = this->getChild(0)->getCode();
    do_code = this->getChild(1)->getCode();

    // Create jump back and nop codes
    jump_back = new Tac(ILOC_JUMPI, l3); // jumpI -> l3
    nop = new Tac(ILOC_NOP);             // nop

    // Link code together
    condition_code->addLast(do_code);
    condition_code->addLast(jump_back);
    jump_back->addLast(nop);

    // Label code
    do_code->setLabel(l1);
    nop->setLabel(l2);
    condition_code->setLabel(l3);

    // Return first instruction in the generated code block
    return condition_code;
}

Tac *Node::generateShiftTAC(ILOCop op)
{
    // Instructions
    Tac *calculate_variable_address = NULL;
    Tac *calculate_shift_amount = NULL;
    Tac *load_actual_value = NULL;
    Tac *shift_instruction = NULL;
    Tac *store_new_value = NULL;

    // Get temporary registers
    std::string *t1 = this->getChild(0)->getTemp();
    std::string *t2 = this->getChild(1)->getTemp();

    // New temporary register to store the result
    std::string *t3 = Tac::newRegister();

    //  Get variable and shift amount calculation code
    calculate_variable_address = this->getChild(0)->getCode();
    calculate_shift_amount = this->getChild(1)->getCode();

    // Generate code for shift operation
    load_actual_value = new Tac(ILOC_LOAD, t1, t3); // load   t1     => t3
    shift_instruction = new Tac(op, t3, t2, t3);    // shift t3, t2 => t3
    store_new_value = new Tac(ILOC_STORE, t3, t1);  // store  t3     => t1

    // Link code together
    calculate_variable_address->addLast(calculate_shift_amount);
    calculate_shift_amount->addLast(load_actual_value);
    load_actual_value->addLast(shift_instruction);
    shift_instruction->addLast(store_new_value);

    // Return first instruction in the generated code block
    return calculate_variable_address;
}

Tac *Node::generateFunctionDeclarationTAC()
{
    // Instructions
    Tac *update_rfp_call = NULL; // rfp <- rsp
    Tac *update_rsp_vars = NULL; // rsp <- rsp + (local_vars(N))
    Tac *function_body = NULL;   // ... function code ...

    // Load as many passed arguments as necessary
    Tac *load_param_i = NULL; // ti <- Mem(rfp + param_address)
    Tac *save_param_i = NULL; // Mem(rfp + var_addr) <- ti

    // Temporary registers
    std::string *ti = Tac::newRegister(); // For loading parameters into their respective local vars

    // Labels
    std::string *l1 = Tac::newLabel(); // Label given to function starting point

    // Get current active symbol table
    SymbolTable *current_st = Manager::getActiveSymbolTable();

    // Get function symbol
    Symbol *function = Manager::getSymbol(this->lexval);

    //

    // Calculate amount of space occupied by function parameters
    int params_occupied_size = function->getParams().size() * getSize(TYPE_INT);

    // Sum frame call information, return value and parameter values' occupied memory in the frame
    int call_offset = 3 * getSize(TYPE_INT) + params_occupied_size + getSize(TYPE_INT);

    // Calculate amount of space used by the local variables in the function scope
    int local_vars_occupied_size = current_st->getAddress();

    // Names
    std::string *rfp = Tac::getName("rfp");
    std::string *rsp = Tac::getName("rsp");
    std::string *local_offset = Tac::getName(std::to_string(local_vars_occupied_size));
    std::string *param_load_address = NULL;
    std::string *param_store_address = NULL;

    //

    // Get code for the function start (If exists)
    if (this->getChild(0) != NULL)
        function_body = this->getChild(0)->getCode();

    // Generate code
    update_rfp_call = new Tac(ILOC_I2I, rsp, rfp);                // i2i  rsp               => rfp
    update_rsp_vars = new Tac(ILOC_ADDI, rsp, local_offset, rsp); // addI rsp, vars_offset  => rsp

    // Generate code for loading arguments, if any
    int param_load_offset = 3 * getSize(TYPE_INT);
    int param_store_offset = call_offset;
    for (uint i = 0; i < function->getParams().size(); ++i, param_load_offset += getSize(TYPE_INT), param_store_offset += getSize(TYPE_INT))
    {
        // Get param address name
        param_load_address = Tac::getName(std::to_string(param_load_offset));
        param_store_address = Tac::getName(std::to_string(param_store_offset));

        // Generate code for loading parameter i
        load_param_i = new Tac(ILOC_LOADAI, rfp, param_load_address, ti);   // loadAI rfp, param_load_offset=> ti
        save_param_i = new Tac(ILOC_STOREAI, ti, rfp, param_store_address); // storeAI ti => rfp, param_store_offset

        // Link code together
        load_param_i->addLast(save_param_i);
        update_rsp_vars->addLast(load_param_i);
    }

    // Link code together
    update_rfp_call->addLast(update_rsp_vars);
    update_rsp_vars->addLast(function_body);

    // Label code and function symbol
    update_rfp_call->setLabel(l1);
    function->giveLabel(l1);

    // If this is the 'main' function
    if (!strcmp(this->getValue()->getName(), "main"))
    {
        // Update reference in manager
        Manager::updateMain(this, function);
    }

    //

    // Add artificial 'return 0' command, if programmer forgot to add it

    //

    // Instructions
    Tac *load_result = NULL;
    Tac *save_return = NULL;        // Mem(rfp + offset_return) <- t0
    Tac *get_return_address = NULL; // t1 <- Mem(rfp + 0)
    Tac *get_old_rsp = NULL;        // t2 <- Mem(rfp + 4)
    Tac *get_old_rfp = NULL;        // t3 <- Mem(rfp + 8)
    Tac *update_rsp = NULL;         // rsp <- t2
    Tac *update_rfp = NULL;         // rfp <- t3
    Tac *jump_return = NULL;        // jump t1

    // Temporary registers
    std::string *t1 = Tac::newRegister(); // Return value
    std::string *t2 = Tac::newRegister(); // Return address
    std::string *t3 = Tac::newRegister(); // Old rsp
    std::string *t4 = Tac::newRegister(); // Old rfp

    // Calculate size occupied by function parameters in frame
    int return_offset = 3 * getSize(TYPE_INT) + function->getParams().size() * getSize(TYPE_INT);

    // Names
    std::string *offset_return = Tac::getName(std::to_string(return_offset));
    std::string *const_0 = Tac::getName(std::to_string(0));
    std::string *const_4 = Tac::getName(std::to_string(4));
    std::string *const_8 = Tac::getName(std::to_string(8));

    // Generate code for the return statement
    load_result = new Tac(ILOC_LOADI, const_0, t1);              // loadI   0      => t1
    save_return = new Tac(ILOC_STOREAI, t1, rfp, offset_return); // storeAI t1     => rfp, offset_return
    get_return_address = new Tac(ILOC_LOADAI, rfp, const_0, t2); // loadAI  rfp, 0 => t2
    get_old_rsp = new Tac(ILOC_LOADAI, rfp, const_4, t3);        // loadAI  rfp, 4 => t3
    get_old_rfp = new Tac(ILOC_LOADAI, rfp, const_8, t4);        // loadAi  rpf, 8 => t4
    update_rsp = new Tac(ILOC_I2I, t3, rsp);                     // i2i     t3     => rsp
    update_rfp = new Tac(ILOC_I2I, t4, rfp);                     // i2i     t4     => rfp
    jump_return = new Tac(ILOC_JUMP, t2);                        // jump           => t2

    // Link code together
    load_result->addLast(save_return);
    save_return->addLast(get_return_address);
    get_return_address->addLast(get_old_rsp);
    get_old_rsp->addLast(get_old_rfp);
    get_old_rfp->addLast(update_rsp);
    update_rsp->addLast(update_rfp);
    update_rfp->addLast(jump_return);

    // Add 'fake' return at the end of function
    update_rsp_vars->addLast(load_result);

    // Return first instruction in the generated code block
    return update_rfp_call;
}

Tac *Node::generateFunctionCallTAC()
{
    // Instructions
    Tac *calc_return_address = NULL; // t1 <- rpc + (3 + param_count + 2)
    Tac *save_return_address = NULL; // Mem (rsp + 0) <- t1
    Tac *save_current_rsp = NULL;    // Mem (rsp + 4) <- rsp
    Tac *save_current_rfp = NULL;    // Mem (rsp + 8) <- rfp
    Tac *jump_function = NULL;       // jumpI => func_label
    Tac *get_return_val = NULL;      // t1 <- Mem(rsp + offset_return)

    // Load as many parameters as necessary
    Tac *code_param_i = NULL; // t2 <- ... calculate param expression ...
    Tac *save_param_i = NULL; // Mem(rsp + param_offset) <- t2

    // Get function symbol
    Symbol *called_func = Manager::getSymbol(this->getValue());
    Node *aux = NULL; // Auxiliary pointer for traversing arguments

    // Get function label
    std::string *l1 = called_func->getLabel();

    // Temporary registers
    std::string *t1 = Tac::newRegister(); // Return value
    std::string *t2 = NULL;               // Parameter copying

    // Calculate size occupied by return data + params in function frame
    int offset_return_val = 3 * getSize(TYPE_INT) + called_func->getParams().size() * getSize(TYPE_INT);

    // Calculate return address offset
    int offset_return_addr = 5; // Considering 0 parameters, 5 instructions are still needed

    // Parameter offset
    int param_offset = 3 * getSize(TYPE_INT); // Starts 3 values after rsp

    // Names
    std::string *rfp = Tac::getName("rfp");
    std::string *rsp = Tac::getName("rsp");
    std::string *rpc = Tac::getName("rpc");
    std::string *const_0 = Tac::getName(std::to_string(0));
    std::string *const_4 = Tac::getName(std::to_string(4));
    std::string *const_8 = Tac::getName(std::to_string(8));
    std::string *offset_param = NULL;

    //

    // Generate code for saving state variables
    save_current_rsp = new Tac(ILOC_STOREAI, rsp, rsp, const_4); // storeAI rsp => rsp, 4
    save_current_rfp = new Tac(ILOC_STOREAI, rfp, rsp, const_8); // storeAi rfp => rsp, 8

    // Generate code for passing arguments, if any
    aux = this->getChild(0);
    for (uint i = 0; i < called_func->getParams().size(); ++i, param_offset += getSize(TYPE_INT), aux = aux->getNextElement())
    {
        // Get temp for argument
        t2 = aux->getTemp();

        // Get code for argument
        code_param_i = aux->getCode();

        // Get name for offset
        offset_param = Tac::getName(std::to_string(param_offset));

        // Generate code for 'stacking' value of parameter in the called function's frame
        save_param_i = new Tac(ILOC_STOREAI, t2, rsp, offset_param); // storeAI t2 => rsp, param_offset

        // Link to the instructions
        save_current_rfp->addLast(code_param_i);
        save_current_rfp->addLast(save_param_i);

        // Increment the return address offset by the size of the new code
        offset_return_addr += code_param_i->getCodeSize();
    }

    std::string *offset_retval = Tac::getName(std::to_string(offset_return_val));
    std::string *offset_retaddr = Tac::getName(std::to_string(offset_return_addr));

    // Calculate the return address
    calc_return_address = new Tac(ILOC_ADDI, rpc, offset_retaddr, t1); // addI    rpc, offset_return_addr => t1
    save_return_address = new Tac(ILOC_STOREAI, t1, rsp, const_0);     // storeAI t1                      => rsp, 0

    // Generate code for jumping and returning
    jump_function = new Tac(ILOC_JUMPI, l1);                       // jumpI                           => l1
    get_return_val = new Tac(ILOC_LOADAI, rsp, offset_retval, t1); // loadAI  rsp, offset_return_val  => t1

    // Link code together
    calc_return_address->addLast(save_return_address);
    save_return_address->addLast(save_current_rsp);
    save_current_rsp->addLast(save_current_rfp);
    save_current_rfp->addLast(jump_function);
    jump_function->addLast(get_return_val);

    // Set this node's temp
    this->setTemp(t1);

    // Set this node as having a value ( The one returned by the function)
    this->setValue();

    // Return first instruction in the generated code block
    return calc_return_address;
}

Tac *Node::generateReturnTAC()
{
    // Instructions
    Tac *load_result = NULL;        // t0 <- return_expression
    Tac *save_return = NULL;        // Mem(rfp + offset_return) <- t0
    Tac *get_return_address = NULL; // t1 <- Mem(rfp + 0)
    Tac *get_old_rsp = NULL;        // t2 <- Mem(rfp + 4)
    Tac *get_old_rfp = NULL;        // t3 <- Mem(rfp + 8)
    Tac *update_rsp = NULL;         // rsp <- t2
    Tac *update_rfp = NULL;         // rfp <- t3
    Tac *jump_return = NULL;        // jump t1

    // Get current function symbol
    Symbol *current_func = Manager::getCurrentFunction();

    // Temporary registers
    std::string *t1 = this->getChild(0)->getTemp(); // Return value
    std::string *t2 = Tac::newRegister();           // Return address
    std::string *t3 = Tac::newRegister();           // Old rsp
    std::string *t4 = Tac::newRegister();           // Old rfp

    // Calculate size occupied by function parameters in frame
    int return_offset = 3 * getSize(TYPE_INT) + current_func->getParams().size() * getSize(TYPE_INT);

    // Names
    std::string *rfp = Tac::getName("rfp");
    std::string *rsp = Tac::getName("rsp");
    std::string *offset_return = Tac::getName(std::to_string(return_offset));
    std::string *const_0 = Tac::getName(std::to_string(0));
    std::string *const_4 = Tac::getName(std::to_string(4));
    std::string *const_8 = Tac::getName(std::to_string(8));

    // Get code from child expression
    load_result = this->getChild(0)->getCode(); // t1 <- ... expression ...

    // Generate code for the return statement
    save_return = new Tac(ILOC_STOREAI, t1, rfp, offset_return); // storeAI t1     => rfp, offset_return
    get_return_address = new Tac(ILOC_LOADAI, rfp, const_0, t2); // loadAI  rfp, 0 => t2
    get_old_rsp = new Tac(ILOC_LOADAI, rfp, const_4, t3);        // loadAI  rfp, 4 => t3
    get_old_rfp = new Tac(ILOC_LOADAI, rfp, const_8, t4);        // loadAi  rpf, 8 => t4
    update_rsp = new Tac(ILOC_I2I, t3, rsp);                     // i2i     t3     => rsp
    update_rfp = new Tac(ILOC_I2I, t4, rfp);                     // i2i     t4     => rfp
    jump_return = new Tac(ILOC_JUMP, t2);                        // jump           => t2

    // Link code together
    load_result->addLast(save_return);
    save_return->addLast(get_return_address);
    get_return_address->addLast(get_old_rsp);
    get_old_rsp->addLast(get_old_rfp);
    get_old_rfp->addLast(update_rsp);
    update_rsp->addLast(update_rfp);
    update_rfp->addLast(jump_return);

    // Return first instruction in the generated code block
    return load_result;
}

void Node::generateProgramStartTAC(int rsp_val, int rbss_val)
{
    // Setup
    Tac *start_rfp = NULL;  // rfp <- rsp_start
    Tac *start_rsp = NULL;  // rsp <- rsp_start
    Tac *start_rbss = NULL; // rbss <- rbss_start

    Tac *load_literal_i = NULL;  // For loading literal value into the temp register
    Tac *store_literal_i = NULL; // For storing literal value into the data segment

    // Function call to main
    Tac *calc_return_address = NULL; // t1 <- rpc + (3 + param_count + 2)
    Tac *save_return_address = NULL; // Mem (rsp + 0) <- t1
    Tac *save_current_rsp = NULL;    // Mem (rsp + 4) <- rsp
    Tac *save_current_rfp = NULL;    // Mem (rsp + 8) <- rfp
    Tac *jump_main = NULL;           // jumpI label_main

    // Return from main
    Tac *halt = NULL; // halt

    // Get main function symbol
    Symbol *main = Manager::getMain();

    std::string *t1 = Tac::newRegister(); // Temp reg for return address
    std::string *t2 = Tac::newRegister(); // Temp reg for loading literals

    // Get names for rbss and rsp initial values
    std::string *rsp_start = Tac::getName(std::to_string(rsp_val));
    std::string *rbss_start = Tac::getName(std::to_string(rbss_val));

    // Get names for registers
    std::string *rfp = Tac::getName("rfp");
    std::string *rsp = Tac::getName("rsp");
    std::string *rbss = Tac::getName("rbss");
    std::string *rpc = Tac::getName("rpc");
    std::string *const_0 = Tac::getName(std::to_string(0));
    std::string *const_4 = Tac::getName(std::to_string(4));
    std::string *const_5 = Tac::getName(std::to_string(5));
    std::string *const_8 = Tac::getName(std::to_string(8));

    // Literal value address offset in the data segment
    int literal_address = -1;
    std::string *lit_offset = NULL;
    std::string *lit_value = NULL;

    // Get main code label
    std::string *label_main = main->getLabel();

    // Generate code
    start_rfp = new Tac(ILOC_LOADI, rsp_start, rsp);    // loadI rsp_start => rsp
    start_rsp = new Tac(ILOC_LOADI, rsp_start, rfp);    // loadI rsp_start => rfp
    start_rbss = new Tac(ILOC_LOADI, rbss_start, rbss); // loadI rbss_start => rbss

    // Calculate the return address
    calc_return_address = new Tac(ILOC_ADDI, rpc, const_5, t1);    // addI    rpc, 5 => t1
    save_return_address = new Tac(ILOC_STOREAI, t1, rsp, const_0); // storeAI t1     => rsp, 0
    save_current_rsp = new Tac(ILOC_STOREAI, rsp, rsp, const_4);   // storeAI rsp    => rsp, 4
    save_current_rfp = new Tac(ILOC_STOREAI, rfp, rsp, const_8);   // storeAi rfp    => rsp, 8

    jump_main = new Tac(ILOC_JUMPI, label_main); // jumpI label_main
    halt = new Tac(ILOC_HALT);                   // halt

    // Get global symbol table
    SymbolTable *global_scope = Manager::getActiveSymbolTable();
    std::unordered_map<std::string, Symbol *> symbols = global_scope->getHashTable();

    // Generate code for loading literal values, if there are any
    for (auto i = symbols.begin(); i != symbols.end(); ++i)
    {
        // If literal value
        if (i->second->getCategory() == CAT_LITERAL)
        {
            // Get literal address and value
            literal_address = i->second->getAddress();
            lit_value = Tac::getName(i->second->getName());

            // Generate a name for the literal address
            lit_offset = Tac::getName(std::to_string(literal_address));

            // Generate code for storing this value in the data segment
            load_literal_i = new Tac(ILOC_LOADI, lit_value, t2);           // loadI   lit_val => t2
            store_literal_i = new Tac(ILOC_STOREAI, t2, rbss, lit_offset); // storeAI t2      => rbss , lit_offset

            // Link code together
            start_rbss->addLast(load_literal_i);
            load_literal_i->addLast(store_literal_i);
        }
    }

    // Link code together
    start_rfp->addLast(start_rsp);
    start_rsp->addLast(start_rbss);
    start_rbss->addLast(calc_return_address);

    calc_return_address->addLast(save_return_address);
    save_return_address->addLast(save_current_rsp);
    save_current_rsp->addLast(save_current_rfp);
    save_current_rfp->addLast(jump_main);

    jump_main->addLast(halt);

    // Add this node's code after the driver code
    halt->addLast(this->code);

    // Update this node's code to start at the driver code
    this->code = start_rfp;

    return;
}
