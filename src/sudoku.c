#include "sudoku.h"
#include <string.h>

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
        g1 = grid_update(g, c); /*update the grid with the value*/
        attempt = sudoku_try_full(g1); /*try to fill the grid*/

        ok = 1;
        for(i = 0; i<N; i++){
            for(j = 0; j<N; j++){
                if(attempt.cell[i][j].choices[0] == 0){
                    ok = 0; /*if there is an empty cell set ok to 0*/
                    break;
                }
            }
            if(!ok){break;}
        }
    }
    /*if we didnt find a good grid return g as it was*/
    return g;
}

Grid_T sudoku_generate(int nelts, int unique){
    Grid_T g;
    int i, j, pcount;
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

    /*shuffle to later remove 81-nelts things*/
    for(i = pcount-1; i<=0; i--){
        int r = rand() % (i+1);
        int tempi = pos[i].i; /*swap the positions of the cells*/
        int tempj = pos[i].j;
        
        pos[i].i = pos[r].i;
        pos[i].j = pos[r].j;
    }

    return g;
}


int  main(int argc, char *argv[]){
    if(argc == 1){ /*if the argumenst are sudoku*/
        Grid_T g;
        g = sudoku_generate(81, 1);/*generate a sudoku with 81 elements and unique*/
        return 0;
    }
    else if(argc == 2 && (strcmp(argv[0], "sudoku") == 0 && strcmp(argv[1], "-c") == 0)){ /*if the arguments are sudoku -c*/

        return 0;
    }
    else if(argc == 4 && strcmp(argv[1], "-g") == 0 && strcmp(argv[3], "-u") == 0){ /*if the arguments are sudoku -g nelts -u */

        return 0;
    }
    return 0;
}