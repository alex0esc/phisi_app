#include "phisi_app.hpp"

int screen_index = 0;
int sim_pixel_ratio = 2;

int main(int argc, char* argv[]) {        
    if(argc == 3) {
        std::string str = argv[1];
        screen_index = std::stoi(str);
        str = argv[2];
        sim_pixel_ratio = std::stoi(str);        
    }
    phisi_app::Application app;
    app.init();
    app.run();
    return 0;
}
