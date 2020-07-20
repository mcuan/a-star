#include "game_engine.hpp"

static constexpr int NUM_OF_ARGS = 3;

int main(int argc, char* argv[]) {

    if (argc < NUM_OF_ARGS) {
        std::cout << "Please specify number of rows and columns"
            << "\nUsage: a-star ROWS COLS\n";
            return -1;
    }

    int rows = std::stoi(argv[1]);
    int cols = std::stoi(argv[2]);

    int px_height = rows*SCALE_FACTOR;
    int px_width = cols*SCALE_FACTOR;

    GameEngine game(rows, cols);
    
    if (game.Construct(px_width, px_height, PIXEL_SIZE, PIXEL_SIZE)) {
        std::cout << "Creating " << px_width*PIXEL_SIZE << "x" << px_height*PIXEL_SIZE << " px window" << std::endl;
        game.Start();
    }

    return 0;
}
