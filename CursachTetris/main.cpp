#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <sstream>

using namespace sf;

const int M = 25; // ������� ����� � �������� ���
const int N = 15; // ������� ��������

int field[M][N] = { 0 }; // ���� ��� ������������ �� ���������� �����
int w = 34; // ������ �� ������ ����� ������ � �������
int score = 0; // ������� ������

struct Point {
    int x, y; // ��������� ������� ���������� x �� y ����� �� �������� ���
} a[4], b[4]; // ������ �� �������� ������� ������ ������� 
              // ��������� �������� ������� ��� ��������� �������

// ����� ����� �� ������
int figures[7][4] = {
    1,3,5,7,
    2,4,5,7,
    3,5,4,6,
    3,5,4,7,
    2,3,5,7,
    3,5,7,6,
    2,3,4,5,
};

// �� ���� ������ ���� �������� �� ������� ������� �� ��� ���
bool check() {
    for (int i = 0; i < 4; i++) {
        if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) // �� ������ �� �������� �� ��� ����
            return false;
        else if (a[i].y >= 0 && field[a[i].y][a[i].x]) // �� ������ �� ������������ � ������ 
                                                       // �������� �� ���
            return false;
    }
    return true; // ������ ���� ���� �������� �� ������� �������
}

// �� ��� ����������
bool gameOver() {
    for (int j = 0; j < N; j++)
        if (field[1][j]) // �� � ��������� ������ � ����� � �������� 1
            return true; // ��� ����������, �� � ��������� ������
    return false; // ��� ������������, �� � ����� ���� ���������� ������
}

int main() {
    srand(time(0)); // ��������� ���������� ����� �� �������� ����� �������� �������� ���� �������� ���������� ����� ���
    
    // ������ ��������� ����
    void setIcon(unsigned int width, unsigned int height, const Uint8 * pixels);

    RenderWindow window(VideoMode(N * w, (M + 2) * w), "TETRIS"); // ���� ��� � ���������� ��������
    //window.setIcon(32, 32, "C:/Users/Yana Heorhieva/OneDrive/������� ����/ME/NURE/1 ����/2 �������/���/���/CursovayaTetris/icon.png");
    sf::Image icon;
    if (!icon.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/������� ����/ME/NURE/1 ����/2 �������/���/��� ��������/icon.png")) {
    }

    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr()); // ������������ ������ ��� ���� ���

    // ����������� ���������� ���
    // ��� ������� � ���� ������ ����  
    Text text;
    Font font;
    Clock gameClock;
    font.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/������� ����/ME/NURE/1 ����/2 �������/���/��� ��������/Godzilla.ttf");
    text.setFont(font);
    text.setCharacterSize(30); // ����� ������
    text.setFillColor(Color::White); // ���� ������
    text.setOutlineColor(Color(40, 40, 40)); // ������ ������� ������
    text.setOutlineThickness(2.0f); // ������� ������� ������

    // ��������� ������ �� ��'���� ��� ��������� �������� �� ����������� ���
    Texture t;
    t.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/������� ����/ME/NURE/1 ����/2 �������/���/��� ��������/tiles.png");
    Sprite tiles(t);

    int dx = 0, colorNum = 1; // dx ������� ������ �� ����������, colorNum - ����� ������� ������� ������
    bool rotate = false; // rotate �������, �� ��������� ��������� ������
    float timer = 0, delay = 0.3; // timer: ������ ��� ��������� �������� ������ �����, delay - �������� �� �������� �����
    Clock clock;
    bool ad = true;

    // ��'��� ��� ���������� ����� ���������� ��� ��� (���������, ��� ���)
    std::ostringstream gameStats;

    // ���������� ������ ��� ���������� �����
    sf::SoundBuffer startBuffer;
    sf::SoundBuffer gameOverBuffer;
    sf::Sound startSound;
    sf::Sound gameOverSound;

    // ������������ �� ������������ ������ ������
    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("C:/Users/Yana Heorhieva/OneDrive/������� ����/ME/NURE/1 ����/2 �������/���/��� ��������/background.wav")) {
    }
    backgroundMusic.setLoop(true); // ����������� ����������

    // ���������� �����
    //sf::SoundBuffer startBuffer; // ����� ����� ��� ������
    if (!startBuffer.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/������� ����/ME/NURE/1 ����/2 �������/���/��� ��������/Start.wav")) {
    }

    //sf::SoundBuffer gameOverBuffer; // ����� ����� ��� ��������
    if (!gameOverBuffer.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/������� ����/ME/NURE/1 ����/2 �������/���/��� ��������/YouLose.wav")) {
    }

    //sf::Sound startSound; // ��'��� ��� ���������� ������
    startSound.setBuffer(startBuffer); // ���������� ����� ����� ��� ��'���� startSound

    //sf::Sound gameOverSound; // ��'��� ��� ���������� ��������
    gameOverSound.setBuffer(gameOverBuffer); // ���������� ����� ����� ��� ��'���� gameOverSound

    bool gameStarted = false; // ����� �����, �� ��������� ���
    bool gamePaused = false; // ����� �����, �� ��� �����������
    float pausedTime = 0; // ��� ���������� ��������� ������������ ��� � ��������
    Clock pauseClock; // �������� ��� ����� ���� �����

    // �������� ����
    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds(); // ��������� ����, ���� ������� � ���������� ����������� ���������
        clock.restart(); // ���������� ��������� ��� ���������� ����� ������� ����

        if (!gamePaused)
            timer += time; // ���������� ����, ���� ������� � ���������� �����, ���� ��� �� �����������

        // ������� ���� ����
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            // ������ � ���������� 
            if (event.type == Event::KeyPressed) {

                if (event.key.code == Keyboard::Up)
                    rotate = true; // ���� ������ ����� - ����� ��������� ������

                else if (event.key.code == Keyboard::Right)
                    dx = 1; // ���� ������ ������ - ����� �������� ������ ������

                else if (event.key.code == Keyboard::Left)
                    dx = -1; // ���� ������ ���� - ����� �������� ������ ����

                else if (event.key.code == Keyboard::P) { // ��������� � �� �������� ��� �����

                    if (!gameStarted)
                        continue;
                    gamePaused = !gamePaused;

                    if (gamePaused)
                        pauseClock.restart(); // ������ ���� ���� �����
                    else
                        pausedTime += pauseClock.getElapsedTime().asSeconds(); // ������ ��� ����� �� ���������� ����������� ���� ���
                }
            }
        }

        // ���� ��������� ������ ���� - ������������� ������ ������
        if (Keyboard::isKeyPressed(Keyboard::Down))
            delay = 0.05;

        // ������� ����� "Game started" � ���������� ���� ����������: �����, �����, ����
        if (!gameStarted) {
            Text startText;
            Font font;
            font.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/������� ����/ME/NURE/1 ����/2 �������/���/��� ��������/Godzilla.ttf");
            startText.setFont(font);
            startText.setString("Game started");
            startText.setCharacterSize(50);
            startText.setFillColor(Color::Green);

            // ������� ������ 
            FloatRect textRect = startText.getLocalBounds(); // ������� ����������� ������ ��� ������
            startText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            startText.setPosition(Vector2f(N * w / 2, M * w / 2)); // ��������� ������ �� ������

            // ������ ����
            backgroundMusic.setVolume(20);
            startSound.play();

            float startTime = gameClock.getElapsedTime().asSeconds(); // ��� � ��������  
            while (window.isOpen() && gameClock.getElapsedTime().asSeconds() - startTime < 3.0f) {

                // ������� ���� ����, ��� ������� 
                Event e;
                while (window.pollEvent(e)) {

                    if (e.type == Event::Closed)
                        window.close();
                }

                // ��� ����������� ������ � ������� ������ 
                window.clear(Color(40, 40, 40)); // ������ ����, ���������� �����-���� ���� ����
                window.draw(startText); // ����� ���������
                window.display();
            }

            backgroundMusic.play(); // ������� ���������� ������ ������

            gameStarted = true;
            gameClock.restart();
        }

        if (!gamePaused) { // ���� ��� �� �������� 

            for (int i = 0; i < 4; i++) {
                b[i] = a[i]; 
                a[i].x += dx;
            }

            if (!check()) { // ���� ������ ��������� 

                for (int i = 0; i < 4; i++) // �������� �������� ��������� ������ � �������� ������ b
                    a[i] = b[i];
            }

            if (rotate) { // ���� ������ ��������� 

                Point p = a[1]; // ��� �������� � ����� ����� ���� ����� ������ 

                for (int i = 0; i < 4; i++) { 
                    int x = a[i].y - p.y; 
                    int y = a[i].x - p.x;

                    a[i].x = p.x - x; // ��� ���������� ������
                    a[i].y = p.y + y;
                }

                if (!check()) { // ���� ������ ��������� ���� ���������

                    for (int i = 0; i < 4; i++) // �������� �������� ��������� ������ � �������� ������ b
                        a[i] = b[i];
                }
            }

            if (timer > delay) { // ���� � �������� ���� ����� ������ ��������� ����

                for (int i = 0; i < 4; i++) { 
                    b[i] = a[i]; 
                    a[i].y += 1; // �������� ������ ���� �� 1 ���������
                }

                if (!check()) { // ���� ������ ������������ 

                    for (int i = 0; i < 4; i++) // �������� ������� �������� ���� ��������� �������� �������
                        field[b[i].y][b[i].x] = colorNum;

                    if (gameOver()) { // ���� ����� ���
                        // ����� �������� 
                        Text text;
                        Font font;
                        font.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/������� ����/ME/NURE/1 ����/2 �������/���/��� ��������/Godzilla.ttf");
                        text.setFont(font);
                        text.setString("You Lose !");
                        text.setCharacterSize(50);
                        text.setFillColor(Color::Red);

                        // ����������� 
                        FloatRect textRect = text.getLocalBounds();
                        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                        text.setPosition(Vector2f(N * w / 2, M * w / 2));

                        gameOverSound.play();

                        while (window.isOpen()) {
                            // �������� ���� 
                            Event e;

                            while (window.pollEvent(e)) {

                                if (e.type == Event::Closed)
                                    window.close();
                            }

                            window.clear(Color(40, 40, 40));
                            window.draw(text);
                            window.display();
                            backgroundMusic.stop();
                        }
                        break;
                    }

                    // ��������� ������� �����
                    colorNum = 1 + rand() % 7;
                   
                    // ��������� ����� ������
                    int n = rand() % 7; 

                    // ���������� 
                    for (int i = 0; i < 4; i++) { 
                        a[i].x = figures[n][i] % 2;
                        a[i].y = figures[n][i] / 2;
                    }
                }

                // ������� ������ 
                timer = 0; 
            }

            if (ad) {

                // ��������� ����� ������
                int n = rand() % 7; 

                if (a[0].x == 0) { 

                    for (int i = 0; i < 4; i++) {
                        a[i].x = figures[n][i] % 2;
                        a[i].y = figures[n][i] / 2;
                    }
                }
                ad = false;
            }

            int k = M - 1;

            for (int i = M - 1; i > 0; i--) {
                int count = 0; // ��������� ������� ������ � �����

                for (int j = 0; j < N; j++) {

                    if (field[i][j])
                        count++; // ������� ��������, ���� ������ ���������
                    field[k][j] = field[i][j];
                }

                if (count < N)
                    k--; // ������� ������ �����, ���� �������� ����� �� ���������� �������
                else
                    score++; // ������� ����
            }

            dx = 0;
            rotate = false;
            delay = 0.3; // �������� ����� ��������� ������� ������
        }

        window.clear(Color(40, 40, 40));

        //// ������� ������������� ��� �������������� ������
        //RectangleShape extraRow(Vector2f(N* w, w));
        //extraRow.setFillColor(Color::Transparent); // ������ ���� ���������� �������������� (����������)
        //extraRow.setOutlineColor(Color(80, 80, 80)); // ������ ���� ������� ��������������
        //extraRow.setOutlineThickness(1.0f); // ������ ������� ������� ��������������
        //extraRow.setPosition(0, 0); // ������ ��������� ��������������
        //window.draw(extraRow); // ������������ ������������� �� ����

        //// ������ ��������� �������������� ������ ����� �������� ����
        //extraRow.setPosition(0, (M - 1)* w);
        //window.draw(extraRow); // ������������ �������������� ������ �� ����

        // ����� ���� �� ���
        for (int i = 0; i < M - 1; i++) {
            for (int j = 0; j < N; j++) {

                RectangleShape cell(Vector2f(w, w));
                cell.setFillColor(Color::Transparent);
                cell.setOutlineColor(Color(80, 80, 80));
                cell.setOutlineThickness(1.0f);

                cell.setPosition(j * w, (i + 1) * w);
                window.draw(cell);
            }
        }

        // ����, ��� ������� ������ ���� �� �����, ��� ��� �������� ������� ������ - �� ���������
        for (int i = 0; i < M; i++) { 

            for (int j = 0; j < N; j++) { 

                if (field[i][j] == 0)
                    continue; 

                tiles.setTextureRect(IntRect(field[i][j] * w, 0, w, w));
                tiles.setPosition(j* w, i* w);
                window.draw(tiles); 
            }
        }

        for (int i = 0; i < 4; i++) {
            tiles.setTextureRect(IntRect(colorNum * w, 0, w, w));
            tiles.setPosition(a[i].x * w, a[i].y * w); 
            window.draw(tiles); 
        }

        gameStats.str(""); 

        if (gamePaused)
            gameStats << "Game Paused";
        else
            gameStats << "Score: " << score << "   Time: " << std::max(0.0f, gameClock.getElapsedTime().asSeconds() - pausedTime);
        text.setString(gameStats.str());
        text.setPosition(0, 0);
        window.draw(text);

        if (gamePaused) {
            Text pauseText;
            pauseText.setFont(font);
            pauseText.setString("Paused");
            pauseText.setCharacterSize(50);
            pauseText.setFillColor(Color::White);

            FloatRect textRect = pauseText.getLocalBounds();
            pauseText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            pauseText.setPosition(Vector2f(N * w / 2, M * w / 2));

            window.draw(pauseText);
        }

        window.display();
    }

    backgroundMusic.stop();

    return 0;
}
