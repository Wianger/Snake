#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

#define INF 10000
#define MAXN 30
#define MAXM 40
#define ID 2023202316

constexpr std::array<std::array<int, 2>, 4> DIRECTIONS = {{
    {0, -1}, // Left
    {-1, 0}, // Up
    {0, 1},  // Right
    {1, 0}   // Down
}};
enum Choose
{
    LEFT = 0,
    UP = 1,
    RIGHT = 2,
    DOWN = 3,
    SHIELD = 4
};
enum ObjectType
{
    GROW_BEAN = -1,
    ONE_BEAN = 1,
    TWO_BEAN = 2,
    THREE_BEAN = 3,
    FIVE_BEAN = 5,
    TRAP = -1,
    WALL = -4,
    HEDA = -3,
    BODY = -5
};
struct Position
{
    int x, y;
};
struct Object
{
    Position pos;
    int value;
    int distance;
};
struct Player
{
    int id, length, score, direction, shieldCooldown, shieldTime;
    Position head;
    std::vector<Position> body;
};
class Game
{
private:
    std::array<std::array<int, MAXN>, MAXM> map;
    Player me;
    int remainingTime;
    int numObjects;
    int numPlayers;
    std::vector<Object> objects;
    std::vector<Player> players;
    std::vector<Position> checkDanger();

    bool isValidMove(int x, int y, int direction) const;
    void loadPlayer(Player &player);
    void initDistance();
    void loadGameConfig();
    Choose decideNextMove();

public:
    Game() : map{}, me{}, remainingTime(0), numObjects(0), numPlayers(0) {}
    void play();
};

bool cmp(const Object &a, const Object &b)
{
    if (a.value < -1 && b.value >= -1)
        return false;
    else if (b.value < -1 && a.value >= -1)
        return true;
    else if (a.value < -1 && b.value < -1)
        return a.distance > b.distance;
    return a.distance < b.distance;
}
bool Game::isValidMove(int x, int y, int direction) const
{
    if (x < 0 || x >= MAXN || y < 0 || y >= MAXM || map[x][y] == WALL || map[x][y] == TRAP || map[x][y] == BODY) //???
        return false;
    // Prevents moving in the opposite direction
    if (direction == (me.direction + 2) % 4)
        return false;
    return true;
}
void Game::loadPlayer(Player &player)
{
    std::cin >> player.head.x >> player.head.y;
    player.body.clear();
    player.body.push_back(player.head);
    map[player.head.x][player.head.y] = HEDA;
    for (int j = 1; j < player.length; ++j)
    {
        Position segment;
        std::cin >> segment.x >> segment.y;
        player.body.push_back(segment);
        map[segment.x][segment.y] = BODY;
    }
}
void Game::loadGameConfig()
{
    std::cin >> remainingTime >> numObjects;
    objects.resize(numObjects);
    for (auto &obj : objects)
    {
        std::cin >> obj.pos.x >> obj.pos.y >> obj.value;
        map[obj.pos.x][obj.pos.y] = obj.value; // Reflect game objects on the map
    }

    std::cin >> numPlayers;
    players.resize(numPlayers);
    for (auto &player : players)
    {
        std::cin >> player.id >> player.length >> player.score >> player.direction >> player.shieldCooldown >> player.shieldTime;
        if (player.id == ID)
        {
            me = player;
            loadPlayer(me);
            initDistance();
        }
        else
            loadPlayer(player);
    }
}
Choose Game::decideNextMove()
{
    Choose bestDirection;
    std::vector<Position> danger = checkDanger();
    if (danger.size() > 0)
    {
        if (me.shieldCooldown == 0)
            return SHIELD;
        else
        {
        }
    }

    Position goal = objects[0].pos;
    return bestDirection;
}
void Game::play()
{
    loadGameConfig();
    std::cout << decideNextMove() << std::endl;
}
void Game::initDistance()
{
    for (auto &obj : objects)
        obj.distance = std::abs(obj.pos.x - me.head.x) + std::abs(obj.pos.y - me.head.y);
    std::sort(objects.begin(), objects.end(), cmp);
}
std::vector<Position> Game::checkDanger()
{
    std::vector<Position> danger;
    int x = me.head.x + DIRECTIONS[me.direction][0] * 2, y = me.head.y + DIRECTIONS[me.direction][1] * 2;
    if (map[x][y] == HEDA)
        danger.push_back({x, y});
    x = me.head.x + DIRECTIONS[me.direction][0], y = me.head.y + DIRECTIONS[me.direction][1];
    if (map[x][y] == HEDA)
        danger.push_back({x, y});
    x += DIRECTIONS[(me.direction + 1) % 4][0], y += DIRECTIONS[(me.direction + 1) % 4][1];
    if (map[x][y] == HEDA)
        danger.push_back({x, y});
    x += DIRECTIONS[(me.direction + 3) % 4][0] * 2, y += DIRECTIONS[(me.direction + 3) % 4][1] * 2;
    if (map[x][y] == HEDA)
        danger.push_back({x, y});
    for (int i = 1; i <= 2; i++)
    {
        x = me.head.x + DIRECTIONS[(me.direction + 1) % 4][0] * i, y = me.head.y + DIRECTIONS[(me.direction + 1) % 4][1] * i;
        if (map[x][y] == HEDA)
            danger.push_back({x, y});
        x = me.head.x + DIRECTIONS[(me.direction + 3) % 4][0] * i, y = me.head.y + DIRECTIONS[(me.direction + 3) % 4][1] * i;
        if (map[x][y] == HEDA)
            danger.push_back({x, y});
    }
    x = me.head.x + DIRECTIONS[(me.direction + 2) % 4][0], y = me.head.y + DIRECTIONS[(me.direction + 2) % 4][1];
    x += DIRECTIONS[(me.direction + 1) % 4][0], y += DIRECTIONS[(me.direction + 1) % 4][1];
    if (map[x][y] == HEDA)
        danger.push_back({x, y});
    x += DIRECTIONS[(me.direction + 3) % 4][0] * 2, y += DIRECTIONS[(me.direction + 3) % 4][1] * 2;
    if (map[x][y] == HEDA)
        danger.push_back({x, y});
    return danger;
}