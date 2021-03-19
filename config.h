typedef struct config_t {
    int time;
    int dist;
    int nTurns;
    int nTeams;
    int tBreakDown;
    int tMinBox;
    int tMaxBox;
    int fuelTank; 
} config_t;

int load_config(config_t*, char*);
