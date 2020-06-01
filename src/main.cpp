#include <SFML/Graphics.hpp>
#include <cstring>
#include <math.h>
#include <iostream>
#include <stdint.h>

using namespace sf;

//Screen constants
const int SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 720;

//Map constants
const int M_WIDTH = 8, M_HEIGHT = 8;
const int TILE_SIZE = 32, fTILE_SIZE = 30.f;

//Grid stuff
const float LAMBDA = 300.f;

//Player constants
const float PLAYER_SPEED = 5.f;
const float PLAYER_FOV = M_PI/4;
const float PLAYER_SIZE = 0.375f;
const float ROT_SPEED = 3.f;
const float CAMERA_HEIGHT = 0.66f;

//Helpers
Vector2f rotateVec2f(sf::Vector2f vec, float value) {
    return Vector2f(
            vec.x * std::cos(value) - vec.y * std::sin(value),
            vec.x * std::sin(value) + vec.y * std::cos(value)
    );
}

Color halfColor(Color c){
  c.r /= 2, c.b /=2, c.g /=2;
  return c;
}
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
      wall_rect.setFillColor(Color::White);

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
      if((x <= M_WIDTH - 1 && x >= 0) && (y <= M_HEIGHT - 1 && y >= 0)) return w_map[y][x] != 0;
      return false;
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
      camera_plane = rotateVec2f(direction, -M_PI/2);
     }
    
    void renderGrid(RenderWindow *window){
      //Draw player 'circle'
      CircleShape shape(TILE_SIZE/3);
      shape.setFillColor(Color::Green);
      //shape.setPosition((position.x * TILE_SIZE) + TLE_SIZE/4, (position.y * TILE_SIZE) + TILE_SIZE/4);
     
      //Draw view line
      Vertex line[] = {Vertex(Vector2f(position.x * TILE_SIZE, position.y * TILE_SIZE)), Vertex(position + direction * LAMBDA)};
      window->draw(line, 2, Lines); 
     
      //Draw camera line
      /* Vertex line2[] = {Vertex(Vector2f(position.x * TILE_SIZE, position.y * TILE_SIZE)), Vertex(position + camera_plane * LAMBDA)};
      window->draw(line2, 2, Lines); */
      
      //Draw player
      shape.setPosition((position.x * TILE_SIZE) , (position.y * TILE_SIZE));
      window->draw(shape);
      
    }

    void move(float dx, float dy){
      position.x += dx;
      position.y += dy;
    }
     
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

    Clock clock;

    bool render_map = false;
    bool killThisShit = false;
    worldMap main_map(_m); 

    //Set player
    Player m_player(1, 1, Vector2f(1.f, 0.f));
    
    RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Lets Try Raycasting");
    window.setFramerateLimit(30);
    /* CircleShape shape(100.f);
    shape.setFillColor(Color::Green); */

    VertexArray lines(sf::Lines, 18 * SCREEN_WIDTH);

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type){
              case Event::Closed:
                window.close();
                break;
              case Event::KeyPressed:
                if(Keyboard::isKeyPressed(Keyboard::Escape)) window.close();
                if(Keyboard::isKeyPressed(Keyboard::M)) render_map = !render_map;
                if(Keyboard::isKeyPressed(Keyboard::K)) killThisShit = !killThisShit;
                float move_direction = 0.f;
                if(Keyboard::isKeyPressed(Keyboard::Up)) move_direction = 1.f;
                if(Keyboard::isKeyPressed(Keyboard::Down)) move_direction = -1.f;
                
                
                if(move_direction != 0){
                  Vector2f moveVec = m_player.direction * PLAYER_SPEED * move_direction * dt; 
                  if(main_map.canMove(m_player.position.x + moveVec.x, m_player.position.y)){
                    m_player.move(moveVec.x, 0);
                  } 
                  if(main_map.canMove(m_player.position.x, m_player.position.y + moveVec.y)){
                    m_player.move(0, moveVec.y);
                  }
                }

                float rotDirection = 0.f;
                if(Keyboard::isKeyPressed(Keyboard::Left)) rotDirection = 1.f;
                if(Keyboard::isKeyPressed(Keyboard::Right)) rotDirection = -1.f;

                if(rotDirection != 0.f){
                  float rot = ROT_SPEED * rotDirection * dt;
                  m_player.direction = rotateVec2f(m_player.direction, rot);
                  m_player.camera_plane = rotateVec2f(m_player.camera_plane, rot);
                }
                //break;
            }        
        }
        //Actual Raycasting
        Entity ray;

        Color groundColor = Color::White;
        Color wallColor = Color::Blue;
        Color ceilingColor = halfColor(groundColor);

        if(!killThisShit) 
         
          for(int x = 0; x < SCREEN_WIDTH; x++){
            float cameraX = 2 * x / (float)SCREEN_WIDTH - 1.0f; // x in camera space (between -1 and +1)
            ray.position = m_player.position;
            ray.direction = m_player.direction + m_player.camera_plane * cameraX;

            if (ray.direction.x == 0 || ray.direction.y == 0) {
                continue;
            }

            // calculate distance traversed between each grid line for x and y based on direction
            sf::Vector2f deltaDist(
                    sqrt(1.0f + (ray.direction.y * ray.direction.y) / (ray.direction.x * ray.direction.x)),
                    sqrt(1.0f + (ray.direction.x * ray.direction.x) / (ray.direction.y * ray.direction.y))
            );

            Vector2i mapPos(ray.position); // which box of the map we're in

            Vector2i step; // what direction to step in (+1 or -1 for each dimension)
            Vector2f sideDist; // length of ray from current position to next x or y side

            // calculate step and initial sideDist
            if (ray.direction.x < 0.0f) {
                step.x = -1;
                sideDist.x = (ray.position.x - mapPos.x) * deltaDist.x;
            } else {
                step.x = 1;
                sideDist.x = (mapPos.x + 1.0f - ray.position.x) * deltaDist.x;
            }
            if (ray.direction.y < 0.0f) {
                step.y = -1;
                sideDist.y = (ray.position.y - mapPos.y) * deltaDist.y;
            } else {
                step.y = 1;
                sideDist.y = (mapPos.y + 1.0f - ray.position.y) * deltaDist.y;
            }

            bool hit = false; // tile type that got hit
            bool horizontal; // did we hit a horizontal side? Otherwise it's vertical

            // cast the ray until we hit a wall
            while (!hit) {
                if (sideDist.x < sideDist.y) {
                    sideDist.x += deltaDist.x;
                    mapPos.x += step.x;
                    horizontal = true;
                } else {
                    sideDist.y += deltaDist.y;
                    mapPos.y += step.y;
                    horizontal = false;
                }

                hit = main_map.isWall(mapPos.x, mapPos.y);
            }

            // calculate wall distance, projected on camera direction
            float perpWallDist;
            if (horizontal) {
                perpWallDist = std::abs((mapPos.x - ray.position.x + (1 - step.x) / 2) / ray.direction.x);
            } else {
                perpWallDist = std::abs((mapPos.y - ray.position.y + (1 - step.y) / 2) / ray.direction.y);
            }

            // calculate height of line to draw on the screen
            int lineHeight = abs(int(SCREEN_HEIGHT / perpWallDist));

            // calculate lowest and highest pixel to fill in current line
            int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
            if (drawStart < 0) {
                drawStart = 0;
            }
            int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
            if (drawEnd < 0) {
                drawEnd = 0;
            }

            // get wall color
            sf::Color color = Color::White;

            // illusion of shadows by making horizontal walls darker
            if (horizontal) {
                color.r /= 2;
                color.g /= 2;
                color.b /= 2;
            }

            // add lines to vertex buffer
            lines[x * 2].position = sf::Vector2f((float)x, (float)drawStart);
            lines[x * 2].color = color;
            lines[x * 2 + 1].position = sf::Vector2f((float)x, (float)drawEnd);
            lines[x * 2 + 1].color = color; 
    }
        window.clear();
        if(!killThisShit) window.draw(lines);
        if(render_map){
          main_map.render_2D(&window);        
          m_player.renderGrid(&window);
        }
        window.display();
    }

    return 0;

}