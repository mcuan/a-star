#include "node_grid.hpp"
#include <iostream>
#include <queue>

NodeGrid::NodeGrid(int rows, int columns)
    : m_rows(rows)
    , m_columns(columns)
    , startNode(nullptr)
    , endNode(nullptr)
{
    // Grid Map Constructor
    initGridNodes(getTotalNodes());

    printf("Creating grid map with %zu nodes.\n", m_NodeGrid.size());
    printf("No. of rows: %d\n", getRows());
    printf("No. of cols: %d\n", getColumns());

}

NodeGrid::~NodeGrid() {
    // Grid Map Destructor
}

int NodeGrid::getRows() {
    return m_rows;
}

int NodeGrid::getColumns() {
    return m_columns;
}

int NodeGrid::getTotalNodes() {
    return m_rows * m_columns;
}

std::vector<NodePtr> NodeGrid::getNodes() {
    return m_NodeGrid;
}

std::vector<NodePtr> NodeGrid::getShortestPath() {
    return m_shortestPath;
}

std::deque<NodePtr> NodeGrid::getVisitedNodes() {
    return m_visitedNodes;
}

void NodeGrid::popFrontVisitedNode() {
    m_visitedNodes.pop_front();
}

NodePtr NodeGrid::getStartNode() {
    return startNode;
}

NodePtr NodeGrid::getEndNode() {
    return endNode;
}

void NodeGrid::initGridNodes(int totalNodes) {
    for (int nid = 0; nid < totalNodes; ++nid) {
        int xpos = nid % m_columns;
        int ypos = nid / m_columns;
        m_NodeGrid.push_back(std::make_shared<Node>(xpos, ypos, nid));
    }

    setStartNode(0);
    setEndNode(totalNodes - 1);
}

void NodeGrid::randomizeObstacles() {
    std::bernoulli_distribution distribution(0.5);
    
    for (auto node_ptr : m_NodeGrid) {
        if (distribution(generator)) {
            node_ptr->toggleObstacle();
        }
    }
}

void NodeGrid::setStartNode(int nodeId) {
    if (startNode != nullptr)
        startNode->clearFlags();

    m_NodeGrid[nodeId]->setStartNode(true);
    m_NodeGrid[nodeId]->setDistFromStart(0.0);
    startNode = m_NodeGrid[nodeId];
}

void NodeGrid::setEndNode(int nodeId) {
    if (endNode != nullptr)
        endNode->clearFlags();

    m_NodeGrid[nodeId]->setEndNode(true);
    m_NodeGrid[nodeId]->setDistToEnd(0.0);
    endNode = m_NodeGrid[nodeId];
}

void NodeGrid::updateNodeAdjacency() {
    m_shortestPath.clear();
    m_visitedNodes.clear();
    
    for (auto node : m_NodeGrid) {
        node->setVisited(false);
        node->parent = nullptr;
        node->adjNodes.clear();

        auto addAdjNode = [&](int x_adj, int y_adj) {
            int adj_nid = y_adj * m_columns + x_adj;

            if (!m_NodeGrid[adj_nid]->isObstacle()) {
                node->adjNodes.emplace_back(m_NodeGrid[adj_nid]);
            }
        };

        if (!node->isObstacle()) {
            // if x > 0 check left
            if(node->x() > 0) {
                addAdjNode(node->x() - 1, node->y());
                // if y > 0 check top left corner
                if(node->y() > 0)  {
                    addAdjNode(node->x() - 1, node->y() - 1);
                }
                // if y < N check bottom left corner
                if(node->y() < m_rows - 1)  {
                    addAdjNode(node->x() - 1, node->y() + 1);
                }
            }
            // if x < N check right
            if(node->x() < m_columns - 1) {
                addAdjNode(node->x() + 1, node->y());
                // if y > 0 check top right corner
                if(node->y() > 0)  {
                    addAdjNode(node->x() + 1, node->y() - 1);
                }
                // if y < N check bottom right corner
                if(node->y() < m_rows - 1)  {
                    addAdjNode(node->x() + 1, node->y() + 1);
                }
            }
            // if y > 0 check top
            if(node->y() > 0) {
                addAdjNode(node->x(), node->y() - 1);
                
            }
            // if y < N check bottom
            if(node->y() < m_rows - 1) {
                addAdjNode(node->x(), node->y() + 1);
            }
        }

    }
}

void NodeGrid::solvePath() {
    // (A*) using A-Star algorithm
    if (startNode == nullptr || endNode == nullptr)
        return;


    auto euclidean_dist = [](NodePtr node_A, 
                                NodePtr node_B) 
    {
        float x_A = static_cast<float>(node_A->x());
        float y_A = static_cast<float>(node_A->y());
        float x_B = static_cast<float>(node_B->x());
        float y_B = static_cast<float>(node_B->y());

        float xx = x_A - x_B;
        float yy = y_A - y_B;

        return std::sqrt(xx * xx + yy * yy);
    };

    class pQueueCompare {
        public:
        bool operator() (const NodePtr& node_A,
                            const NodePtr& node_B)
        {
            float heuristic_A = node_A->distFromStart() + node_A->distToEnd();
            float heuristic_B = node_B->distFromStart() + node_B->distToEnd();

            return (heuristic_A > heuristic_B);
        }
    };

    std::priority_queue<NodePtr, std::vector<NodePtr>, pQueueCompare> pQueue;
    startNode->setVisited(true);
    pQueue.push(startNode);

    while (pQueue.top() != endNode && !pQueue.empty()) {
        auto current = pQueue.top();

        if (current->isVisited())
            pQueue.pop();

        for (auto adj : current->adjNodes) {
            if (!adj->isVisited()) {
                adj->parent = current;
                adj->setDistFromStart(current->distFromStart() + euclidean_dist(current, adj));
                adj->setDistToEnd(euclidean_dist(adj, endNode));
                adj->setVisited(true);
                m_visitedNodes.push_back(adj);

                pQueue.push(adj);
            }
        }
    }
    
    if (endNode->parent != nullptr) {
        printf("Solved path from start to end node!!!\n");
        auto current = endNode;

        while (current != nullptr) {
            printf("[%i] - ", current->nid());
            m_shortestPath.push_back(current);
            current = current->parent;
        }
        printf("\n");
    }
}