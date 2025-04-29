#include "sudoku.h"
#include <string.h>


static int count_solutions(Grid_T g, Choice_T c){
    Choice_T next, last, zero;
    Grid_T   g2;
    int      cnt = 0;

    zero.i = zero.j = zero.n = 0;

    /* 1) Apply all forced singles */
    next = grid_exist_unique(g);
    while (next.n != 0) {
        g = grid_update(g, next);
        next = grid_exist_unique(g);
    }

    /* 2) If the grid is full, we’ve found one solution */
    if (grid_iterate(g, zero).n == 0)
        return 1;

    /* 3) Otherwise backtrack, accumulating counts up to 2 */
    last = c;
    next = grid_iterate(g, last);
    while (next.n != 0 && cnt < 2) {
        g2    = grid_update(g, next);
        cnt  += count_solutions(g2, zero);
        last  = next;
        next  = grid_iterate(g, last);
    }

    return cnt;
}

Grid_T sudoku_read(void){
    int v[N][N];
    Grid_T g;
    int i, j;

    for(i = 0; i<N; i++){
        for(j = 0; j<N; j++){
            if(scanf("%d", &v[i][j]) !=1){
                fprintf(stderr, "Error reading puzzle");    /*read the values from the file*/
                exit(1); 
            }
        }
    }

    g = grid_init(g, v); /*initialise the grid with the values*/
    return g;
}

void sudoku_print(FILE *s, Grid_T g){
    int i, j;

    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            fprintf(s, "%d", g.cell[i][j].choices[0]);
            if (j < 8) {
                fprintf(s, " "); /* space between the cells*/
            }
        }
        fprintf(s, "\n"); /*end the */
    }
}

Grid_T sudoku_solve(Grid_T g, Choice_T c)
{
    Choice_T next, last, zero;
    Grid_T   g2, attempt;

    /* zero choice for resets and completion checks */
    zero.i = zero.j = zero.n = 0;

    /* 1) Deterministic fill of all forced cells */
    next = grid_exist_unique(g);
    while (next.n != 0) {
        g = grid_update(g, next);
        g.unique = 1;
        next = grid_exist_unique(g);
    }

    /* 2) If grid is complete, return it */
    if (grid_iterate(g, zero).n == 0)
        return g;

    /* 3) Backtracking: start from c (initially zero), then advance */
    last = c;
    next = grid_iterate(g, last);
    while (next.n != 0) {
        g2      = grid_update(g, next);
        attempt = sudoku_solve(g2, zero);
        /* if solved, propagate up */
        if (grid_iterate(attempt, zero).n == 0)
            return attempt;
        /* else try next candidate */
        last = next;
        next = grid_iterate(g, last);
    }

    /* 4) all candidates exhausted: failure, return original */
    return g;
}

Grid_T sudoku_generate(int nelts, int unique) {
    Grid_T full, candidate, solved;
    int    base[9][9], work[9][9], pos[81];
    Choice_T zero = {0,0,0};
    int d, i, j, k, m, t;

    /* 1) get one complete solution */
    memset(base, 0, sizeof base);
    full = grid_init(full, base);
    full = sudoku_solve(full, zero);

    /* 2) seed RNG */
    srand((unsigned)time(NULL) ^ (unsigned)getpid());

    /* 3) digit‐permute to randomize the solved grid */
    {
      int map[10];
      for(d=1; d<=9; d++) map[d] = d;
      for(d=9; d>1; d--){
        int r = rand()%d + 1;
        int tmp = map[d]; map[d]=map[r]; map[r]=tmp;
      }
      for(i=0;i<9;i++)
        for(j=0;j<9;j++){
          int old = full.cell[i][j].choices[0];
          full.cell[i][j].choices[0] = map[old];
        }
    }

    /* 4) copy that into base[][] */
    for(i=0;i<9;i++)
      for(j=0;j<9;j++)
        base[i][j] = full.cell[i][j].choices[0];

    /* 5) build & shuffle hole‐positions */
    for(k=0;k<81;k++) pos[k] = k;
    for(k=80;k>0;k--){
      int r = rand()%(k+1);
      int t = pos[k]; pos[k]=pos[r]; pos[r]=t;
    }

    /* 6) punch holes, undo non‐unique if needed */
    for(t=0; t<10; t++){
      memcpy(work, base, sizeof work);
      for(m=0; m<81-nelts; m++){
        int x = pos[m]/9, y = pos[m]%9;
        int backup = work[x][y];
        work[x][y] = 0;
        if(unique){
          candidate = grid_init(candidate, work);
          solved    = sudoku_solve(candidate, zero);
          if(!grid_unique(solved))
            work[x][y] = backup;
        }
      }
      if(!unique) break;
      candidate = grid_init(candidate, work);
      solved    = sudoku_solve(candidate, zero);
      if(grid_unique(solved)) break;
    }

    /* 7) return that puzzle */
    return grid_init(full, work);
}
int sudoku_is_correct(Grid_T g) { /* Check puzzle correctness: no duplicate non-zero entries */
    int row[9][10] = { {0} },
        col[9][10] = { {0} },
        box[9][10] = { {0} };
    int i, j, v;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            v = g.cell[i][j].choices[0];
            if (v != 0) {
                if (v < 1 || v > 9)
                    return 0;
                if (row[i][v]++)
                    return 0;
                if (col[j][v]++)
                    return 0;
                if (box[(i/3)*3 + (j/3)][v]++)
                    return 0;
            }
        }
    }
    return 1;
}


/* Print all row/col/block conflicts in order:
    row 1, col 1, block 1, row 2, col 2, block 2, …, row 9, col 9, block 9 */
    void sudoku_print_errors(Grid_T g) {
    int pos[9], pcnt;
    int k, i, j, v;
    int bi, bj, di, dj;

    for (k = 0; k < 9; k++) {
        /* 1) Row k */
        for (v = 1; v <= 9; v++) {
            pcnt = 0;
            for (j = 0; j < 9; j++) {
                if (g.cell[k][j].choices[0] == v)
                    pos[pcnt++] = j + 1;
            }
            if (pcnt > 1) {
                fprintf(stderr, "In row %d cells %d", k+1, pos[0]);
                if (pcnt == 2) {
                    fprintf(stderr, " and %d", pos[1]);
                } else {
                    for (i = 1; i < pcnt-1; i++)
                        fprintf(stderr, ", %d", pos[i]);
                    fprintf(stderr, ", and %d", pos[pcnt-1]);
                }
                fprintf(stderr, " are the same (%d)\n", v);
            }
        }

        /* 2) Column k */
        for (v = 1; v <= 9; v++) {
            pcnt = 0;
            for (i = 0; i < 9; i++) {
                if (g.cell[i][k].choices[0] == v)
                    pos[pcnt++] = i + 1;
            }
            if (pcnt > 1) {
                fprintf(stderr, "In column %d cells %d", k+1, pos[0]);
                if (pcnt == 2) {
                    fprintf(stderr, " and %d", pos[1]);
                } else {
                    for (j = 1; j < pcnt-1; j++)
                        fprintf(stderr, ", %d", pos[j]);
                    fprintf(stderr, ", and %d", pos[pcnt-1]);
                }
                fprintf(stderr, " are the same (%d)\n", v);
            }
        }

        /* 3) Block k */
        /* blockRow = k/3, blockCol = k%3 */
        bi = (k/3)*3;
        bj = (k%3)*3;
        for (v = 1; v <= 9; v++) {
            pcnt = 0;
            for (di = 0; di < 3; di++) {
                for (dj = 0; dj < 3; dj++) {
                    if (g.cell[bi+di][bj+dj].choices[0] == v)
                        pos[pcnt++] = di*3 + dj + 1;
                }
            }
            if (pcnt > 1) {
                fprintf(stderr, "In block %d cells %d", k+1, pos[0]);
                if (pcnt == 2) {
                    fprintf(stderr, " and %d", pos[1]);
                } else {
                    for (i = 1; i < pcnt-1; i++)
                        fprintf(stderr, ", %d", pos[i]);
                    fprintf(stderr, ", and %d", pos[pcnt-1]);
                }
                fprintf(stderr, " are the same (%d)\n", v);
            }
        }
    }
}

int  main(int argc, char *argv[]){
    Grid_T g, sol;
    Choice_T c, zero;
    int nelts, unique_flag;
    int sol_count;

    /* initialize Choice_T */
    c.i = c.j = c.n = 0;
    zero = c;

    if(argc == 1){ /*if the argumenst are sudoku*/
        g = sudoku_read();
        /* 2) Echo input */
        fprintf(stderr, "Input puzzle:\n");
        sudoku_print(stderr, g);
        /* 3) Count solutions up to 2 */
        sol_count = count_solutions(g, c);
        if (sol_count == 0) {
            fprintf(stderr, "No solution\n");
        } else if (sol_count == 1) {
            fprintf(stderr, "Unique solution\n");
        } else {
            fprintf(stderr, "Multiple solutions (%d found)\n", sol_count);
        }
        /* 4) Get one solution or attempted grid */
        sol = sudoku_solve(g, zero);
        /* 5) Print to stdout */
        sudoku_print(stdout, sol);

    }
    else if (argc == 2 && strcmp(argv[1], "-c") == 0) {
        /* Check mode */
        g = sudoku_read();
        fprintf(stderr, "Input puzzle:\n");
        sudoku_print(stderr, g);
        if (sudoku_is_correct(g)) {
            fprintf(stderr, "Puzzle is correct.\n");
        } 
        else {
            fprintf(stderr, "Puzzle is incorrect. Errors:\n");
            sudoku_print_errors(g);
        }

        return 0;
    }
    else if (strcmp(argv[1], "-g") == 0) {
        if (argc < 3 || argc > 4) {
            fprintf(stderr, "Usage: %s -g nelts [-u]\n", argv[0]);
            return 1;
        }
        nelts = atoi(argv[2]);
        unique_flag = (argc == 4 && strcmp(argv[3], "-u") == 0);
    
        g = sudoku_generate(nelts, unique_flag);
        sudoku_print(stdout, g);
        return 0;
    }
    return 0;
}