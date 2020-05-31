#include <SFML/Graphics.hpp>
#include <cstring>
#include <math.h>
#include <iostream>

using namespace sf;

//Screen constants
const int SCREEN_WIDTH = 1000, SCREEN_HEIGHT = 1000;

//Map constants
const int M_WIDTH = 8, M_HEIGHT = 8;
const int TILE_SIZE = 32, fTILE_SIZE = 30.f;

//Player constants
const float PLAYER_INC = 0.25f;
const float PLAYER_FOV = M_PI/4;
const float PLAYER_SIZE = 0.375f;
const float ROT_SPEED = 0.25f;

class worldMap {
  private:
    int w_map[M_WIDTH][M_HEIGHT];

  public:
    worldMap(int m[M_HEIGHT][M_WIDTH]){
      memcpy(w_map, m, sizeof(int) * M_WIDTH * M_HEIGHT);
    }

    void render_2D(RenderWindow* window){
      
      RectangleShape blank_rect(Vector2f(fTILE_SIZE, fTILE_SIZE));
      blank_rect.setFillColor(Color::Black);
      
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
    bool isWall(int x, int y){
      return w_map[y][x] > 0;
    }
    bool canMove(float x, float y){
     // x = (int) x, y = (int) y;
      std::cout << x << ", " << y << " gives " << isWall(x, y) << std::endl;
      return (x <= M_WIDTH - 1 && x >= 0) && (y <= M_HEIGHT - 1 && y >= 0) && isWall(x, y) == 0;
    }
};

class Entity {
  public:
    Vector2f position;
    Vector2f direction;
};

class Player : public Entity {
  public:
    Vector2f camera_plane; 

  public:
    Player(float a, float b, Vector2f c){
      position.x = a;
      position.y = b;
      direction = c;
      camera_plane.x = 0.f, camera_plane.y = 0.66f; 
    }
    
    void renderGrid(RenderWindow *window){
      //Draw player 'circle'
      CircleShape shape(TILE_SIZE/3);
      shape.setFillColor(Color::Green);
      //shape.setPosition((position.x * TILE_SIZE) + TLE_SIZE/4, (position.y * TILE_SIZE) + TILE_SIZE/4);
      shape.setPosition((position.x * TILE_SIZE) , (position.y * TILE_SIZE));
      window->draw(shape);
      
     /*  //Draw view line
      Vertex line[] = {Vertex(position), Vertex(position) + Vertex(view_vector)};
      window->draw(line, 2, Lines); */
    }

    void move(float dx, float dy){
      position.x += dx;
      position.y += dy;
    }
     
};


Vector2f rotateVec2f(sf::Vector2f vec, float value) {
    return Vector2f(
            vec.x * std::cos(value) - vec.y * std::sin(value),
            vec.x * std::sin(value) + vec.y * std::cos(value)
    );
}

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

    bool render_map = true;

    worldMap main_map(_m); 

    //Set player
    Player m_player(1, 1, Vector2f(-1.f, 0.f));
    
    RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Lets Try Raycasting");
    /* CircleShape shape(100.f);
    shape.setFillColor(Color::Green); */

    VertexArray lines(Lines, 18 * SCREEN_WIDTH);

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

                if(Keyboard::isKeyPressed(Keyboard::Left) && main_map.canMove(m_player.position.x - PLAYER_INC, m_player.position.y)){
                  m_player.move(-PLAYER_INC, 0);
                } else if (Keyboard::isKeyPressed(Keyboard::Right) && main_map.canMove(m_player.position.x + PLAYER_INC, m_player.position.y)){
                  m_player.move(PLAYER_INC, 0);
                } else if(Keyboard::isKeyPressed(Keyboard::Down) && main_map.canMove(m_player.position.x, m_player.position.y + PLAYER_INC)){
                  m_player.move(0, PLAYER_INC);
                } else if(Keyboard::isKeyPressed(Keyboard::Up) && main_map.canMove(m_player.position.x, m_player.position.y - PLAYER_INC)){
                  m_player.move(0, -PLAYER_INC);
                } else if(Keyboard::isKeyPressed(Keyboard::M)){
                    render_map = !render_map;
                }

                break;
            }        
        }

        window.clear();
        
        if(render_map){
          main_map.render_2D(&window);        
          m_player.renderGrid(&window);
        }
        //Actual Raycasting
        Entity ray;
        ray.position = m_player.position; 
        ray.direction= m_player.direction;

        for(int x = 0; x < SCREEN_WIDTH; x++){
            float cameraX = 2 * m_player.position.x / double(SCREEN_WIDTH) - 1;
            ray.direction = m_player.direction + m_player.camera_plane * cameraX;
            
            Vector2i map_pos(ray.position);
            
            Vector2f sideDist;
            Vector2i step;
            Vector2f deltaDist(std::abs(1/ray.direction.x), std::abs(1/ray.direction.y));

            float perpWallDist;
            int wallHeight, ceilingPixel = 0, groundPixel = SCREEN_HEIGHT;
            
            bool hit;
            bool horizontal;
          
            if(ray.direction.x < 0.f){
                step.x = -1;
                sideDist.x = (m_player.position.x - map_pos.x) * deltaDist.x;
            } else { 
                step.x = 1;
                sideDist.x = (m_player.position.x + 1.0 - map_pos.x) * deltaDist.x;
            }

            if(ray.direction.y < 0.f){
                step.y = -1;
                sideDist.y = (m_player.position.y - map_pos.y) * deltaDist.y;
            } else { 
                step.x = 1;
                sideDist.y = (m_player.position.y + 1.0 - map_pos.y) * deltaDist.y;
            }
           
            while(!hit){
              if(sideDist.x < sideDist.y){
                sideDist.x += deltaDist.x;
                map_pos.x += step.x;
                horizontal = false;
              } else {
                sideDist.y += deltaDist.y;
                map_pos.y += step.y;
                horizontal = true;
              }

              if(main_map.isWall(map_pos.x, map_pos.y) > 0) hit = true;
            }


            if(horizontal) {
              perpWallDist = (map_pos.y - m_player.position.y + (1 - step.y)/2) / ray.direction.y;
            } else {
              perpWallDist = (map_pos.x - m_player.position.x + (1 - step.x)/2) / ray.direction.x;
            }





        }
        

        window.display();
    }

    return 0;

}
