#include "sudoku.h"
#include <string.h>


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
/**
 * tries to fill a sudoku grid recursively 
 * if it does return the grid else return how it was before
 */

static Grid_T sudoku_try_full(Grid_T g){
    Choice_T c;
    Grid_T g1, attempt;
    int i, j, ok, full;


    c.i = 0; /*initialise the cell*/
    c.j = 0; 
    c.n = 0; /*initialise the value*/

    c = grid_iterate(g, c); /*get the first cell to fill*/
    if(c.n == 0){ /*nothing left chec if full */
        full = 1;
        for(i = 0; i<N; i++){
            if(full){
                for(j = 0; j<N; j++){
                    if(g.cell[i][j].choices[0] == 0){ /*if there is empty cell is not full; break*/
                        full = 0;
                        break;
                    }
                }
                if(!full){break;} 
            }
        }
        if(full){return g;} /*if full return the grid*/
    }

    /*iterate for every choice*/
    while(c.n != 0){
        g1 = grid_update(g, c);
        attempt = sudoku_try_full(g1);
    
        ok = 1;
        for(i = 0; i<N; i++){
            for(j = 0; j<N; j++){
                if(attempt.cell[i][j].choices[0] == 0){
                    ok = 0;
                    break;
                }
            }
            if(!ok){ break; }
        }
    
        if(ok){return attempt;}
    
        /*next choice*/
        c = grid_iterate(g, c);
    }
    
    /*if we didnt find a good grid return g as it was*/
    return g;
}

Grid_T sudoku_generate(int nelts, int unique){
    Grid_T g;
    int i, j, pcount, tries;
    int v[N][N];
    struct {
        int i, j;
    }pos[81]; /*array to store the positions of the cells*/

    for(i = 0; i<N; i++){
        for(j = 0; j<N; j++){
            v[i][j] = 0; /*initialise the grid with 0s*/
        }
    }
    g = grid_init(g, v); /*initialise the grid with the values*/

    srand(getpid()); /*seed the random number generator*/

    g = sudoku_try_full(g); /*try to fill the grid*/

    pcount = 0;
    for(i = 0; i<N; i++){
        for(j = 0; j<N; j++){
            pos[pcount].i = i; /*store the positions of the cells*/
            pos[pcount].j = j;
            pcount++;
        }
    }

    for (i = pcount - 1; i > 0; i--) {
        int r = rand() % (i + 1);
        /* swap pos[i] <-> pos[r] */
        int ti = pos[i].i, tj = pos[i].j;
        pos[i].i = pos[r].i;  
        pos[i].j = pos[r].j;
        pos[r].i = ti;        
        pos[r].j = tj;
    }
    
    /*copy the contents of g to v to do hole-punch the v*/
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            v[i][j] = g.cell[i][j].choices[0];
        }
    }
    /*hole-punch 81-nelts values of the grid*/
    tries = 0;
    do{
        for(i = 0; i < 81 - nelts; i++){
            int ii = pos[i].i;
            int jj = pos[i].j;
            int backup = v[ii][jj]; /* Backup the current value before removing it */
        
            v[ii][jj] = 0;
            /* If we want the puzzle to have a unique solution */
            if(unique){
                Choice_T cTest;
                Grid_T gtest;
                Grid_T solved;
                cTest.i = 0; 
                cTest.j = 0;
                cTest.n = 0; 
                
                gtest = grid_init(gtest, v);
                
                solved = sudoku_solve(gtest, cTest);
                /* If the solved puzzle does not have a unique solution */
                if(!grid_unique(solved)){
                    v[ii][jj] = backup;  /* Restore the original value — don't remove this cell */
                }
            }
        }
        tries++;
    }while(tries < 10 && !grid_unique(grid_init(g, v))); /*try to hole-punch the grid 100 times*/

    g = grid_init(g, v); /*initialise the grid with the values*/
    return g;
}


int  main(int argc, char *argv[]){
    Grid_T g;
    if(argc == 1){ /*if the argumenst are sudoku*/
        if (argc == 1) {
            Choice_T c;
            c.i = 0; /*initialise the cell*/
            c.j = 0;
            c.n = 0; /*initialise the value*/
            g = sudoku_read();
            sudoku_print(stderr, g);
            g = sudoku_solve(g, c);
            printf("Solved:\n");
            sudoku_print(stdout, g);
        }
    }
    else if (argc == 2 && strcmp(argv[1], "-c") == 0) {
        return 0;
    }
    else if(argc == 4 && strcmp(argv[1], "-g") == 0 && strcmp(argv[3], "-u") == 0){ /*if the arguments are sudoku -g nelts -u */

        return 0;
    }
    return 0;
}