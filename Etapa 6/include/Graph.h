/**
 * Graph class which models efficient data structures for consulting node 
 * adjacency and colors.
 * 
 * Author: Fábio de Azevedo Gomes 
 */
#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>

#include "Color.h"
#include "Vertex.h"
#include "Edge.h"
#include "Type.h"

class Graph
{
private:
    // Information
    unsigned int vertex_count;
    unsigned int edge_count;
    unsigned int used_colors_counter;

    // Map vertex name to ID
    std::map<std::string *, int> vertex_ids;

    // Vertex set V
    std::vector<Vertex> V;

    // Edge set E
    std::vector<Edge> E;

    // Color set C
    std::vector<Color> C;

public:
    // CONSTRUCTOR AND DESTRUCTOR

    /**
     * @brief Constructs an empty graph 
     */
    Graph();

    /**
     * @brief Copy constructor 
     */
    Graph(Graph &graph);

    /**
     * @brief Graph destructor 
     */
    ~Graph();

    // BUILDERS

    /**
     * @brief Adds a new vertex to the graph 
     */
    void addVertex(std::string *name);

    /**
     * @brief Adds a new edge to the graph, connecting two vertexes 
     */
    void addEdge(std::string *end_1, std::string *end_2);

    // GETTERS

    /**
     * @brief Returns number of vertices 
     */
    unsigned int getVertexCount();

    /**
     * @brief Returns number of edges 
     */
    unsigned int getEdgeCount();

    /**
     * @brief Returns the number of colors
     * that were used in the coloring 
     */
    unsigned int getUsedColorCount();

    /**
     * @brief Returns the list of vertexes for this graph 
     */
    std::vector<Vertex> getVertexes();

    /**
     * @brief Returns the list of colors for this graph 
     */
    std::vector<Color> getColors();

    /**
     * @brief Returns the list of colors that are used for this graph 
     */
    std::vector<Color> getUsedColors();

    /**
     * @brief Returns this list of edges for this graph 
     */
    std::vector<Edge> getEdges();

    /**
     * @brief Returns a pointer to indexed vertex
     * @param vertex_id Identifier for the wanted vertex
     * @returns Pointer to indexed vertex 
     */
    Vertex *getVertex(unsigned int vertex_id);

    /**
     * @brief Returns a pointer to indexed color
     * @param color_id Identifier for the color
     * @returns Pointer to the indexed color
     */
    Color *getColor(unsigned int color_id);

    // SETTERS

    /**
     * @brief Gives color to vertex
     * @param vertex_id The vertex
     * @param color_id  The color
     */
    void colorVertex(unsigned int vertex, unsigned int color);

    /**
     * @brief Removes color from vertex 
     * @param vertex_id
     */
    void cleanVertex(unsigned int vertex_id);

    // INFORMATION

    /**
     * @brief Checks if vertex can use the color 
     */
    bool canUse(unsigned int vertex_id, unsigned int color_id);

    /**
     * @brief Returns general information about this graph 
     */
    std::string outputInfo();

    // COMPILER LOGIC

    /**
     * @brief Attempts to minimize the amount of "colors" (AKA
     * registers) necessary for the graph, by using a K-coloring
     * heuristic 
     * @param k Maximum number of registers
     * @returns True if was able to minimize register usage within given amount, false otherwise
     */
    bool minimizeRegisterUsage(int k);

    /**
     * @brief Returns the color that was given to each vertex 
     */
    std::map<std::string *, std::string> getColorMapping();
};

#endif
