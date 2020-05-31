#include <SFML/Graphics.hpp>
#include <cstring>
#include <math.h>

using namespace sf;

//Screen constants
const int SCREEN_WIDTH = 1000, SCREEN_HEIGHT = 1000;

//Map constants
const int M_WIDTH = 8, M_HEIGHT = 8;
const int TILE_SIZE = 32, fTILE_SIZE = 30.f;

//Player constants
const float PLAYER_INC = 0.25f;
const float PLAYER_FOV = M_PI/4;


class worldMap {
  private:
    int w_map[M_WIDTH][M_HEIGHT];

  public:
    worldMap(int m[M_HEIGHT][M_WIDTH]){
      memcpy(w_map, m, sizeof(int) * M_WIDTH * M_HEIGHT);
    }

    void render_2D(RenderWindow* window){
      
      RectangleShape blank_rect(Vector2f(fTILE_SIZE, fTILE_SIZE));
      blank_rect.setFillColor(Color::White);
      
      RectangleShape wall_rect(Vector2f(fTILE_SIZE, fTILE_SIZE));
      wall_rect.setFillColor(Color::Blue);

      for(int y = 0; y < M_HEIGHT; y++){
        for(int x = 0; x < M_WIDTH; x++){
          if(w_map[y][x] == 0){
              blank_rect.setPosition(x * TILE_SIZE, y * TILE_SIZE);
              window->draw(blank_rect);
          }
          else if(w_map[y][x] == 1){
              wall_rect.setPosition(x * TILE_SIZE, y * TILE_SIZE);
              window->draw(wall_rect);
          }
        }
      } 
    }
};


class Player {
  private:
    float x;
    float y;
    Vector2f view_vector;
    Vector2f camera_plane; 
  public:
    Player(float a, float b, Vector2f c){
      x = a;
      y = b;
      view_vector = c;
    }
    
    void renderGrid(RenderWindow *window){
      CircleShape shape(fTILE_SIZE/3);
      shape.setFillColor(Color::Green);
      shape.setPosition((x * TILE_SIZE) + TILE_SIZE/4, (y * TILE_SIZE) + TILE_SIZE/4);
      window->draw(shape);
    }

    void move(float dx, float dy){
      x += dx;
      y += dy;
   }
};


struct Ray{
  Vector2f position, direction;
};

int main(){
    
  //Set map
  int _m[M_HEIGHT][M_WIDTH] = {
      {1, 1, 1, 1, 1, 1, 1, 1},
      {1, 0, 0, 1, 1, 0, 1, 1},
      {1, 0, 0, 0, 1, 0, 0, 1},
      {1, 0, 0, 0, 0, 0, 0, 1},
      {1, 0, 1, 1, 1, 0, 0, 1},
      {1, 0, 0, 0, 1, 0, 0, 1},
      {1, 1, 0, 0, 0, 0, 0, 1},
      {1, 1, 1, 1, 1, 1, 1, 1},
    };

    worldMap main_map(_m); 

    //Set player
    Player m_player(1, 1, Vector2f(0.f, 0.f));
    
    RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Lets Try Raycasting");
    /* CircleShape shape(100.f);
    shape.setFillColor(Color::Green); */


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type){
              case Event::Closed:
                window.close();
                break;
              case Event::KeyPressed:
                if(Keyboard::isKeyPressed(Keyboard::Escape)) window.close();

                if(Keyboard::isKeyPressed(Keyboard::Left)){
                  m_player.move(-PLAYER_INC, 0);
                } else if (Keyboard::isKeyPressed(Keyboard::Right)){
                  m_player.move(PLAYER_INC, 0);
                } else if(Keyboard::isKeyPressed(Keyboard::Down)){
                  m_player.move(0, PLAYER_INC);
                } else if(Keyboard::isKeyPressed(Keyboard::Up)){
                  m_player.move(0, -PLAYER_INC);
                }

                break;
            }        
        }

        window.clear();
        
        main_map.render_2D(&window);
        
        m_player.renderGrid(&window);

        //Actual Raycasting
        struct Ray* r;
        for(int x = 0; x < SCREEN_WIDTH; x++){


        }

        window.display();
    }

    return 0;

}
