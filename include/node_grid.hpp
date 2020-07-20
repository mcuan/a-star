#ifndef A_STAR_NODE_GRID_HPP
#define A_STAR_NODE_GRID_HPP

#include "node.hpp"
#include <deque>

class NodeGrid {
public:
    NodeGrid(int rows, int columns);
    ~NodeGrid();

    int getRows();
    int getColumns();
    int getTotalNodes();
    std::vector<NodePtr> getNodes();
    std::vector<NodePtr> getShortestPath();
    std::deque<NodePtr> getVisitedNodes();
    void popFrontVisitedNode();

    NodePtr getStartNode();
    NodePtr getEndNode();

    void initGridNodes(int totalNodes);
    void setStartNode(int nodeId);
    void setEndNode(int nodeId);

    void updateNodeAdjacency();
    void solvePath();

private:
    int m_rows, m_columns;
    std::vector<NodePtr> m_NodeGrid, m_shortestPath;
    std::deque<NodePtr> m_visitedNodes;
    NodePtr startNode, endNode;
};

#endif /* A_STAR_NODE_GRID_HPP */
