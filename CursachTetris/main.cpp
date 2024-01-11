#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <sstream>

using namespace sf;

const int M = 25; // кількість рядків у ігровому полі
const int N = 15; // кількість стовпців

int field[M][N] = { 0 }; // поле гри представлене як двовимірний масив
int w = 34; // ширина та висота кожної комірки у пікселях
int score = 0; // рахунок гравця

struct Point {
    int x, y; // структура визначає координати x та y точки на ігровому полі
} a[4], b[4]; // поточні та попередні позиції падачуї фігурки 
              // зберігають попередні позиції для виявлення зіткнень

// форми фігур за схемою
int figures[7][4] = {
    1,3,5,7,
    2,4,5,7,
    3,5,4,6,
    3,5,4,7,
    2,3,5,7,
    3,5,7,6,
    2,3,4,5,
};

// чи може фігура бути розміщена на поточній позиції на полі гри
bool check() {
    for (int i = 0; i < 4; i++) {
        if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) // чи фігура не виходить за межі поля
            return false;
        else if (a[i].y >= 0 && field[a[i].y][a[i].x]) // чи фігура не перетинається з іншими 
                                                       // комірками на полі
            return false;
    }
    return true; // фігура може бути розміщена на поточній позиції
}

// чи гра закінчилася
bool gameOver() {
    for (int j = 0; j < N; j++)
        if (field[1][j]) // чи є заповнена комірка в рядку з індексом 1
            return true; // гра закінчилася, бо є заповнена комірка
    return false; // гра продовжується, бо в рядку немає заповнених комірок
}

int main() {
    srand(time(0)); // генерація випадкових чисел за поточним часом допомагає отримати більш випадкові результати кожен раз
    
    // іконка грального вікна
    void setIcon(unsigned int width, unsigned int height, const Uint8 * pixels);

    RenderWindow window(VideoMode(N * w, (M + 2) * w), "TETRIS"); // вікно гри з відповідними розмірами
    //window.setIcon(32, 32, "C:/Users/Yana Heorhieva/OneDrive/Рабочий стол/ME/NURE/1 КУРС/2 СЕМЕСТР/ОПн/ЛАБ/CursovayaTetris/icon.png");
    sf::Image icon;
    if (!icon.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/Рабочий стол/ME/NURE/1 КУРС/2 СЕМЕСТР/ОПн/для курсовой/icon.png")) {
    }

    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr()); // Встановлення іконки для вікна гри

    // відображення інформації гри
    // для рахунку і часу зверху вікна  
    Text text;
    Font font;
    Clock gameClock;
    font.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/Рабочий стол/ME/NURE/1 КУРС/2 СЕМЕСТР/ОПн/для курсовой/Godzilla.ttf");
    text.setFont(font);
    text.setCharacterSize(30); // розмір шрифту
    text.setFillColor(Color::White); // колір тексту
    text.setOutlineColor(Color(40, 40, 40)); // відтінок обводки тексту
    text.setOutlineThickness(2.0f); // товщина обводки тексту

    // підготовка змінних та об'єктів для управління фігурами та відображення гри
    Texture t;
    t.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/Рабочий стол/ME/NURE/1 КУРС/2 СЕМЕСТР/ОПн/для курсовой/tiles.png");
    Sprite tiles(t);

    int dx = 0, colorNum = 1; // dx зміщення фігури по горизонталі, colorNum - номер кольору поточної фігури
    bool rotate = false; // rotate позначає, чи необхідно повернути фігуру
    float timer = 0, delay = 0.3; // timer: таймер для керування швидкістю падіння фігур, delay - затримка між падіннями фігур
    Clock clock;
    bool ad = true;

    // об'єкт для формування рядка інформації про гру (результат, час гри)
    std::ostringstream gameStats;

    // оголошення змінних для відтворення звуків
    sf::SoundBuffer startBuffer;
    sf::SoundBuffer gameOverBuffer;
    sf::Sound startSound;
    sf::Sound gameOverSound;

    // завантаження та налаштування фонової музики
    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("C:/Users/Yana Heorhieva/OneDrive/Рабочий стол/ME/NURE/1 КУРС/2 СЕМЕСТР/ОПн/для курсовой/background.wav")) {
    }
    backgroundMusic.setLoop(true); // повторюване відтворення

    // відтворення звуків
    //sf::SoundBuffer startBuffer; // буфер звуку для старту
    if (!startBuffer.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/Рабочий стол/ME/NURE/1 КУРС/2 СЕМЕСТР/ОПн/для курсовой/Start.wav")) {
    }

    //sf::SoundBuffer gameOverBuffer; // буфер звуку для програшу
    if (!gameOverBuffer.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/Рабочий стол/ME/NURE/1 КУРС/2 СЕМЕСТР/ОПн/для курсовой/YouLose.wav")) {
    }

    //sf::Sound startSound; // об'єкт для відтворення старту
    startSound.setBuffer(startBuffer); // встановлюю буфер звуку для об'єкта startSound

    //sf::Sound gameOverSound; // об'єкт для відтворення програшу
    gameOverSound.setBuffer(gameOverBuffer); // встановлюю буфер звуку для об'єкта gameOverSound

    bool gameStarted = false; // змінна вказує, чи розпочато гру
    bool gamePaused = false; // змінна вказує, чи гра призупинена
    float pausedTime = 0; // для збереження тривалості призупинення гри в секундах
    Clock pauseClock; // годинник для виміру часу паузи

    // основний цикл
    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds(); // отримання часу, який пройшов з останнього перезапуску годинника
        clock.restart(); // перезапуск годинника для наступного виміру проміжку часу

        if (!gamePaused)
            timer += time; // обчислення часу, який пройшов з останнього кадру, якщо гра не призупинена

        // обробка подій вікна
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            // робота з клавіатурою 
            if (event.type == Event::KeyPressed) {

                if (event.key.code == Keyboard::Up)
                    rotate = true; // якщо стрілка вгору - можна повернути фігуру

                else if (event.key.code == Keyboard::Right)
                    dx = 1; // якщо стрілка вправо - можна здвинути фігуру впарво

                else if (event.key.code == Keyboard::Left)
                    dx = -1; // якщо стрілка вліво - можна здвинути фігуру вліво

                else if (event.key.code == Keyboard::P) { // натиснути Р на клавіатурі для паузи

                    if (!gameStarted)
                        continue;
                    gamePaused = !gamePaused;

                    if (gamePaused)
                        pauseClock.restart(); // почати вимір часу паузи
                    else
                        pausedTime += pauseClock.getElapsedTime().asSeconds(); // додати час паузи до загального затриманого часу гри
                }
            }
        }

        // якщо натиснута стрілка вниз - прискорюється падіння фігури
        if (Keyboard::isKeyPressed(Keyboard::Down))
            delay = 0.05;

        // створюю текст "Game started" і встановлюю його властивості: шрифт, розмір, колір
        if (!gameStarted) {
            Text startText;
            Font font;
            font.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/Рабочий стол/ME/NURE/1 КУРС/2 СЕМЕСТР/ОПн/для курсовой/Godzilla.ttf");
            startText.setFont(font);
            startText.setString("Game started");
            startText.setCharacterSize(50);
            startText.setFillColor(Color::Green);

            // позиція тексту 
            FloatRect textRect = startText.getLocalBounds(); // отримую прямокутник розміру для тексту
            startText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            startText.setPosition(Vector2f(N * w / 2, M * w / 2)); // положення тексту по центру

            // музика фону
            backgroundMusic.setVolume(20);
            startSound.play();

            float startTime = gameClock.getElapsedTime().asSeconds(); // час у секундах  
            while (window.isOpen() && gameClock.getElapsedTime().asSeconds() - startTime < 3.0f) {

                // обробка подій вікна, щоб закрити 
                Event e;
                while (window.pollEvent(e)) {

                    if (e.type == Event::Closed)
                        window.close();
                }

                // для початкового екрану з написом старту 
                window.clear(Color(40, 40, 40)); // очищаю вікно, встановлюю темно-сірий колір фону
                window.draw(startText); // текст відображаю
                window.display();
            }

            backgroundMusic.play(); // Початок відтворення фонової музики

            gameStarted = true;
            gameClock.restart();
        }

        if (!gamePaused) { // якщо гра не зупинена 

            for (int i = 0; i < 4; i++) {
                b[i] = a[i]; 
                a[i].x += dx;
            }

            if (!check()) { // якщо фігури зіткнулися 

                for (int i = 0; i < 4; i++) // відновити попереднє положення фігури з часового масиву b
                    a[i] = b[i];
            }

            if (rotate) { // якщо фігуру повернули 

                Point p = a[1]; // для повороту в якості опору беру центр фігури 

                for (int i = 0; i < 4; i++) { 
                    int x = a[i].y - p.y; 
                    int y = a[i].x - p.x;

                    a[i].x = p.x - x; // нові координати фігури
                    a[i].y = p.y + y;
                }

                if (!check()) { // якщо фігура стикається після обертання

                    for (int i = 0; i < 4; i++) // відновлюю попереднє положення фігури з часового масиву b
                        a[i] = b[i];
                }
            }

            if (timer > delay) { // якщо з останньої осені цифри минуло достатньо часу

                for (int i = 0; i < 4; i++) { 
                    b[i] = a[i]; 
                    a[i].y += 1; // переміщую фігуру вниз на 1 положення
                }

                if (!check()) { // якщо фігури зіштовхнулися 

                    for (int i = 0; i < 4; i++) // заповнюю клітинки ігрового поля відповідним кольором малюнка
                        field[b[i].y][b[i].x] = colorNum;

                    if (gameOver()) { // якщо кінець гри
                        // вивод програшу 
                        Text text;
                        Font font;
                        font.loadFromFile("C:/Users/Yana Heorhieva/OneDrive/Рабочий стол/ME/NURE/1 КУРС/2 СЕМЕСТР/ОПн/для курсовой/Godzilla.ttf");
                        text.setFont(font);
                        text.setString("You Lose !");
                        text.setCharacterSize(50);
                        text.setFillColor(Color::Red);

                        // відцентровую 
                        FloatRect textRect = text.getLocalBounds();
                        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                        text.setPosition(Vector2f(N * w / 2, M * w / 2));

                        gameOverSound.play();

                        while (window.isOpen()) {
                            // закриття вікна 
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

                    // генерація кольору фігур
                    colorNum = 1 + rand() % 7;
                   
                    // генерація форми фігури
                    int n = rand() % 7; 

                    // координати 
                    for (int i = 0; i < 4; i++) { 
                        a[i].x = figures[n][i] % 2;
                        a[i].y = figures[n][i] / 2;
                    }
                }

                // оновлюю таймер 
                timer = 0; 
            }

            if (ad) {

                // генерація форми фігури
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
                int count = 0; // заповнена кількість комірок у рядку

                for (int j = 0; j < N; j++) {

                    if (field[i][j])
                        count++; // збільшую лічильник, якщо клітина заповнена
                    field[k][j] = field[i][j];
                }

                if (count < N)
                    k--; // зменшую індекс рядка, якщо поточний рядок не заповнений повністю
                else
                    score++; // збільшую бали
            }

            dx = 0;
            rotate = false;
            delay = 0.3; // затримка перед наступним падінням фігури
        }

        window.clear(Color(40, 40, 40));

        //// Создаем прямоугольник для дополнительной строки
        //RectangleShape extraRow(Vector2f(N* w, w));
        //extraRow.setFillColor(Color::Transparent); // Задаем цвет заполнения прямоугольника (прозрачный)
        //extraRow.setOutlineColor(Color(80, 80, 80)); // Задаем цвет границы прямоугольника
        //extraRow.setOutlineThickness(1.0f); // Задаем толщину границы прямоугольника
        //extraRow.setPosition(0, 0); // Задаем положение прямоугольника
        //window.draw(extraRow); // Отрисовываем прямоугольник на окне

        //// Задаем положение дополнительной строки внизу игрового поля
        //extraRow.setPosition(0, (M - 1)* w);
        //window.draw(extraRow); // Отрисовываем дополнительную строку на окне

        // малюю сітку на фоні
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

        // цикл, щоб фігурки падали один на одний, щоб при досяганні границі нижньої - не пропадали
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
