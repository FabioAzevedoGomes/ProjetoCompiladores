#include "Graph.h"

Graph::Graph()
{
    this->edge_count = 0;
    this->vertex_count = 0;
    this->used_colors_counter = 0;
}

Graph::Graph(Graph &graph)
{
    this->V = graph.V;
    this->E = graph.E;
    this->C = graph.C;

    this->edge_count = graph.edge_count;
    this->vertex_count = graph.vertex_count;
    this->used_colors_counter = graph.used_colors_counter;
}

Graph::~Graph()
{
    // nil
}

void Graph::addVertex(std::string *name)
{
    // Create new vertex
    Vertex v(name, this->vertex_count);

    // Create a new color
    Color c(this->vertex_count);

    // Add to vertex map
    this->vertex_ids.insert(std::make_pair(name, this->vertex_count));

    // Increase vertex count
    this->vertex_count++;

    // Add to vertex list
    this->V.push_back(v);

    // Add an available color
    this->C.push_back(c);
}

void Graph::addEdge(std::string *end_1, std::string *end_2)
{
    // Get id for both vertexes
    int v1 = this->vertex_ids[end_1];
    int v2 = this->vertex_ids[end_2];

    // Create edge with both vertexes
    Edge e(this->edge_count, 0, &this->V[v1], &this->V[v2]);

    // Increase edge counter
    this->edge_count++;

    // Add as adjacent to each other
    this->V[v1].setAdjacent(v2);
    this->V[v2].setAdjacent(v1);

    // Add edge to list
    this->E.push_back(e);
}

uint Graph::getVertexCount()
{
    return this->vertex_count;
}

uint Graph::getEdgeCount()
{
    return this->edge_count;
}

uint Graph::getUsedColorCount()
{
    return this->used_colors_counter;
}

std::vector<Vertex> Graph::getVertexes()
{
    return this->V;
}

std::vector<Color> Graph::getColors()
{
    return this->C;
}

std::vector<Color> Graph::getUsedColors()
{
    std::vector<Color> new_vector;

    for (auto i = this->C.begin(); i != this->C.end(); ++i)
    {
        if ((*i).getUsageCounter() > 0)
            new_vector.push_back(*i);
    }

    return new_vector;
}

std::vector<Edge> Graph::getEdges()
{
    return this->E;
}

Vertex *Graph::getVertex(unsigned int vertex_id)
{
    return &this->V[vertex_id];
}

Color *Graph::getColor(unsigned int color_id)
{
    return &this->C[color_id];
}

void Graph::colorVertex(unsigned int vertex_id, unsigned int color_id)
{
    if (this->C[color_id].getUsageCounter() == 0)
        this->used_colors_counter++;

    this->V[vertex_id].setColor(color_id);

    this->C[color_id].addUser(vertex_id);
}

void Graph::cleanVertex(unsigned int vertex_id)
{
    unsigned int color_id = this->V[vertex_id].getColor();

    this->C[color_id].removeUser(vertex_id);

    this->V[vertex_id].setColor(-1);

    if (this->C[color_id].getUsageCounter() == 0)
        this->used_colors_counter--;
}

bool Graph::canUse(unsigned int vertex_id, unsigned int color_id)
{
    bool can_use = true;

    std::vector<unsigned int> adjacent = this->V[vertex_id].getAdjacent();

    for (auto i = adjacent.begin(); can_use && i != adjacent.end(); ++i)
        if (this->V[*i].getColor() == color_id)
            can_use = false;

    return can_use;
}

std::string Graph::outputInfo()
{
    std::stringstream info;

    info << "Vertex count: " << this->vertex_count << std::endl
         << "Edge count: " << this->edge_count << std::endl
         << "Color count: " << this->getUsedColorCount() << std::endl;

    for (auto i = this->V.begin(); i != this->V.end(); ++i)
    {
        info << "Vertex " << *i->getName() << " given color " << this->C[i->getColor()].getId() << std::endl;
    }

    info << std::endl;

    return info.str();
}

bool Graph::minimizeRegisterUsage(int k)
{
    if (this->getVertexCount() > 0)
    {
        // Set with every vertex id
        std::vector<unsigned int> vertex_ids;
        for (unsigned int i = 0; i < this->getVertexCount(); ++i)
            vertex_ids.push_back(i);

        // Randomize a vertex
        unsigned int v_i = random() % vertex_ids.size();

        // Give color 0 to vertex
        this->colorVertex(v_i, 0);

        // Remove vertex from list
        vertex_ids.erase(vertex_ids.begin() + v_i);

        // Iterate remaining vertexes
        for (unsigned int i = 0; i < this->getVertexCount() - 1; ++i)
        {
            // Sample random vertex
            v_i = random() % vertex_ids.size();

            // Iterate colors
            for (unsigned int j = 0; j < this->getVertexCount(); ++j)
            {
                // If vertex can use color
                if (this->canUse(vertex_ids[v_i], j))
                {
                    // Give color to vertex
                    this->colorVertex(vertex_ids[v_i], j);

                    // Exit color looping
                    j = this->getVertexCount();
                }
            }

            // Remove vertex from list
            vertex_ids.erase(vertex_ids.begin() + v_i);
        }
    }
    return this->getUsedColorCount() <= (unsigned int)k;
}

std::map<std::string *, std::string> Graph::getColorMapping()
{
    std::map<std::string *, std::string> translation;

    for (auto i = this->V.begin(); i != this->V.end(); ++i)
    {
        // Make register name
        std::stringstream regname;
        regname << "\%r" << X86_64_REGCOUNT + i->getColor();

        // Add to translation map
        translation.insert(std::make_pair(i->getName(), regname.str()));
    }

    return translation;
}
