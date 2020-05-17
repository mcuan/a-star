#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <set>
#include <queue>


class Node {
  public:
    Node(uint x, uint y, uint nid)
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

    std::shared_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> adjNodes;

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

class GridMap {
  public:
    GridMap(uint rows, uint columns)
        : m_rows(rows)
        , m_columns(columns)
        , startNode(nullptr)
        , endNode(nullptr)
    {
        initGridNodes(getTotalNodes());

        printf("Creating grid map with %d nodes.\n", m_gridMap.size());
        printf("No. of rows: %d\n", getRows());
        printf("No. of cols: %d\n", getColumns());

    }

    uint getRows() { return m_rows; }
    uint getColumns() { return m_columns; }
    uint getTotalNodes() { return m_rows * m_columns; }
    std::vector<std::shared_ptr<Node>> getNodes() { return m_gridMap; }

    std::shared_ptr<Node> getStartNode() { return startNode; }
    std::shared_ptr<Node> getEndNode() { return endNode; }

    void initGridNodes(uint totalNodes) {
        for (uint nid = 0; nid < totalNodes; ++nid) {
            uint xpos = nid % m_columns;
            uint ypos = nid / m_columns;
            m_gridMap.push_back(std::make_shared<Node>(xpos, ypos, nid));
        }

        setStartNode(0);
        setEndNode(totalNodes - 1);
    }

    void setStartNode(int nodeId) {
        if (startNode != nullptr)
            startNode->clearFlags();

        m_gridMap[nodeId]->setStartNode(true);
        m_gridMap[nodeId]->setDistFromStart(0.0);
        startNode = m_gridMap[nodeId];
        
    }

    void setEndNode(int nodeId) {
        if (endNode != nullptr)
            endNode->clearFlags();

        m_gridMap[nodeId]->setEndNode(true);
        m_gridMap[nodeId]->setDistToEnd(0.0);
        endNode = m_gridMap[nodeId];
    }

    void updateGrid() {
        for (auto node : m_gridMap) {
            node->setVisited(false);
            node->parent = nullptr;
            node->adjNodes.clear();

            auto addAdjNode = [&](int x_adj, int y_adj) {
                int adj_nid = y_adj * m_columns + x_adj;

                if (!m_gridMap[adj_nid]->isObstacle()) {
                    node->adjNodes.emplace_back(m_gridMap[adj_nid]);
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

    void solvePath() {
        // (A*) using A-Star algorithm
        if (startNode == nullptr || endNode == nullptr)
            return;


        auto euclidean_dist = [](std::shared_ptr<Node> node_A, 
                                 std::shared_ptr<Node> node_B) 
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
            bool operator() (const std::shared_ptr<Node>& node_A,
                             const std::shared_ptr<Node>& node_B)
            {
                float heuristic_A = node_A->distFromStart() + node_A->distToEnd();
                float heuristic_B = node_B->distFromStart() + node_B->distToEnd();

                return (heuristic_A > heuristic_B);
            }
        };

        std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, pQueueCompare> pQueue;
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

                    pQueue.push(adj);
                }
            }
        }
        
        if (endNode->parent != nullptr) {
            printf("Solved path from star to end node!!!\n");
            auto current = endNode;

            while (current != nullptr) {
                printf("[%i] - ", current->nid());
                current = current->parent;
            }
            printf("\n");
        }
    }

  private:
    uint m_rows, m_columns;
    std::vector<std::shared_ptr<Node>> m_gridMap;
    std::shared_ptr<Node> startNode, endNode;
};

static constexpr uint GRID_ROWS = 20;
static constexpr uint GRID_COLS = 20;

class Simulation : public olc::PixelGameEngine
{
public:
    Simulation()
        : m_gridMap(GRID_ROWS, GRID_COLS)
    {
        sAppName = "Simulation";
    }

public:
    bool OnUserCreate() override
    {
        Clear(getPixelColor(BACKGROUND));
        DrawNodeGrid();
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        int selectedNodeX = GRID_COLS * GetMouseX() / ScreenWidth();
        int selectedNodeY = GRID_ROWS * GetMouseY() / ScreenHeight();

        if (GetMouse(0).bReleased) {
            int nid = selectedNodeY * GRID_COLS + selectedNodeX;
            printf("Selected Node : [%d]\n", nid);

            if      (GetKey(olc::Key::SHIFT).bHeld) { m_gridMap.setStartNode(nid); }
            else if (GetKey(olc::Key::CTRL ).bHeld) { m_gridMap.setEndNode(nid); }
            else {
                auto pNode = m_gridMap.getNodes()[nid];
                if (!pNode->isStartNode() && !pNode->isEndNode()) {
                    m_gridMap.getNodes()[nid]->toggleObstacle();
                } 
            }

            Clear(getPixelColor(BACKGROUND));
            m_gridMap.updateGrid();
            m_gridMap.solvePath();
            DrawNodeGrid();
        }


        return true;
    }

private:
    GridMap m_gridMap;

    enum ColorEnums {
        BACKGROUND     = 0x282828,
        NEUTRAL_NODE   = 0x076678,
        VISITED_NODE   = 0x83a598,
        OBSTACLE_NODE  = 0x928374,
        START_NODE     = 0x98971a,
        END_NODE       = 0xfb4934,
        ADJACENT_LINE  = 0x458588,
        PATH_LINE      = 0x98971a,
    };

    olc::Pixel getPixelColor(Simulation::ColorEnums color) {
        uint8_t red   = static_cast<uint8_t>(color >> UINT8_WIDTH * 2);
        uint8_t green = static_cast<uint8_t>(color >> UINT8_WIDTH * 1);
        uint8_t blue  = static_cast<uint8_t>(color >> UINT8_WIDTH * 0);

        // printf("RGB = [0x%02x, 0x%02x, 0x%02x]\n", red, green, blue);
        return olc::Pixel(red, green , blue);
    }

    int getX_pixelSpace(int x) {
        return static_cast<int>(ScreenWidth() * (x + 0.5) / GRID_COLS);
    }

    int getY_pixelSpace(int y) {
        return static_cast<int>(ScreenHeight() * (y + 0.5) / GRID_ROWS);
    }

    void DrawNodeGrid() {
        DrawNodeConnections();
        DrawShortestPath();
        DrawNodes();
    }

    void DrawNodeConnections() {
        for(auto node : m_gridMap.getNodes()) {
            int xpos = getX_pixelSpace(node->x());
            int ypos = getY_pixelSpace(node->y());

            for (auto adj : node->adjNodes) {
                int adj_xpos = getX_pixelSpace(adj->x());
                int adj_ypos = getY_pixelSpace(adj->y());

                DrawLine(xpos, ypos, adj_xpos, adj_ypos, getPixelColor(ADJACENT_LINE), 0xF0F0F0F0);
            }
        }
    }

    void DrawShortestPath() {
        auto current = m_gridMap.getEndNode();

        while (current->parent != nullptr) {
            int x_A = getX_pixelSpace(current->x());
            int y_A = getY_pixelSpace(current->y());

            int x_B = getX_pixelSpace(current->parent->x());
            int y_B = getY_pixelSpace(current->parent->y());

            DrawLine(x_A, y_A, x_B, y_B, getPixelColor(PATH_LINE));
            current = current->parent;
        }
    }

    void DrawNodes() {
        constexpr int nodeRadius = 4;

        for(auto node : m_gridMap.getNodes()) {
            int xpos = getX_pixelSpace(node->x());
            int ypos = getY_pixelSpace(node->y());

            olc::Pixel nodeColor = (node->isStartNode()) ? getPixelColor(START_NODE)
                                 : (node->isEndNode())   ? getPixelColor(END_NODE)
                                 : (node->isObstacle())  ? getPixelColor(OBSTACLE_NODE)
                                 : (node->isVisited())   ? getPixelColor(VISITED_NODE)
                                 : getPixelColor(NEUTRAL_NODE);

            FillCircle(xpos, ypos, nodeRadius, nodeColor);
        }
    }

};

int main()
{
    Simulation sim;
    if (sim.Construct(500, 500, 1, 1))
        sim.Start();

    return 0;
}
