#include "Node.h"

Node::Node(Token *lexval_, Type type_, Statement statement_)
{
    this->lexval = lexval_;
    this->type = type_;
    this->statement = statement_;
    this->next_cmd = NULL;
    this->next_elem = NULL;
    this->code = NULL;

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
