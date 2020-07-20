#ifndef A_STAR_GAME_ENGINE_HPP
#define A_STAR_GAME_ENGINE_HPP

#include "olcPixelGameEngine.h"
#include "node_grid.hpp"

static constexpr int PIXEL_SIZE = 1;
static constexpr int SCALE_FACTOR = 10;

class GameEngine : public olc::PixelGameEngine
{
public:
    GameEngine(int rows, int cols);
    ~GameEngine();

public:
    bool OnUserCreate() override;
    bool OnUserUpdate(float fElapsedTime) override;
    
private:
    int m_rows, m_cols;
    NodeGrid m_NodeGrid;

    enum ColorEnums {
        BACKGROUND     = 0x000000,
        NEUTRAL_NODE   = 0x002200,
        VISITED_NODE   = 0x508050,
        OBSTACLE_NODE  = 0x20207F,
        START_NODE     = 0x00FF00,
        END_NODE       = 0xFF0000,
        PATH_LINE      = 0x00FFFF,
    };

    olc::Pixel getPixelColor(GameEngine::ColorEnums color);
    
    int getX_pixelSpace(int x);
    int getY_pixelSpace(int y);

    void DrawNodeGrid();
    void DrawNodeConnections();
    void DrawShortestPath();
    void DrawNodes();
    void DrawVisitedNode(const NodePtr& nodePtr);

    bool isLargerThanFPS = false;
    int getStdDistVal(int x, int mean, float std_dev, int size);

    int m_totalFrames, m_frameCount;
    NodePtr m_nodePtr;
};

#endif /* A_STAR_GAME_ENGINE_HPP */
