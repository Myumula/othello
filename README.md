# othello
Othello AI

C:\Users\user\Downloads\Mini-Project 3 Package_1\pj
main.exe hw1.exe baseline4.exe

depth 6 + X + C:
1. weight modified: 5000->20000, 2000->8000
b5->great fail

2. weight modified: 5000->20000 when me can hit sumi(stateValue)
        for (int i = 0; i < 4; i++)
        {
            if (spot == sumi[i] && ob.cur_player == me)
                sW = 20000;
        }
b5->win, but didnt hit sumi at the first time
b4->win
b1->win, 4~5cases didn't run through(11, 16, 19...)
b2(reverse)->fail, the others->win

3. weight modified:(solve)
            Point sumi[4] = { {0, 0}, {0, 7}, {7, 0}, {7, 7} };
            for (int i = 0; i < 4; i++)
            {
                if (spot == sumi[i])
                    value *= 2;
            }
b5->draw, hit sumi at the first time


