#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <queue>
#include <fstream>

#define INF 10000
#define MAXN 30
#define MAXM 40
#define ID 2023202316

std::array<std::array<bool, MAXM>, MAXN> close;             // 访问情况记录，close列表
std::array<std::array<int, MAXN>, MAXM> valueF;             // 记录每个节点对应的F值
std::array<std::array<std::array<int, 2>, MAXM>, MAXN> pre; // 存储每个节点的父节点
int map[MAXN][MAXM];

void printMap()
{
    for (int i = 0; i < MAXN; i++)
    {
        for (int j = 0; j < MAXM; j++)
        {
            std::cout << map[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
struct Position
{
    int x, y;
};
constexpr std::array<Position, 4> DIRECTIONS = {{
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
    TRAP = -2,
    WALL = -4,
    HEDA = -3,
    BODY = -5
};

bool operator==(const Position &a, const Position &b)
{
    return a.x == b.x && a.y == b.y;
}
bool operator!=(const Position &a, const Position &b)
{
    return a.x != b.x || a.y != b.y;
}
Position operator-(const Position &a, const Position &b)
{
    return {a.x - b.x, a.y - b.y};
}
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
class Node
{
public:
    Position pos;
    int direction;
    int f, g, h;
    Node() : pos({0, 0}), direction(-1), f(INF), g(INF), h(INF) {}
    Node(Position pos, int d, int f, int g, int h) : pos(pos), direction(d), f(f), g(g), h(h) {}
    bool operator<(const Node &node) const { return f > node.f; }
    bool operator==(const Node &node) const { return pos == node.pos; }
    bool operator!=(const Node &node) const { return pos != node.pos; }
};
class Game
{
private:
    Player me;
    int remainingTime;
    int numObjects;
    int numPlayers;
    std::vector<Object> objects;
    std::vector<Player> players;

    bool isValidMove(int x, int y, int, int direction) const;
    void loadPlayer(Player &player);
    void initDistance();
    void loadGameConfig();
    Choose decideNextMove();
    Position Astart(Position p1, Position p2);

public:
    Game() : me{}, remainingTime(0), numObjects(0), numPlayers(0) {}
    Position getHead() const { return me.head; }
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
int Manhattan(int x, int y, int x1, int y1)
{
    return (abs(x - x1) + abs(y - y1)) * 10;
}
Position Game::Astart(Position p1, Position p2)
{
    // std::fstream in("C:\\Cpp\\in.txt");
    // in << "Goal: " << p2.x << " " << p2.y << std::endl;
    // std::cout << "Goal: " << p2.x << " " << p2.y << std::endl;
    bool judge = false;
    for (int i = 0; i < MAXN; i++)
        for (int j = 0; j < MAXM; j++)
            valueF[i][j] = INF;
    std::priority_queue<Node> open;
    Node start(p1, me.direction, 0, 0, 0);
    start.h = Manhattan(p1.x, p1.y, p2.x, p2.y);
    start.f = start.h;
    valueF[p1.x][p1.y] = start.f;
    open.push(start);
    while (!open.empty())
    {
        // in << "-------------------" << std::endl;
        Node cur = open.top();
        open.pop();
        // in << "CurrentPos: " << "[" << cur.pos.x << "," << cur.pos.y << "] " << " F:" << cur.f << std::endl;
        close[cur.pos.x][cur.pos.y] = true;
        for (int i = 0; i < 4; i++)
        {
            int x = cur.pos.x + DIRECTIONS[i].x, y = cur.pos.y + DIRECTIONS[i].y;
            // in << "to: " << "[" << x << " " << y << "]" << std::endl;
            if (!isValidMove(x, y, cur.direction, i) || close[x][y])
            {
                // if (!isValidMove(x, y, cur.direction, i))
                // in << "NotValid: " << cur.direction << " " << i << std::endl;
                // in << "[" << x << " " << y << "]" << "is not ok" << std::endl;
                // std::cout << "[" << x << " " << y << "]" << "is not ok" << std::endl;
                continue;
            }
            int g = cur.g + 10;
            int h = Manhattan(x, y, p2.x, p2.y);
            int f = g + h;
            // in << "Compare: " << valueF[x][y] << " " << f << std::endl;
            if (valueF[x][y] > f)
            {
                // in << "[" << x << " " << y << "]" << "is ok" << std::endl;
                valueF[x][y] = f;
                pre[x][y][0] = cur.pos.x;
                pre[x][y][1] = cur.pos.y;
                open.push(Node({x, y}, i, f, g, h));
                if (x == p2.x && y == p2.y)
                {
                    judge = true;
                    // in << "Find" << std::endl;
                    break;
                }
            }
        }
        if (open.empty())
        {
            // in << "Empty" << std::endl;
            // std::cout << "Empty" << std::endl;
            return p1;
        }
        if (judge)
            break;
    }
    Position t = p2;
    while (p2 != p1)
    {
        t = p2;
        // in << "-------------------" << std::endl;
        // in << "CurrentPos: " << "[" << p2.x << "," << p2.y << "] " << std::endl;
        p2 = {pre[p2.x][p2.y][0], pre[p2.x][p2.y][1]};
    }
    // in << "-------------------" << std::endl;
    // in << "CurrentPos: " << "[" << p2.x << "," << p2.y << "] " << std::endl;
    // in << "Choose: " << t.x << " " << t.y << std::endl;
    return t;
}
bool Game::isValidMove(int x, int y, int now_direction, int cur_direction) const
{
    if (x < 0 || x >= MAXN || y < 0 || y >= MAXM || map[x][y] == WALL || map[x][y] == HEDA || map[x][y] == BODY) //???
    {
        // std::cout << "Out" << std::endl;
        return false;
    }
    // Prevents moving in the opposite direction
    if (cur_direction == (now_direction + 2) % 4)
    {
        // std::cout << "Opposite" << cur_direction << " " << now_direction << std::endl;
        return false;
    }
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
        if (player.id != ID)
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
        // printMap();
        // std::cout << "-------------------" << std::endl;
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
        }
        else
            loadPlayer(player);
    }
    initDistance();
}
Choose Game::decideNextMove()
{
    // printMap();
    // std::cout << "-------------------" << std::endl;
    Choose bestDirection = LEFT;
    if (me.shieldCooldown == 0)
        bestDirection = SHIELD;
    Position goal = objects[0].pos;
    Position pos = Astart(me.head, goal) - me.head;
    if (pos.x == 0 && pos.y == 0 && me.shieldCooldown == 0)
        return SHIELD;
    for (int i = 0; i < 4; i++)
        if (pos == DIRECTIONS[i])
            return static_cast<Choose>(i);
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

int main()
{
    try
    {
        Game game;
        game.play();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}