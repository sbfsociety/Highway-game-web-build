#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <fstream>
bool lost{false};
int gameState{0};
int score{0};
int highScore{0};
bool newHighscore{false};
int obCount{0}; // count of enemies, makes sure that there are the same number and there isn't a memory leak
int veto{-2};
float travelError = 0;
int speedingTicket = 0;
int drivers{0};
bool moveCar{false};

void ReadHighScore() {
    std::ifstream r("score.txt", std::ios::in);
    r >> ::highScore;
    r.close();
}

void WriteHighScore() {
    if (::score > ::highScore) {
        std::ofstream w("score.txt", std::ios::trunc);
        w << ::score;
        ::newHighscore = true;
        w.close();
        //std::cout << "score:" << ::score << std::flush;
        //std::cout << "wrote highscore:" << ::highScore << std::flush;
    }
}


struct vec2 {
    float x, y;
};

void text(sf::Font& font, sf::RenderWindow& window, int textSize, std::string text, float x, float y);

class Player {
private:
        vec2 pos;
        float vel;
        int lane = 0;
public:
        Player(vec2 _pos, float _vel) : pos(_pos), vel(_vel) {}

        bool aLastFrame = false;
        bool dihLastFrame = false;

        int walkingAnimationIndex = 0;



        void Move(float dt) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) && lane!=-1 && !aLastFrame) {lane-=1;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) && lane!=1 && !dihLastFrame) {lane+=1;}
            aLastFrame = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
            dihLastFrame = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
            pos.y += vel * dt;

            // std::cout << lane << std::endl;
        }
        void Draw(sf::RenderWindow& window, int frameCount, sf::Sprite& player) {
            // std::cout << lane << std::endl;
            player.setPosition({float((lane + 1) * 200 + 50), pos.y + 26});
            //player.setPosition({pos.x, pos.y});

            if (frameCount % 30 == 0) {
                walkingAnimationIndex++;
                player.setTextureRect(sf::IntRect({(walkingAnimationIndex%2)*128, 0}, {128, 256}));
            }

            window.draw(player);
        }

        float GetY() const { return pos.y; }

        void SetY(float _y) { pos.y = _y; }

        int GetLane() const { return lane; }


};

class Obstacle {
private:
    vec2 pos;
    float vel{-100};
    int lane = 0;
    // bool kamikaze = false;
    int animationIndex{0};
    bool car = false;
public:
    Obstacle(int _lane, Player& player, float y) :  lane(_lane) {
        this->pos = {float((lane+1)*200 + 50), player.GetY() + y};

    }
    Obstacle(int _lane, Player& player, float _vel, float y) :  lane(_lane), vel(_vel) {
        this->pos = {float((lane+1)*200 + 50), player.GetY() + y};
        this->car = true;
    }


    void Move(float dt) {
        pos.y += vel * dt;
    }

    void Collide(Player& player) {
        if (abs(player.GetY() + 128 - 110 + 30 + 20 - pos.y) <= 50 && player.GetLane() == lane) {
            // kamikaze = true;
            // std::cout << "HIT" << std::flush;
            ::lost = true;
        }
    }

    void Draw(sf::RenderWindow& window, sf::Sprite& obs, int frameCount, Player& player) {
         /*if (kamikaze) {
             ::lost = true;
         }*/
        if (frameCount % 30 == 0) {
            obs.setTextureRect(sf::IntRect({float((animationIndex%2) * 64), 0 }, {64, 64}));
        }
        obs.setPosition({pos.x, pos.y});
        window.draw(obs);
    }

    float GetY() const { return pos.y; }

    void SetY(float _y) { pos.y = _y; }

    bool IsCar() const { return car; }


};



void text(sf::Font& font, sf::RenderWindow& window, int textSize, std::string text, float x, float y) {
    sf::Text st(font);
    st.setString(text);
    st.setFillColor(sf::Color::White);
    st.setCharacterSize(textSize);
    st.setPosition({x, y});
    window.draw(st);
}



void Background(sf::RenderWindow& window, sf::Sprite& bt, Player& player) {
    for (int i=-1; i<4; i++) {
        bt.setPosition({0, player.GetY() - -200 + float(i*200)});
        window.draw(bt);
    }
}

void Background(sf::RenderWindow& window, sf::Sprite& trava, sf::Sprite& put, Player& player) {
    float parallax = -0.5f;
    float cameraY = player.GetY();
    float grassOffset = std::fmod(cameraY * parallax, 200.f); // tile size = 200

    for (int i=-1; i<7; i++) {
        trava.setPosition({0, cameraY - 400 + grassOffset + float(i*200)});
        window.draw(trava);
    }
    for (int j=-4; j<8; j++) {
        put.setPosition({0, player.GetY() - 200 + float(j*200)});
        window.draw(put);
    }
}




void obs(Player& player, std::vector<Obstacle>& enemies, sf::RenderWindow& window, float dt, sf::Sprite& obs, int frameCount) {
    srand(time(NULL));

    static int theOneBefore = -2;
    static int lastLane = -2;

    for (int i = 0; i < 10 - ::obCount; i++) {
        int randomLane = (rand() % 3) - 1;

        while (randomLane == lastLane || randomLane == theOneBefore) {
            randomLane = (rand() % 3) - 1;
        }

        theOneBefore = randomLane;

        lastLane = randomLane;

        enemies.push_back(Obstacle( randomLane, player, 900 + i * 400));
        // enemies[i].SetY(enemies[i].GetY() + (rand() % 5) * 50 + i * 100);
        ::obCount++;
    }

    // int j{0};

    for (int i = enemies.size() - 1; i >= 0; i--) {

        if (enemies[i].GetY() < player.GetY() - 300) {
            enemies.erase(enemies.begin() + i);
            ::obCount--;
        }
        else{
            enemies[i].Move(dt);
            enemies[i].Collide(player);
            enemies[i].Draw(window, obs, frameCount , player);
        }
    }

    if (::lost || ::gameState == 0) {
        enemies.clear();
        ::obCount = 0;
    }


}

void obBalanced(Player& player, std::vector<Obstacle>& enemies, sf::RenderWindow& window, float dt, sf::Sprite& obs, int frameCount) {
    srand(time(NULL));

    static int theOneBefore = -2;
    static int lastLane = -2;

    for (int i = 0; i < 10 - ::obCount; i++) {
        int randomLane = (rand() % 3) - 1;


        while (randomLane == lastLane || randomLane == theOneBefore) {
            randomLane = (rand() % 3) - 1;
        }

        theOneBefore = randomLane;

        if (enemies.size() > 0) {
            float spawnY = enemies.back().GetY();
        }
        else {
            float spawnY = player.GetY();
        }

    if (::travelError > 7 ) ::travelError = 2;


        lastLane = randomLane;
        int j = 0;
        for (int i =  -1; i <=1; i++) {
                if (i != randomLane) {
                    // enemies.push_back(Obstacle( i, player, 900 + ::travelError * 200));
                    enemies.push_back(Obstacle( i, player, 600 + ::travelError * 300));
                    j++;
                    ::obCount++;
                    ::travelError++;
                }
        }
        // enemies.push_back(Obstacle( randomLane, player, 900 + i * 400));
        // enemies[i].SetY(enemies[i].GetY() + (rand() % 5) * 50 + i * 100);
        // ::obCount++;
    }

    // int j{0};

    for (int i = enemies.size() - 1; i >= 0; i--) {

        if (enemies[i].GetY() < player.GetY() - 300) {
            enemies.erase(enemies.begin() + i);
            ::obCount--;
        }
        else{
            enemies[i].Move(dt);
            enemies[i].Collide(player);
            enemies[i].Draw(window, obs, frameCount , player);
        }
    }

    if (::lost || ::gameState == 0) {
        enemies.clear();
        ::obCount = 0;
    }


}

void obHard(Player& player, std::vector<Obstacle>& enemies, sf::RenderWindow& window, float dt, sf::Sprite& obs, int frameCount, sf::Sprite& car) {
    srand(time(NULL));
    static int theOneBefore = -2;
    static int lastLane = -2;
    // KUGLE
    for (int i = 0; i < 8 - ::obCount; i++) {
        int randomLane = (rand() % 3) - 1;


        while (randomLane == lastLane || randomLane == theOneBefore) {
            randomLane = (rand() % 3) - 1;
        }

        theOneBefore = randomLane;

        if (enemies.size() > 0) {
            float spawnY = enemies.back().GetY();
        }
        else {
            float spawnY = player.GetY();
        }

        if (::travelError > 7 ) ::travelError = 2;


        lastLane = randomLane;
        int j = 0;
        for (int i =  -1; i <=1; i++) {
            if (i != randomLane) {
                // enemies.push_back(Obstacle( i, player, 900 + ::travelError * 200));
                enemies.push_back(Obstacle( i, player, 600 + ::travelError * 300));
                j++;
                ::obCount++;
                ::travelError++;
            }
        }
        // enemies.push_back(Obstacle( randomLane, player, 900 + i * 400));
        // enemies[i].SetY(enemies[i].GetY() + (rand() % 5) * 50 + i * 100);
        // ::obCount++;
    }
    // PIJANI VOZACI
    for (int i = 0; i < 2 - ::speedingTicket; i++) {
        int randomLane = (rand() % 3) - 1;
        if (randomLane == lastLane || randomLane == theOneBefore) { randomLane = (rand() % 3) - 1; }
        enemies.push_back(Obstacle( randomLane, player, -200, 600 + ((::speedingTicket % 4) + 3) * 600));
        ::speedingTicket++;



    }

    // int j{0};

    for (int i = enemies.size() - 1 - ::speedingTicket; i >= 0; i--) {

        if (enemies[i].GetY() < player.GetY() - 300) {
            enemies.erase(enemies.begin() + i);
            if (enemies[i].IsCar()) {
                ::speedingTicket--;
            }
            else {
                ::obCount--;
            }
        }
        else{
            enemies[i].Move(dt);
            enemies[i].Collide(player);
            if (enemies[i].IsCar()) {
                enemies[i].Draw(window, car, frameCount , player);
            }
            else {
                enemies[i].Draw(window, obs, frameCount , player);
            }

        }
    }

    if (::lost || ::gameState == 0) {
        enemies.clear();
        ::obCount = 0;
    }


}

void obHardFixed(Player& player, std::vector<Obstacle>& enemies, sf::RenderWindow& window, float dt, sf::Sprite& obs, int frameCount,sf::Sprite& car, sf::Sound& carS, sf::Sound& ball) {
    static int theOneBefore = -2;
    static int lastLane = -2;

    int normalObstacles = 0;
    int fastCars = 0;
    for (size_t i = 0; i < enemies.size(); i++) {
        if (enemies[i].IsCar()) {
            fastCars++;
        } else {
            normalObstacles++;
        }
    }
    // ukupno 10 obs, 8 lopta 2 pijana vozaca
    while (enemies.size() < 10) {
        ::moveCar =  false;
        float offsetFromPlayer = 900;
        if (!enemies.empty()) {
            // zamena za ::travelError, tacno 400 piksela ispred proslog auta
            offsetFromPlayer = (enemies.back().GetY() - player.GetY()) + 400;
        }

        if (normalObstacles < 8) {
            int freeLane = (rand() % 3) - 1;

            // uvek druga traka
            while (freeLane == lastLane || freeLane == theOneBefore) {
                freeLane = (rand() % 3) - 1;
            }
            theOneBefore = lastLane;
            lastLane = freeLane;
            ::veto = freeLane;
            // uvek dva ob oko prazne traker
            for (int lane = -1; lane <= 1; lane++) {
                if (lane != freeLane) {
                    enemies.push_back(Obstacle(lane, player, offsetFromPlayer));
                    normalObstacles++;
                }
            }
        }
        else if (fastCars < 2) {
            // pijani vozaci
            int randomLane = (rand() % 3) - 1;
            if (randomLane == ::veto) {
                ::moveCar = true;
            }
            if (!::moveCar) {
                enemies.push_back(Obstacle(randomLane, player, -300, offsetFromPlayer + 300));
            }
            else {
                enemies.push_back(Obstacle(randomLane, player, -300, offsetFromPlayer + 200));
            }
            fastCars++;
        }
    }


    // petlja za prepreke i despawnavanje onih koji vise nisu na ekranu

    for (int i = enemies.size() - 1; i >= 0; i--) {
        if (enemies[i].GetY() < player.GetY() - 300) {
            enemies.erase(enemies.begin() + i);
        } else {
            bool soundPlayed = false;
            enemies[i].Move(dt);
            enemies[i].Collide(player);
            if (!enemies[i].IsCar()) {
                enemies[i].Draw(window, obs, frameCount, player);
                /*if (ball.getStatus() != sf::Sound::Status::Playing) {
                    ball.play();
                }*/
            }
            else{
                enemies[i].Draw(window, car, frameCount, player);
                if (!soundPlayed && carS.getStatus() != sf::Sound::Status::Playing && abs(enemies[i].GetY() - player.GetY()) < 500) {
                    carS.play();
                    soundPlayed = true;
                }

            }
        }

        if (::lost || ::gameState == 0) {
            enemies.clear();
        }
    }
}
int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 0;
    settings.majorVersion = 2; 
    settings.minorVersion = 0;

    sf::RenderWindow window(sf::VideoMode(800, 600), "Game", sf::Style::Default, settings);

    Player player({200, 200},200);
    window.setFramerateLimit(60);

    sf::View playerView(sf::FloatRect({0, 0}, {600, 800}));

    // PROBA OBSTACLE
    sf::Texture obstacleTexture;
    obstacleTexture.setSmooth(true);
    obstacleTexture.loadFromFile("../assets/blob.png");
    sf::Sprite obstacleSprite(obstacleTexture);
    obstacleSprite.setScale({1.f, 1.f});
    obstacleSprite.setTextureRect(sf::IntRect({0, 0}, {64, 64}));

    //Obstacle obstacle1(1, player);

    sf::Font font;
    font.openFromFile("../assets/flappy.ttf");


    // OBSTACLES ---------------------------------------------------------

    std::vector<Obstacle> enemies;


    // SOUND EFFECTS
    sf::SoundBuffer splatB, runB, carB, melonB;
    if (!splatB.loadFromFile("../assets/splatC.mp3")) std::cerr << "Error loading splatC.mp3" << std::endl;
    if (!runB.loadFromFile("../assets/run.mp3")) std::cerr << "Error loading run.mp3" << std::endl;
    if (!carB.loadFromFile("../assets/carCCC.mp3")) std::cerr << "Error loading carC.mp3" << std::endl;
    if (!melonB.loadFromFile("../assets/melon.mp3")) std::cerr << "Error loading melon.mp3" << std::endl;

    sf::Sound splat(splatB);
    splat.setVolume(100);
    sf::Sound run(runB);
    run.setVolume(100);
    sf::Sound carS(carB);
    carS.setVolume(40);
    sf::Sound melon(melonB);
    melon.setVolume(100);

    sf::Music soundtrack;
    if (!soundtrack.openFromFile("../assets/soundtrack.mp3")) std::cerr << "Error loading soundtrack.ogg" << std::endl;
    soundtrack.setVolume(30);
    soundtrack.setLooping(true);





    // SLIKA -------------------------------------------------------------

    sf::Texture backgroundTexture, travaT, putT;
    backgroundTexture.loadFromFile("../assets/popravka.png");
    sf::Sprite background(backgroundTexture);
    background.setPosition({100, 100});
    background.setScale({1.f, 1.f});

    travaT.loadFromFile("../assets/trava.png");
    putT.loadFromFile("../assets/ulica.png");
    sf::Sprite trava(travaT);
    trava.setScale({1.f, 1.f});
    sf::Sprite put(putT);

    // SPRITE AUTA
    sf::Texture carTexture;
    carTexture.loadFromFile("../assets/car.png");
    sf::Sprite car(carTexture);
    car.setScale({1.f, 1.f});




    // ----------------------------------------------------------------

    int frameCount = 0;

    sf::Clock clock;
    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("../assets/darthMalls.png")) std::cerr << "Error loading player.png" << std::endl;

    sf::Sprite playerSprite(playerTexture);
    playerSprite.setScale(sf::Vector2f({0.5f, 0.4f}));
    playerSprite.setTextureRect(sf::IntRect({0, 0}, {128, 256}));

    float cameraY = 0;
    float autoScrollSpeed = 200.f;

    ::gameState = 0;

    int movPos = 0;

    soundtrack.play();


    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (::gameState == 0 &&  sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {::gameState = 1; ::score = 0;  }
        }

        frameCount++;
        float dt = clock.restart().asSeconds();


        if (::lost) {
            splat.stop();
            run.stop();
            carS.stop();
            melon.stop();
            sf::sleep(sf::seconds(0.1f));
            splat.play();
            sf::sleep(sf::seconds(1.f));
            gameState = 0;
            ::travelError = 0;
            ::speedingTicket = 0;
            soundtrack.stop();
            sf::Clock clock2;
            clock2.restart();
            //if (clock2.getElapsedTime().asSeconds() >= 1.f) {
                soundtrack.play();
            //}


            ::lost = false;
            WriteHighScore();
            // ::score = 0;
        }

        if (::gameState == 0) {

            float bobbingOffset = std::sin(frameCount * 0.05f) * 15.f;
            ReadHighScore();
            window.clear(sf::Color::Black);
            cameraY += autoScrollSpeed * dt;

            playerView.setCenter({300, 400 + cameraY});
            window.setView(playerView);

            player.SetY(cameraY + 200);

            Background(window, trava, put , player);

            text(font, window, 32, "Press space to play", 140  , 300 + player.GetY() + bobbingOffset );
            text(font, window, 60, "Highscore: " + std::to_string(::highScore), 120, -130 + player.GetY());
            text(font, window, 60, "Score: " + std::to_string(::score), 190, -30 + player.GetY());




        }


        if (::gameState == 1){

            ++::score;

            player.Move(dt);

            window.clear(sf::Color::White);


            playerView.setCenter({300, 400 - 200 + player.GetY()});
            window.setView(playerView);
            // Background(window, background, player);
            Background(window, trava, put , player);
            player.Draw(window, frameCount, playerSprite);
            std::string score = std::to_string(::score);
            text(font, window, 60, score , 250 + 24 - score.length() * 38/3, -130 + player.GetY());


            obHardFixed(player, enemies, window, dt, obstacleSprite, frameCount, car, carS, melon);





            // std::cout << 1/dt << std::endl;
            //obstacle1.Move(dt);
            //obstacle1.Collide(player);
            //obstacle1.Draw(window, obstacleSprite, frameCount, player);
        }
        else {

        }

        window.display();


    }


    return 0;
}