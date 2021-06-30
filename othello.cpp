#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm> 
#define DEBUG 0
#define BATTLE 1

const int depth = 6;

struct Point {
    int x, y;
    Point() : Point(0, 0) {}
    Point(float x, float y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};

class OthelloBoard
{
    friend class SolveBoard;
private:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };

    static const int SIZE = 8;
    const std::array<Point, 8> directions{ {
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    } };

    std::array<std::array<int, SIZE>, SIZE> board;
    std::array<int, 3> disc_count;
    std::vector<Point> next_valid_spots;

public:
    int cur_player;

    OthelloBoard(std::ifstream& fin)
    {
        read_board(fin);
        read_valid_spots(fin);
    }
    OthelloBoard() {}
    OthelloBoard(OthelloBoard& src)
    {
        // copy constructor
        this->board = src.board;
        this->disc_count = src.disc_count;
        this->next_valid_spots = src.next_valid_spots;
        this->cur_player = src.cur_player;
    }

private:
    void read_board(std::ifstream& fin) {
        fin >> cur_player;
        disc_count[EMPTY] = disc_count[BLACK] = disc_count[WHITE] = 0;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                fin >> board[i][j];
                disc_count[board[i][j]]++;
            }
        }
    }
    void read_valid_spots(std::ifstream& fin) {
        int n_valid_spots;
        fin >> n_valid_spots;
        int x, y;
        for (int i = 0; i < n_valid_spots; i++) {
            fin >> x >> y;
            next_valid_spots.push_back({ (float)x, (float)y });
        }
    }
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        // want to place disc at center
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir : directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir : directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({ p });
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s : discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    void put_disc(Point p)
    {
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
    }
    bool onFrontier(Point p)
    {
        if (get_disc(p) == EMPTY)
            return false;

        // surrounded by other discs
        for (int i = 0; i < 8; i++)
        {
            Point nextP = p + directions[i];
            if (is_spot_on_board(nextP) && get_disc(nextP) != EMPTY)
                return true;
        }
        return false;
    }
    void printBoard(std::ofstream& log)
    {
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
                log << board[i][j] << " ";
            log << std::endl;
        }
        log << std::endl;
    }
};

class SolveBoard
{
private:
    int me;

    int stateValue(OthelloBoard& ob, Point spot)
    {
        int opponent = ob.get_next_player(me);
        double piececount, sumis, Xs, Cs, potentialSumis, potentialXs, potentialCs, edges, frontier, mobility;

        // piece count
        piececount = (100.0 * ob.disc_count[me]) / ((double)ob.disc_count[ob.BLACK] + (double)ob.disc_count[ob.WHITE]);

        // sumis
        int mineSumi = 0;
        int oppSumi = 0;
        int mineX = 0;
        int oppX = 0;
        int mineC = 0;
        int oppC = 0;
        Point sumi[4] = { {0, 0}, {0, 7}, {7, 0}, {7, 7} };
        Point X[4] = { { 1, 1 }, { 1, 6 }, { 6, 1 }, { 6, 6 } };
        Point C[8] = { { 0, 1 },  { 0, 6 }, { 1, 0 }, { 1, 7 }, { 6, 0 }, { 6, 7 }, { 7, 1 }, { 7, 6 } };
        for (int i = 0; i < 8; i++)
        {
            if (i < 4)
            {
                if (ob.get_disc(sumi[i]) == me)
                    mineSumi++;
                else if (ob.get_disc(sumi[i]) == opponent)
                    oppSumi++;
                if (ob.get_disc(X[i]) == me)
                    mineX++;
                else if (ob.get_disc(X[i]) == opponent)
                    oppX++;
            }
            if (ob.get_disc(C[i]) == me)
                mineC++;
            else if (ob.get_disc(C[i]) == opponent)
                oppC++;
        }
        sumis = 25.0 * ((double)mineSumi - (double)oppSumi);
        Xs = -25.0 * ((double)mineX - (double)oppX);
        Cs = -12.5 * ((double)mineC - (double)oppC);

        //edges and frontier
        int myEdges = 0, oppEdges = 0;
        int myFrontier = 0, oppFrontier = 0;
        for (int i = 0; i < ob.SIZE; i++)
        {
            for (int j = 0; j < ob.SIZE; j++)
            {
                Point spot(i, j);
                if (i == 0 || i == ob.SIZE - 1 || j == 0 || j == ob.SIZE - 1)
                {
                    if (ob.get_disc(spot) == me)
                        myEdges++;
                    else if (ob.get_disc(spot) == opponent)
                        oppEdges++;
                }
                else if (ob.get_disc(spot) != ob.EMPTY)
                {
                    if (ob.onFrontier(spot))
                    {
                        if (ob.get_disc(spot) == me)
                            myFrontier++;
                        else if (ob.get_disc(spot) == opponent)
                            oppFrontier++;
                    }
                }
            }
        }
        if (myEdges == 0 && oppEdges == 0)
            edges = 50.0;
        else
            edges = 100.0 * myEdges / ((double)myEdges + (double)oppEdges);
        frontier = -100 * ((double)myFrontier - (double)oppFrontier); // frontier pieces are bad!

        // mobility
        std::vector<Point> current_spots = ob.next_valid_spots;
        std::vector<Point> next_spots = ob.get_valid_spots();

        int valid_spots_me_size = -1;
        if (ob.cur_player == me)
            valid_spots_me_size = current_spots.size();
        else if (ob.cur_player == opponent)
            valid_spots_me_size = next_spots.size();

        if (current_spots.size() == 0 && next_spots.size() == 0)
            mobility = 50.0;
        else
            mobility = 100.0 * valid_spots_me_size / ((double)current_spots.size() + (double)next_spots.size());

        // potential corners & Xs & Cs (pseudo-expand node)
        int nextSumi = 0;
        int nextX = 0;
        int nextC = 0;
        for (int i = 0; i < (int)next_spots.size(); i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (j < 4)
                {
                    if (next_spots[i] == sumi[i])
                        nextSumi++;
                    if (next_spots[i] == X[i])
                        nextX++;
                }
                if (next_spots[i] == C[i])
                    nextC++;
            }
        }

        int times = 0;
        if (ob.cur_player == me)
            times = 1;
        else if (ob.cur_player == opponent)
            times = -1;
        potentialSumis = -25.0 * nextSumi * times;
        potentialXs = 25.0 * nextX * times;
        potentialCs = 12.5 * nextC * times;

        // set weights of heuristic parameters
        int pW = 2 * (ob.disc_count[ob.BLACK] + ob.disc_count[ob.WHITE]);
        int sW = 5000;
        for (int i = 0; i < 4; i++)
        {
            if (spot == sumi[i] && ob.cur_player == me)
                sW = 20000;
        }
        int xW = 5000;
        int cW = 2500;
        int psW = 2000; // 2000
        int pxW = 2000;
        int pcW = 1000;
        int fW = 200;
        int eW = 50; // 50
        int mW = 10 * (100 - (ob.disc_count[ob.BLACK] + ob.disc_count[ob.WHITE]));
        int value = pW * piececount + sW * sumis + xW * Xs + cW * Cs
            + psW * potentialSumis + pxW * potentialXs + pcW * potentialCs
            + eW * edges + fW * frontier + mW * mobility;
        return value;
    }

    void write_valid_spot(std::ofstream& fout, Point p) {
        srand(time(NULL));
        fout << p.x << " " << p.y << std::endl;
        fout.flush();
    }

    int alphabeta(/*std::ofstream& log,*/ OthelloBoard& ob, Point spot, int depth, int alpha, int beta, bool maximisingPlayer)
    {
        if (depth == 0)
        {
#if DEBUG == 1
            log << "depth 0:" << stateValue(ob, spot) << std::endl;
            //ob.printBoard(log);
#endif
            return stateValue(ob, spot);
        }

        int value = -1;
        ob.cur_player = ob.get_next_player(ob.cur_player);
        ob.next_valid_spots = ob.get_valid_spots();
        if (ob.next_valid_spots.empty())
            value = stateValue(ob, spot);

        for (auto spot : ob.next_valid_spots)
        {
            OthelloBoard nextBoard = ob;
            nextBoard.put_disc(spot);
#if DEBUG == 1
            log << "(" << spot.x << ", " << spot.y << "): " << std::endl;
            //nextBoard.printBoard(log);
#endif
            if (maximisingPlayer)
            {
                alpha = std::max(alpha, alphabeta(/*log,*/ nextBoard, spot, depth - 1, alpha, beta, false));
                value = alpha;
                if (alpha >= beta)
                    break;
            }
            else
            {
                beta = std::min(beta, alphabeta(/*log,*/ nextBoard, spot, depth - 1, alpha, beta, true));
                value = beta;
                if (beta <= alpha)
                    break;
            }
        }
#if DEBUG == 1
        log << "depth " << depth << ": " << std::endl;
        if (maximisingPlayer)
            log << "find max" << std::endl;
        else
            log << "find min" << std::endl;
        log << "alpha = " << alpha << ", beta = " << beta << std::endl;
        log << "value = " << value << std::endl << std::endl << std::endl;
#endif
        return value;
    }

public:
    SolveBoard(int me)
    {
        this->me = me;
    }

    void solve(/*std::ofstream& log,*/ std::ofstream& fout, OthelloBoard& ob)
    {
        int alpha = INT_MIN;
        //int i = 1;
        //log << "next_valid_spots size = " << ob.next_valid_spots.size() << std::endl;
        for (auto spot : ob.next_valid_spots)
        {
            //log << "spot " << i++ << ": (" << spot.x << ", " << spot.y << ")" << std::endl;
            OthelloBoard nextBoard = ob;
            nextBoard.put_disc(spot);
            int value = alphabeta(/*log,*/ nextBoard, spot, depth, alpha, INT_MAX, false);
#if DEBUG == 1 || DEBUG == 2
            log << std::endl << std::endl;
            log << "alpha = " << alpha << std::endl;
            log << "value = " << value << std::endl << std::endl;
#endif
            if (value > alpha)
            {
                alpha = value;
                write_valid_spot(fout, spot);
#if DEBUG == 1 || DEBUG == 2
                log << "nextspot: " << std::endl;
                write_valid_spot(log, spot);
#endif
            }
        }
        //log << "search finish" << std::endl << std::endl;
    }
};


int main(int, char** argv) {
#if BATTLE == 1
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    //std::ofstream log("mmlog.txt", std::ofstream::app);
#else
    std::ifstream fin("in.txt");
    std::ofstream fout("out.txt");
    std::ofstream log("mmlog.txt");
#endif
    OthelloBoard game(fin);
    SolveBoard solveBoard(game.cur_player);
    solveBoard.solve(/*log,*/ fout, game);
    fin.close();
    fout.close();
    //log.close();
    return 0;
}
