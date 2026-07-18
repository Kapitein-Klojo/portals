#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>


class Static_Body{
public:
        sf::Vector2f pos;
        sf::RectangleShape shape;


        void draw(sf::RenderWindow& window)
        {
            window.draw(shape);
        }
};


class Player{
public:

    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::Vector2f accel;

    float MAXSPEED;
    float DASHSPEED;
    float mass;
    sf::RectangleShape shape;

    void apply_force(sf::Vector2f force)
    {
        force /= mass;
        accel += force;
    }

    void update(float dt)
    {
        vel += accel * dt;
        pos += vel * dt;

        accel = {0.f, 0.f};

        shape.setPosition(pos);

    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(shape);
    }

    void collision(Static_Body& collider)
    {   
        float Awidth = shape.getSize().x;
        float Aheight = shape.getSize().y;


        sf::Vector2f Bpos = {collider.pos.x, collider.pos.y};
        float Bwidth = collider.shape.getSize().x;
        float Bheight = collider.shape.getSize().y;

        if ((pos.x + Awidth) >= Bpos.x && pos.x <= (Bpos.x + Bwidth) && (pos.y + Aheight) >= Bpos.y && pos.y <= (Bpos.y + Bheight))
        {
            sf::Vector2f Acenter = {pos.x + (Awidth/2.f), pos.y + (Aheight/2.f)};
            sf::Vector2f Bcenter = {Bpos.x + (Bwidth/2.f), Bpos.y + (Bheight/2.f)};

            float XdistanceAB = Acenter.x - Bcenter.x;
            float YdistanceAB = Acenter.y - Bcenter.y;

            float overlapX = Awidth/2.f + Bwidth/2.f - fabs(XdistanceAB);
            float overlapY = Aheight/2.f + Bheight/2.f - fabs(YdistanceAB);
            
            
            if (overlapX > 0 && overlapY > 0)
            {
                if (overlapX < overlapY)
                {
                    pos.x += copysign(overlapX, XdistanceAB);
                    float bounce = vel.x/2.f;
                    vel.x = 0;
                    vel.x = copysign(bounce, XdistanceAB);
                }
                if (overlapX > overlapY)
                {
                    pos.y += copysign(overlapY, YdistanceAB);
                }
            }
        }
    }

    bool isOnGround(Static_Body& ground)
    {
        float Awidth = shape.getSize().x;
        float Aheight = shape.getSize().y;


        sf::Vector2f Bpos = {ground.pos.x, ground.pos.y};
        float Bwidth = ground.shape.getSize().x;
        float Bheight = ground.shape.getSize().y;

        sf::Vector2f Acenter = {pos.x + (Awidth/2.f), pos.y + (Aheight/2.f)};
        sf::Vector2f Bcenter = {Bpos.x + (Bwidth/2.f), Bpos.y + (Bheight/2.f)};

        float XdistanceAB = Acenter.x - Bcenter.x;
        float YdistanceAB = Acenter.y - Bcenter.y;

        float overlapX = Awidth/2.f + Bwidth/2.f - fabs(XdistanceAB);
        float overlapY = Aheight/2.f + Bheight/2.f - fabs(YdistanceAB);


        if (overlapX > 0 && fabs(overlapY) < 0.5f)
        {
            if (overlapX > overlapY && YdistanceAB < 0)
                return true;
            else
                return false;
        }
        return false;
    }

    void teleport_to(sf::Vector2f destination) // add direction change for vel & accel
    {
        pos = destination;
    }
};

class Portal{
public:
    sf::Vector2f pos;
    int direction; // 0 = left, 1 = right, 2 = up, 3 = down

    float height;
    float width;

    sf::RectangleShape shape;

    void draw(sf::RenderWindow& window)
    {
        window.draw(shape);
    }

    void update()
    {
        shape.setPosition(pos);
    }

    void shoot_portal(Player& player, int shootdirection, std::vector<Static_Body>& Walls)
    {   
                    
        float Closestdistance = 100000.f;
        int closest_wall;

        sf::Vector2f player_center = {player.pos.x + (player.shape.getSize().x / 2.f), player.pos.y + (player.shape.getSize().y / 2.f)};

        float offset = width/4.f;


        switch (shootdirection)
        {
            case 0: // = left
                shape.setSize({width, height});
                for (int i = 0; i < Walls.size(); i++)
                {
                    if (Walls[i].pos.x < player_center.x)
                    {
                        float Walltop = Walls[i].pos.y;
                        float Wallbottom = Walls[i].pos.y + Walls[i].shape.getSize().y;

                        if (player_center.y < Wallbottom && player_center.y > Walltop)
                        {
                            float distance = player_center.x - Walls[i].pos.x;


                            if (distance < Closestdistance)
                            {
                                Closestdistance = distance;
                                closest_wall = i;

                                
                            }

                        }
                    }
                }

                pos = {Walls[closest_wall].pos.x + Walls[closest_wall].shape.getSize().x - player.shape.getSize().x + offset ,player.pos.y - 25.f};
                update();
                break;

            case 1: // = right
                shape.setSize({width, height});
                for (int i = 0; i < Walls.size(); i++)
                {
                    if (Walls[i].pos.x > player_center.x)
                    {
                        float Walltop = Walls[i].pos.y;
                        float Wallbottom = Walls[i].pos.y + Walls[i].shape.getSize().y;

                        if (player_center.y < Wallbottom && player_center.y > Walltop)
                        {
                            float distance = Walls[i].pos.x - player_center.x;


                            if (distance < Closestdistance)
                            {
                                Closestdistance = distance;
                                closest_wall = i;

                                
                            }

                        }
                    }
                }

                pos = {Walls[closest_wall].pos.x - offset ,player.pos.y - 25.f};
                update();
                break;
            
            case 2: // = up
                shape.setSize({height, width});
                for (int i = 0; i < Walls.size(); i++)
                {
                    if (Walls[i].pos.y < player_center.y)
                    {
                        float Wallleft = Walls[i].pos.x;
                        float Wallright = Walls[i].pos.x + Walls[i].shape.getSize().x;

                        if (player_center.x > Wallleft && player_center.x < Wallright)
                        {
                            float distance = Walls[i].pos.y - player_center.y;


                            if (distance < Closestdistance)
                            {
                                Closestdistance = distance;
                                closest_wall = i;

                                
                            }

                        }
                    }
                }

                pos = {player.pos.x - 12.5f, Walls[closest_wall].pos.y + Walls[closest_wall].shape.getSize().y - player.shape.getSize().y + offset};
                update();
                break;

                case 3: // = down
                shape.setSize({height, width});
                for (int i = 0; i < Walls.size(); i++)
                {
                    if (Walls[i].pos.y > player_center.y)
                    {
                        float Wallleft = Walls[i].pos.x;
                        float Wallright = Walls[i].pos.x + Walls[i].shape.getSize().x;

                        if (player_center.x > Wallleft && player_center.x < Wallright)
                        {
                            float distance = Walls[i].pos.y - player_center.y;


                            if (distance < Closestdistance)
                            {
                                Closestdistance = distance;
                                closest_wall = i;

                                
                            }

                        }
                    }
                }

                pos = {player.pos.x - 12.5f, Walls[closest_wall].pos.y - offset};
                update();
                break;


        }

            
        
    }



};

int main()
{

    float height = 1080.f;
    float width = 1920.f;
    
    // window
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned int>(width), static_cast<unsigned int>(height)}), "portals", sf::State::Fullscreen);
    //window.setFramerateLimit(120);
    

    window.setKeyRepeatEnabled(false);

    Player player;
    player.MAXSPEED = 600;
    player.DASHSPEED = 10000;
    player.pos = {100.f, 800.f};
    player.mass = 10;
    player.shape.setSize({50, 50});
    player.shape.setFillColor(sf::Color::Black);

    

    Static_Body ground;
    ground.pos = {0.f, 980.f};
    ground.shape.setPosition(ground.pos);
    ground.shape.setSize({1920.f, 100.f});
    ground.shape.setFillColor(sf::Color(51, 51, 51));

    Static_Body leftwall;
    leftwall.pos = {0.f, 50.f};
    leftwall.shape.setPosition(leftwall.pos);
    leftwall.shape.setSize({50.f, 930.f});
    leftwall.shape.setFillColor(sf::Color(51, 51, 51));

    Static_Body rightwall;
    rightwall.pos = {1870.f, 50.f};
    rightwall.shape.setPosition(rightwall.pos);
    rightwall.shape.setSize({50.f, 930.f});
    rightwall.shape.setFillColor(sf::Color(51, 51, 51));

    Static_Body ceiling;
    ceiling.pos = {0.f, 0.f};
    ceiling.shape.setPosition(ceiling.pos);
    ceiling.shape.setSize({1920.f, 50.f});
    ceiling.shape.setFillColor(sf::Color(51, 51, 51));

    Static_Body wall;
    wall.pos = {300.f, 880};
    wall.shape.setPosition(wall.pos);
    wall.shape.setSize({200.f, 100.f});
    wall.shape.setFillColor(sf::Color(51, 51, 51));

    std::vector<Static_Body> Walls;
    Walls.push_back(ground);
    Walls.push_back(leftwall);
    Walls.push_back(rightwall);
    Walls.push_back(ceiling);
    Walls.push_back(wall);

    Portal portal1;
    portal1.shape.setFillColor(sf::Color::Cyan);
    portal1.height = 75.f;
    portal1.width = 50.f;
    portal1.shape.setSize({portal1.width, portal1.height});

    Portal portal2;
    portal2.shape.setFillColor(sf::Color::Cyan);
    portal2.height = 75.f;
    portal2.width = 50.f;
    portal2.shape.setSize({portal2.width, portal2.height});

    unsigned int which_portal = 1;






    int direction = 1;    // direction 1 = right (D), -1 = left (A)

    bool W_pressed      = false;
    bool A_pressed      = false;
    bool S_pressed      = false;
    bool D_pressed      = false;
    bool space_pressed  = false;
    bool shift_pressed  = false;

    bool up_pressed     = false;
    bool down_pressed   = false;
    bool left_pressed   = false;
    bool right_pressed  = false;

    bool is_on_ground   = false;



    sf::Clock clock;


    // loop until closed
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();


        // handle events (input)
        while (std::optional event = window.pollEvent())
        {
            // when close button is clicked
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // when keys pressed
            else if (auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                
                if (key->scancode == sf::Keyboard::Scancode::W)
                {
                    W_pressed = true;
                }
                if (key->scancode == sf::Keyboard::Scancode::A)
                {
                    A_pressed = true;
                    direction = -1;
                }
                if (key->scancode == sf::Keyboard::Scancode::S)
                {
                    S_pressed = true;
                }
                if (key->scancode == sf::Keyboard::Scancode::D)
                {
                    D_pressed = true;
                    direction = 1;
                }
                if (key->scancode == sf::Keyboard::Scancode::Space)
                {
                    space_pressed = true;
                }
                if (key->scancode == sf::Keyboard::Scancode::Escape)
                {
                    window.close();
                }
                if (key->scancode == sf::Keyboard::Scancode::RShift)
                {
                    shift_pressed = true;
                }
                if (key->scancode == sf::Keyboard::Scancode::Up)
                {
                    up_pressed = true;
                }
                if (key->scancode == sf::Keyboard::Scancode::Down)
                {
                    down_pressed = true;
                }
                if (key->scancode == sf::Keyboard::Scancode::Left)
                {
                    left_pressed = true;
                }
                if (key->scancode == sf::Keyboard::Scancode::Right)
                {
                    right_pressed = true;
                }
            }
                            // when keyboard is released
            else if (auto* key = event->getIf<sf::Event::KeyReleased>())
            {
                    
                if (key->scancode == sf::Keyboard::Scancode::D)
                {
                    D_pressed = false;
                }
                if (key->scancode == sf::Keyboard::Scancode::A)
                {
                    A_pressed = false;
                }
                if (key->scancode == sf::Keyboard::Scancode::W)
                {
                    W_pressed = false;
                }
                if (key->scancode == sf::Keyboard::Scancode::S)
                {
                    S_pressed = false;
                }
                if (key->scancode == sf::Keyboard::Scancode::Space)
                {
                    space_pressed = false;
                }
                if (key->scancode == sf::Keyboard::Scancode::RShift)
                {
                    shift_pressed = false;
                }
                if (key->scancode == sf::Keyboard::Scancode::Up)
                {
                    up_pressed = false;
                }
                if (key->scancode == sf::Keyboard::Scancode::Down)
                {
                    down_pressed = false;
                }
                if (key->scancode == sf::Keyboard::Scancode::Left)
                {
                    left_pressed = false;
                }
                if (key->scancode == sf::Keyboard::Scancode::Right)
                {
                    right_pressed = false;
                }

            }
        
        }
    
            
        // physics update

        sf::Vector2f gravity = {0.f, 9800};

        if (player.isOnGround(ground) || player.isOnGround(wall) || player.isOnGround(leftwall)|| player.isOnGround(rightwall)|| player.isOnGround(ceiling))
            is_on_ground = true;
        else
            is_on_ground = false;
     
        
        

        // player movement

        if (A_pressed && player.vel.x > -player.MAXSPEED)
        {
            player.apply_force({-5000.f, 0.f});
        }
        if (D_pressed && player.vel.x < player.MAXSPEED)
        {
            player.apply_force({5000.f, 0.f});
        }
        if (!A_pressed && !D_pressed && is_on_ground && player.vel.x != 0)
        {
            if (player.vel.x < 5.f)
            {
                player.apply_force({2500.f, 0.f});
            }
            else if (player.vel.x > 10.f)
            {
                player.apply_force({-2500.f, 0.f});
            }
            else
                player.vel.x = 0;
        }

        if ((W_pressed || space_pressed) && is_on_ground)
        {
            player.vel.y = 0.f;
            player.vel.y = -500;
            W_pressed = false;
        }
        else if (!is_on_ground)
        {
            player.apply_force(gravity);
        }
        else if (player.vel.y > 0)
        {
            player.vel.y = 0.f;
        }


        // portal shooting
        int shootDirection;  // 0 = left, 1 = right, 2 = up, 3 = down
      
        if (left_pressed)
        {
            shootDirection = 0;

            if (which_portal == 1)
            {
                portal1.shoot_portal(player, shootDirection, Walls);
                which_portal = 2;
                left_pressed = false;
            }
            else if (which_portal == 2)
            {
                portal2.shoot_portal(player, shootDirection, Walls);
                which_portal = 1;
                left_pressed = false;
            }
        }
        if (right_pressed)
        {
            shootDirection = 1;

            if (which_portal == 1)
            {
                portal1.shoot_portal(player, shootDirection, Walls);
                which_portal = 2;
                right_pressed = false;
            }
            else if (which_portal == 2)
            {
                portal2.shoot_portal(player, shootDirection, Walls);
                which_portal = 1;
                right_pressed = false;
            }
        }
        if (up_pressed)
        {
            shootDirection = 2;

            if (which_portal == 1)
            {
                portal1.shoot_portal(player, shootDirection, Walls);
                which_portal = 2;
                up_pressed = false;
            }
            else if (which_portal == 2)
            {
                portal2.shoot_portal(player, shootDirection, Walls);
                which_portal = 1;
                up_pressed = false;
            }
        }
        if (down_pressed)
        {
            shootDirection = 3;

            if (which_portal == 1)
            {
                portal1.shoot_portal(player, shootDirection, Walls);
                which_portal = 2;
                down_pressed = false;
            }
            else if (which_portal == 2)
            {
                portal2.shoot_portal(player, shootDirection, Walls);
                which_portal = 1;
                down_pressed = false;
            }
        }
        
        
        
        
        // update
        player.update(dt);

       

        player.collision(ground);
        player.collision(ceiling);
        player.collision(leftwall);
        player.collision(rightwall);
        player.collision(wall);

        


        

    
            
        // fill window with color
        window.clear(sf::Color(176, 176, 176));

        
  

        // drwa
        ground.draw(window);
        wall.draw(window);
        leftwall.draw(window);
        rightwall.draw(window);
        ceiling.draw(window);
        portal1.draw(window);
        portal2.draw(window);
        player.draw(window);
        


        // display
        window.display();

    }
        

    return 0;
}