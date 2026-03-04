/*
Current game logic to impliment:
- Fixed window size. Center it in the view.

- Speed setting. All enemies move down at the same speed.
- When you clear an enemy, the remaining enemies stop moving for some amount of time
    relative to that enemies "difficulty".
- Harder words have higher difficulty.
- When the player gets a certain score, or maintains some kind of chars per min,
    the speed increased by 1.
*/




#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <ncurses.h>
#include <panel.h>
#include <string>
#include <sstream>
#include <vector>

#include "log.h"
#include "words_easy.h"

using namespace std;

struct Enemy {
    int x;
    int y;
    double t;   // time
    float tmax; // max time
    double dtht;  // death time
    string s;   // string
    int idx;    // cursor index
    bool alive;
};

int XMAX;
int YMAX;
WINDOW* WIN;
vector<Enemy> ENEMIES;
int DFPS = 60;

double SPEED = 0.5f;
double MOVET;
int SPAWNFM = 3;
int SPAWNF;

int N_WORDS;

//vector<string> WORDS;

void addEnemy(int x, int y, float tmax, string s) {
    string word = WORDS_EASY[rand() % 11];


    Enemy e;
    e.x = rand() % (XMAX - word.length());
    e.y = 0;
    //e.t = 0;
    //e.tmax = 2.5f;
    e.dtht = 1.f;
    e.s = word;
    e.idx = -1;
    e.alive = true;
    ENEMIES.push_back(e);
}

void update(double dt, char ch) {
    bool move = false;
    MOVET += dt;
    if (MOVET >= SPEED) {
        move = true;
        MOVET = 0.0f;
        
        if (SPAWNF == 0) {
            addEnemy(0, 0, 1.5f, "banana");
            SPAWNF = SPAWNFM;
        }
        SPAWNF -= 1;
    }

    for (Enemy& e: ENEMIES) {
        if (!e.alive) {
            e.dtht -= dt;
            continue;
        }
        
        //e.t += dt;
        if (move) {
            //e.t = 0;
            e.y += 1;
        }
        
        if (ch == e.s[e.idx + 1]) {
            e.idx += 1;
            if (e.idx == e.s.length() - 1) {
                loga(">>CLEARED", e.s);
                e.alive = false;
                e.idx = -1;
            }
        }
        else if (ch != -1) {
            log("reseting");
            e.idx = -1;
        }
    }
    
    
    ENEMIES.erase(
        remove_if(
            ENEMIES.begin(),
            ENEMIES.end(),
            [](Enemy const & e) {return !e.alive && e.dtht <= 0;}
        ),
        ENEMIES.end()
    );
}

void draw(string input, int lfps, int d_lfps) {
    wclear(WIN);
    string dts = to_string(lfps) + " " + to_string(d_lfps);
    string mid (XMAX - (input.length() + 1) - (dts.length() + 1), ' ');
    string bot = " " + input + mid + dts;
    wattron(WIN, WA_STANDOUT);
    mvwaddnstr(WIN, YMAX - 1, 0, bot.c_str(), XMAX);
    wattroff(WIN, WA_STANDOUT);
        
    for (Enemy e: ENEMIES) {
        if (e.alive) {
            mvwaddnstr(WIN, e.y, e.x, e.s.c_str(), e.s.length());
            mvwchgat(WIN, e.y, e.x, e.x + 1 + e.idx, WA_STANDOUT, 0, NULL);
        }
        else {
            string dead (e.s.length(), '-');
            mvwaddnstr(WIN, e.y, e.x, dead.c_str(), XMAX);
        }
    }
        
    wrefresh(WIN);
}

int main (int argc, char* argv[]) {
    clearLog();
    log("===== START TYPING GAME =====");
    
    initscr();
    curs_set(0);
    keypad(stdscr, true);
    nonl();
    noecho();
    nodelay(stdscr, true);
    raw();
    set_escdelay(1);
    getmaxyx(stdscr, YMAX, XMAX);
    loga(to_string(XMAX), to_string(YMAX));
    
    WIN = newwin(YMAX, XMAX, 0, 0);
    
    float df_time = 1.f/DFPS;
    loga("frame time", to_string(df_time));
    
    chrono::time_point now = chrono::high_resolution_clock::now();
    double frameT = 0;
    double d_frameT = 0;
    int frames = 0;
    int d_frames = 0;
    int lfps = 0;
    int d_lfps = 0;
    
    bool running = true;

    MOVET = 0.0f;
    SPAWNF = SPAWNFM;
    
    while (running) {
        chrono::time_point old = now;
        now = chrono::high_resolution_clock::now();
        chrono::duration<double> delta = now - old;
        double dt = delta.count();
        frameT += dt;
        frames += 1;
        
        int ch = getch();
        string input(keyname(ch));
        if (input == "^Q") {
            running = false;
        }
        
        update(dt, ch);
        
        d_frameT += dt;
        if (d_frameT >= df_time) {
            d_frameT = 0;
            d_frames += 1;
            draw(input, lfps, d_lfps);
        }
        
        if (frameT >= 1) {
            lfps = frames;
            frames = 0;
            frameT = 0;
            
            d_lfps = d_frames;
            d_frames = 0;
            d_frameT = 0;
        }
    }
    
    log("===== CLOSING TYPING GAME =====");
    delwin(WIN);
    endwin();
}