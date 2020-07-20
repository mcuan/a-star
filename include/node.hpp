#ifndef A_STAR_NODE_HPP
#define A_STAR_NODE_HPP

#include <vector>
#include <memory>
#include <cmath>

class Node;

typedef std::shared_ptr<Node> NodePtr;

class Node {
public:
    Node(int x, int y, int nid)
        : m_x(x), m_y(y)
        , m_nid(nid)
        , m_isObstacle(false)
        , m_isVisited(false)
        , m_isStartNode(false)
        , m_isEndNode(false)
        , parent(nullptr)
    {
        // printf("Creating new Node[x,y] = [%d, %d]\n", m_x, m_y);
    }

    NodePtr parent;
    std::vector<NodePtr> adjNodes;

    int x()   { return m_x;   }
    int y()   { return m_y;   }
    int nid() { return m_nid; }

    bool isObstacle()  { return m_isObstacle;  }
    bool isVisited()   { return m_isVisited;   }
    bool isStartNode() { return m_isStartNode; }
    bool isEndNode()   { return m_isEndNode;   }

    void toggleObstacle()         { m_isObstacle  = !m_isObstacle; }
    void setVisited(bool _flag)   { m_isVisited   = _flag; }
    void setStartNode(bool _flag) { m_isStartNode = _flag; }
    void setEndNode(bool _flag)   { m_isEndNode   = _flag; }

    void setDistFromStart(float dist) { m_distFromStart = dist; }
    float distFromStart() { return m_distFromStart; }

    void setDistToEnd(float dist) { m_distToEnd = dist; }
    float distToEnd() {  return m_distToEnd; }

    void clearFlags() {
        m_isObstacle  = false;
        m_isVisited   = false;
        m_isStartNode = false;
        m_isEndNode   = false;
    }

private:
    int m_x;
    int m_y;
    int m_nid;

    bool m_isObstacle;
    bool m_isVisited;
    bool m_isStartNode;
    bool m_isEndNode;

    float m_distFromStart = INFINITY;
    float m_distToEnd = INFINITY;
};

#endif /* A_STAR_NODE_HPP */