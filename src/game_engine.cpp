
#include "game_engine.hpp"

GameEngine::GameEngine(int rows, int cols)
    : m_rows(rows)
    , m_cols(cols)
    , m_NodeGrid(rows, cols)
    , m_totalFrames(0)
    , m_frameCount(0)
    , m_nodePtr(nullptr)
{
    // Game Engine Consttructor
    sAppName = "A* algorithm demo";
}

GameEngine::~GameEngine() {
    // Game Engine Destructor
}

bool GameEngine::OnUserCreate() {
    Clear(getPixelColor(BACKGROUND));
    DrawNodeGrid();
    return true;
}

bool GameEngine::OnUserUpdate(float fElapsedTime) {
    int selectedNodeX = m_cols * GetMouseX() / ScreenWidth();
    int selectedNodeY = m_rows * GetMouseY() / ScreenHeight();

    if (GetMouse(0).bReleased) {
        int nid = selectedNodeY * m_cols + selectedNodeX;
        printf("Selected Node : [%d]\n", nid);

        if      (GetKey(olc::Key::SHIFT).bHeld) { m_NodeGrid.setStartNode(nid); }
        else if (GetKey(olc::Key::CTRL ).bHeld) { m_NodeGrid.setEndNode(nid); }
        else {
            auto pNode = m_NodeGrid.getNodes()[nid];
            if (!pNode->isStartNode() && !pNode->isEndNode()) {
                m_NodeGrid.getNodes()[nid]->toggleObstacle();
            } 
        }

        Clear(getPixelColor(BACKGROUND));
        m_NodeGrid.updateNodeAdjacency();
        // m_NodeGrid.solvePath();
        DrawNodeGrid();
        m_nodePtr = nullptr;
    }

    if (GetKey(olc::Key::SPACE).bReleased) {
        Clear(getPixelColor(BACKGROUND));
        m_NodeGrid.updateNodeAdjacency();
        DrawNodeGrid();

        m_NodeGrid.solvePath();
        m_nodePtr = m_NodeGrid.getVisitedNodes().front();

        m_frameCount = 1;
        m_totalFrames = m_NodeGrid.getVisitedNodes().size();
        isLargerThanFPS = (m_totalFrames > GetFPS()) ? true : false;
        // DrawNodeGrid();
    }

    if (GetKey(olc::Key::R).bReleased) {
        Clear(getPixelColor(BACKGROUND));
        m_NodeGrid.randomizeObstacles();
        m_NodeGrid.updateNodeAdjacency();
        DrawNodeGrid();
    }

    if (m_nodePtr != nullptr && !m_NodeGrid.getVisitedNodes().empty()) {
        // printf("Drawing visited node [%d]\n", m_nodePtr->nid());
        if (isLargerThanFPS) {
            for (int fr = 0; fr < getStdDistVal(m_frameCount, 30, 11, m_totalFrames); ++fr) {
                if (m_nodePtr != nullptr && !m_NodeGrid.getVisitedNodes().empty()) {
                    if (m_nodePtr == m_NodeGrid.getEndNode()) {
                        // DrawNodeGrid();
                        DrawShortestPath();
                    }
                    else {
                        DrawVisitedNode(m_nodePtr);
                        m_NodeGrid.popFrontVisitedNode();
                        m_nodePtr = m_NodeGrid.getVisitedNodes().front();
                    }
                }
            }
        } 
        else {
            DrawVisitedNode(m_nodePtr);
            m_NodeGrid.popFrontVisitedNode();
            m_nodePtr = m_NodeGrid.getVisitedNodes().front();
            

            if (m_nodePtr == m_NodeGrid.getEndNode()) {
                // DrawNodeGrid();
                DrawShortestPath();
            }
        }

        ++m_frameCount;
    }

    return true;
}

int GameEngine::getStdDistVal(int x, int mean, float std_dev, int size) {
    float pref = 1.0 / (std_dev * std::sqrt(2 * M_PI));
    int mean_diff = x - mean;
    float coeff = static_cast<float>((mean_diff)*(mean_diff)) / (2.0 * std_dev * std_dev);

    float std_dist = pref * std::exp(-coeff);
    float block_size = size * std_dist;

    int ret_val = 1;

    if (block_size > 1) {
        ret_val = static_cast<int>(std::round(block_size));
    }

    return ret_val;
}

void GameEngine::DrawVisitedNode(const NodePtr& nodePtr) {
    constexpr int nodeRadius = 2;

    // Visited Node center x,y coordinates
    int visit_x = getX_pixelSpace(nodePtr->x());
    int visit_y = getY_pixelSpace(nodePtr->y());
    
    // Parent Node center x,y coordinates
    int parent_x = getX_pixelSpace(nodePtr->parent->x());
    int parent_y = getY_pixelSpace(nodePtr->parent->y());


    DrawLine(visit_x, visit_y, parent_x, parent_y, getPixelColor(VISITED_NODE));
    FillCircle(visit_x, visit_y, nodeRadius, getPixelColor(VISITED_NODE));
}

olc::Pixel GameEngine::getPixelColor(GameEngine::ColorEnums color) {
    uint8_t red   = static_cast<uint8_t>(color >> UINT8_WIDTH * 2);
    uint8_t green = static_cast<uint8_t>(color >> UINT8_WIDTH * 1);
    uint8_t blue  = static_cast<uint8_t>(color >> UINT8_WIDTH * 0);

    // printf("RGB = [0x%02x, 0x%02x, 0x%02x]\n", red, green, blue);
    return olc::Pixel(red, green , blue);
}

int GameEngine::getX_pixelSpace(int x) {
    return static_cast<int>(ScreenWidth() * (x + 0.5) / m_cols);
}

int GameEngine::getY_pixelSpace(int y) {
    return static_cast<int>(ScreenHeight() * (y + 0.5) / m_rows);
}

void GameEngine::DrawNodeGrid() {
    DrawNodeConnections();
    // DrawShortestPath();
    DrawNodes();
}

void GameEngine::DrawNodeConnections() {
    for(auto node : m_NodeGrid.getNodes()) {
        int xpos = getX_pixelSpace(node->x());
        int ypos = getY_pixelSpace(node->y());

        for (auto adj : node->adjNodes) {
            int adj_xpos = getX_pixelSpace(adj->x());
            int adj_ypos = getY_pixelSpace(adj->y());

            olc::Pixel line_color = adj->isVisited() ? getPixelColor(VISITED_NODE) : getPixelColor(NEUTRAL_NODE);
            DrawLine(xpos, ypos, adj_xpos, adj_ypos, line_color);
        }
    }
}

void GameEngine::DrawShortestPath() {
    auto current = m_NodeGrid.getEndNode();

    while (current->parent != nullptr) {
        int x_A = getX_pixelSpace(current->x());
        int y_A = getY_pixelSpace(current->y());

        int x_B = getX_pixelSpace(current->parent->x());
        int y_B = getY_pixelSpace(current->parent->y());

        DrawLine(x_A, y_A, x_B, y_B, getPixelColor(PATH_LINE));
        current = current->parent;
    }
}

void GameEngine::DrawNodes() {
    constexpr int nodeRadius = 2;

    for(auto node : m_NodeGrid.getNodes()) {
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
