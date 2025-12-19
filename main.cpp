#include "raylib.h"
#include <fstream>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MAX_ENEMIES = 20;
const float PLAYER_SPEED = 10.0f;
const float BULLET_SPEED = 15.0f;

struct Player
{
    Rectangle rect;
};

struct Bullet
{
    Rectangle rect;
    bool active;
};

struct Enemy
{
    Rectangle rect;
    bool alive;
};

void InitWindowGame();
void InitPlayer(Player &player);
void InitBullet(Bullet &bullet);
void InitEnemies(Enemy enemies[]);
void UpdatePlayer(Player &player);
// i change function here
void FireBullet(Player &player, Bullet &bullet, int &bulletsUsed);
void MoveBullet(Bullet &bullet);
void UpdateBullet(Player &player, Bullet &bullet, int &bulletsUsed);

void MoveEnemies(Enemy enemies[]);
void UpdateEnemies(Enemy enemies[]);

void CheckCollisions(Bullet &bullet, Enemy enemies[], int &score);
void DrawPlayer(Player &player);
void DrawEnemies(Enemy enemies[]);
void DrawBullet(Bullet &bullet);

int SaveLoadHighScore(int score);
int BulletStats(int bullets, int score);

int main()
{
    InitWindowGame();
    SetTargetFPS(60);

    Player player;
    Bullet bullet;
    Enemy enemies[MAX_ENEMIES];
    // bullet stats
    int score = 0;
    int bulletsUsed = 0;
    int highScore = SaveLoadHighScore(0);

    InitPlayer(player);
    InitBullet(bullet);
    InitEnemies(enemies);

    while (!WindowShouldClose())
    {
        UpdatePlayer(player);
        UpdateBullet(player, bullet, bulletsUsed);
        UpdateEnemies(enemies);
        CheckCollisions(bullet, enemies, score);

        if (score > highScore)
            highScore = score;

        BeginDrawing();
        ClearBackground(BLACK);

        DrawPlayer(player);
        DrawBullet(bullet);
        DrawEnemies(enemies);

        DrawText(TextFormat("Score: %d  High Score: %d", score, highScore), 20, 20, 20, WHITE);
        DrawText(TextFormat("Bullets Used: %d", bulletsUsed), 20, 45, 20, WHITE);

        EndDrawing();
    }
    // bullett call
    SaveLoadHighScore(highScore);
    BulletStats(bulletsUsed, score);
    CloseWindow();
    return 0;
}

void InitWindowGame()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders");
}

void InitPlayer(Player &player)
{
    player.rect = {SCREEN_WIDTH / 2.0f - 25, SCREEN_HEIGHT - 99, 50, 20};
}

void InitBullet(Bullet &bullet)
{
    bullet.active = false;
}

void InitEnemies(Enemy enemies[])
{
    int index = 0;
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 5; c++)
        {
            enemies[index].rect = {100 + c * 100.0f, 50 + r * 50.0f, 40, 20};
            enemies[index].alive = true;
            index++;
        }
    }
}

void UpdatePlayer(Player &player)
{
    if (IsKeyDown(KEY_LEFT))
        player.rect.x -= PLAYER_SPEED;
    if (IsKeyDown(KEY_RIGHT))
        player.rect.x += PLAYER_SPEED;

    if (player.rect.x < 0)
        player.rect.x = 0;
    if (player.rect.x + player.rect.width > SCREEN_WIDTH)
        player.rect.x = SCREEN_WIDTH - player.rect.width;
}

void FireBullet(Player &player, Bullet &bullet, int &bulletsUsed)
{
    if (IsKeyPressed(KEY_SPACE) && !bullet.active)
    {
        bullet.rect = {player.rect.x + player.rect.width / 2, player.rect.y, 5, 10};
        bullet.active = true;
        bulletsUsed++;
    }
}

void MoveBullet(Bullet &bullet)
{
    if (bullet.active)
    {
        bullet.rect.y -= BULLET_SPEED;
        if (bullet.rect.y < 0)
            bullet.active = false;
    }
}

void UpdateBullet(Player &player, Bullet &bullet, int &bulletsUsed)
{
    FireBullet(player, bullet, bulletsUsed);
    MoveBullet(bullet);
}

void MoveEnemies(Enemy enemies[])
{
    static float dir = 0.5f;

    for (int i = 0; i < MAX_ENEMIES; i++)
        if (enemies[i].alive)
            enemies[i].rect.x += dir;

    if (enemies[0].rect.x < 20 || enemies[4].rect.x > SCREEN_WIDTH - 60)
        dir *= -1;
}

void UpdateEnemies(Enemy enemies[])
{
    MoveEnemies(enemies);
}

void CheckCollisions(Bullet &bullet, Enemy enemies[], int &score)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (bullet.active && enemies[i].alive &&
            CheckCollisionRecs(bullet.rect, enemies[i].rect))
        {
            enemies[i].alive = false;
            bullet.active = false;
            score += 10;
        }
    }
}

void DrawPlayer(Player &player)
{
    DrawRectangleRec(player.rect, ORANGE);
}

void DrawEnemies(Enemy enemies[])
{
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (enemies[i].alive)
            DrawRectangleRec(enemies[i].rect, RED);
}

void DrawBullet(Bullet &bullet)
{
    if (bullet.active)
        DrawRectangleRec(bullet.rect, PINK);
}
// bullet function
int BulletStats(int bullets, int score)
{
    if (bullets == 0)
        return 0;
    return score / bullets;
}

int SaveLoadHighScore(int score)
{
    int saved = 0;

    ifstream inFile("score.txt");
    if (inFile.is_open())
    {
        inFile >> saved;
        inFile.close();
    }

    if (score > saved)
    {
        ofstream outFile("score.txt");
        if (outFile.is_open())
        {
            outFile << score;
            outFile.close();
        }
        return score;
    }

    return saved;
}
