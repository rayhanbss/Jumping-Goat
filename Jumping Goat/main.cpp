#include "game.hpp"

int main()
{
    srand(time(0));

    RenderWindow app(VideoMode(W, H), "Asteroids!");
    app.setFramerateLimit(60);

    Texture t1, t2, t3, t4, t5, t6, t7, t8;
    t1.loadFromFile("images/spaceship.png");
    t2.loadFromFile("images/background.jpg");
    t3.loadFromFile("images/explosions/type_C.png");
    t4.loadFromFile("images/rock.png");
    t5.loadFromFile("images/fire_blue.png");
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/explosions/type_B.png");
    t8.loadFromFile("images/backgroundAwal.png");

    t1.setSmooth(true);
    t2.setSmooth(true);

    Sprite background(t2);
    Sprite backgroundMenu(t8);

    Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
    Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
    Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
    Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
    Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
    Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
    Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);

    list<Entity*> entities;

    for (int i = 0; i < 15; i++) {
        Asteroids* a = new Asteroids();
        a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
        entities.push_back(a);
    }

    player* p = new player();
    p->settings(sPlayer, 200, 200, 0, 20);
    entities.push_back(p);

    sf::Music music;
    if (!music.openFromFile("music/music.ogg")) {
        // error loading music
        return -1;
    }

    // Set some properties of the music
    music.setLoop(true); // Loop the music
    music.setVolume(50); // Set the volume (0 to 100)

    // Play the music
    music.play();


    sf::Font font;
    font.loadFromFile("font/Pixeltype.ttf");
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(40);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 0);

    sf::Text lifeText;
    lifeText.setFont(font);
    lifeText.setCharacterSize(40);
    lifeText.setFillColor(sf::Color::White);
    lifeText.setPosition(10, 25);

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(100);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("Game Over!");
    gameOverText.setPosition(W / 2 - 150, H / 2 -110);

    sf::Text playAgainText;
    playAgainText.setFont(font);
    playAgainText.setCharacterSize(40);
    playAgainText.setFillColor(sf::Color::White);
    playAgainText.setString("Press \"Enter\" To Play Again");
    playAgainText.setPosition(W / 2 - 150, H / 2 + 50);

    int score = 0;
    int life = 5;

    bool gameOver = false; 
    bool showMenu = true;

    while (showMenu) {
        Event event;
        while (app.pollEvent(event))
        {
            if (event.type == Event::Closed)
                app.close();

            if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::Enter) {
                    showMenu = false;
                }
        }

        app.draw(backgroundMenu);  
        app.display();
    }

    /////main loop/////
    while (app.isOpen() && !showMenu)
    {

        Event event;
        while (app.pollEvent(event))
        {
            if (event.type == Event::Closed)
                app.close();

            if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::Space) {
                    bullet* b = new bullet();
                    b->settings(sBullet, p->x, p->y, p->angle, 10);
                    entities.push_back(b);
                }
        }

        if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3;
        if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle -= 3;
        if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust = true;
        else p->thrust = false;

        for (auto a : entities)
            for (auto b : entities) {
                if (a->name == "Asteroids" && b->name == "bullet")
                    if (isCollide(a, b)) {
                        a->life = false;
                        b->life = false;

                        score += 10;

                        Entity* e = new Entity();
                        e->settings(sExplosion, a->x, a->y);
                        e->name = "explosion";
                        entities.push_back(e);


                        for (int i = 0; i < 2; i++) {
                            if (a->R == 15) continue;
                            Entity* e = new Asteroids();
                            e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
                            entities.push_back(e);
                        }

                    }

                if (a->name == "player" && b->name == "Asteroids")
                    if (isCollide(a, b)) {
                        b->life = false;

                        life -= 1;

                        Entity* e = new Entity();
                        e->settings(sExplosion_ship, a->x, a->y);
                        e->name = "explosion";
                        entities.push_back(e);

                        p->settings(sPlayer, W / 2, H / 2, 0, 20);
                        p->dx = 0; p->dy = 0;
                    }
            }

        
        scoreText.setString("Score: " + std::to_string(score));
        lifeText.setString("Life: " + std::to_string(life));

        if (p->thrust)  p->anim = sPlayer_go;
        else   p->anim = sPlayer;


        for (auto e : entities)
            if (e->name == "explosion")
                if (e->anim.isEnd()) e->life = 0;

        if (rand() % (300 + rand() % 5) == 0) {
            Asteroids* a = new Asteroids();
            a->settings(sRock, 0, rand() % H, rand() % 360, 25);
            entities.push_back(a);
        }

        for (auto i = entities.begin(); i != entities.end();)
        {
            Entity* e = *i;

            e->update();
            e->anim.update();

            if (e->life == false) { i = entities.erase(i); delete e; }
            else i++;

            //////draw//////
            app.draw(background);
            app.draw(scoreText);
            app.draw(lifeText);
        }
        for (auto i : entities) i->draw(app);

        if (life <= 0) {
            gameOver = true;
            music.stop();
            app.draw(background);
            app.draw(gameOverText);
            scoreText.setFillColor(sf::Color::Red);
            scoreText.setPosition(W / 2 - 150, H / 2 -20);
            app.draw(scoreText);
            app.draw(playAgainText);

            if (Keyboard::isKeyPressed(Keyboard::Enter)) {
                score = 0;
                life = 5;

                scoreText.setFillColor(sf::Color::White);
                scoreText.setPosition(10, 0);
                
                p->settings(sPlayer, 200, 200, 0, 20);
                entities.clear();
                entities.push_back(p);

                for (int i = 0; i < 15; i++) {
                    Asteroids* a = new Asteroids();
                    a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
                    entities.push_back(a);
                }

                gameOver = false;
                music.play();
            }
        }

        app.display();
    }
    
    return 0;
}