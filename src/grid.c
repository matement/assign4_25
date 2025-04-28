#include "grid.h"

static Grid_T grid_update_value(Grid_T h, int i, int j, int n){ /*update  a single value*/
    int k;

    h.cell[i][j].choices[0] = n;    /*put the cosen value to choices [0]*/

    for(k = 1; k<10; k++){  /*set all the other values to 0*/
        h.cell[i][j].choices[k] = 0;
    }

    h.cell[i][j].count = 0; /*set the count to 0 since it cant change the value*/
    return h;
}

Grid_T grid_init(Grid_T g, int v[9][9]){ /*initialises the grid*/
    int i, j, k;
    g.unique = 0;

    /* First pass: set empty cells to all choices, and givens to their value */
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            if (v[i][j] == 0) { /* empty: all choices 1..9 */
                g.cell[i][j].choices[0] = 0;
                g.cell[i][j].count = 9;
                for (k = 1; k < 10; k++) {
                    g.cell[i][j].choices[k] = 1;
                }
            } else { /* given: set only that value */
                g = grid_update_value(g, i, j, v[i][j]);
            }
        }
    }

    /* Second pass: eliminate givens from peers */
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            int val = v[i][j];
            if (val != 0) {
                Choice_T c;
                c.i = i;
                c.j = j;
                c.n = val;
                g = grid_update(g, c);
            }
        }
    }

    return g;
}

Grid_T grid_update(Grid_T g, Choice_T c){	/* update value of c.i,c.j to c.n and eliminate c from choices in grid */
    int i, j;
    int row, col, sRow, sCol; /*get the row and column of the cell*/
    int val;

    row = c.i; /*get the row and column of the cell*/
    col = c.j;
    val = c.n; /*get the value of the cell*/

    g = grid_update_value(g, row, col, val); /*update the value of the cell*/

    for(j = 0; j<9; j++){ /*eliminate the value from the row*/
        if(j != col && g.cell[row][j].choices[val] == 1){ 
            g.cell[row][j].choices[val] = 0; 
            g.cell[row][j].count--; 
        }
    }

    for(i = 0; i<9; i++){
        if(i != row && g.cell[i][col].choices[val] == 1){ /*eliminate the value from the column*/
            g.cell[i][col].choices[val] = 0; 
            g.cell[i][col].count--; 
        }
    }

    /*get the starting row and column of the subgrid*/
    sRow = (row/3)*3; 
    sCol = (col/3)*3;
    
    for(i = sRow; i<sRow+3; i++){
        for(j = sCol; j<sCol+3; j++){
            if ((i != row || j != col) && g.cell[i][j].choices[val] == 1){ /*eliminate the value from the subgrid*/
                g.cell[i][j].choices[val] = 0; 
                g.cell[i][j].count--; 
            }
        }
    }

    return g; /*return the updated grid*/
}


Choice_T grid_iterate(Grid_T g, Choice_T t){	/* iterate over all choices in all cells*/
    int i, j, n;
    Choice_T none;
    /*search for a unique choice without backtracking*/
    for(i = 0; i<9; i++){
        for(j = 0; j<9; j++){
            if(g.cell[i][j].choices[0] == 0 && g.cell[i][j].count == 1){ /*if the cell doenst have a value and there is at least 1 choice*/
                for(n = 1; n<10; n++){ 
                    if (g.cell[i][j].choices[n] == 1) {
                        Choice_T c;
                        c.i = i; /*get the row and column of the cell*/
                        c.j = j;
                        c.n = n; /*get the value of the cell*/
                        return c;
                    }
                }
            }
        }
    }
    
    /*if there is no unique solution then to normal iteration*/
    i = t.i; 
    j = t.j;
    n = t.n + 1; /*get the values of the cell*/

    for(; i<9; i++){
        for(; j<9; j++){
            for(; n<10; n++){
                if(g.cell[i][j].choices[0] == 0 && g.cell[i][j].choices[n] == 1){/*if there is no value in [i, j] and the value n is available*/
                    Choice_T c;    
                    c.i = i; /*get the row and column of the cell*/
                    c.j = j;
                    c.n = n; /*get the value of the cell*/ 
                    return c; 
                }
            }
            n = 1; /*reset the value*/
        }
        j = 0; /*reset the column*/
    }
    /*if there is no value return (0, 0, 0)*/
    none.i = 0;
    none.j = 0;
    none.n = 0;
    return none;
}

int grid_unique(Grid_T g){ /*return the unique flag for g*/
    return g.unique; 
}

Choice_T grid_exist_unique(Grid_T g){ /*return a cell with a unique choice, if one exists, otherwise return (0,0,0)*/
    int i, j, k;
    Choice_T none;
    none.i = none.j = none.n = 0;

    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            /* if cell is empty and exactly one choice remains */
            if (g.cell[i][j].choices[0] == 0
             && g.cell[i][j].count    == 1) {

                Choice_T c;
                c.i = i;
                c.j = j;
                /* find which value k (1..9) is left */
                for (k = 1; k <= 9; k++) {
                    if (g.cell[i][j].choices[k] == 1) {
                        c.n = k;
                        break;
                    }
                }
                return c;
            }
        }
    }

    return none;
}

Grid_T grid_clear_unique(Grid_T g) {
    g.unique = 0;
    return g;
}

Choice_T grid_read_value(Grid_T g, Choice_T c){ /*return value of i,j*/
    Choice_T r;
    r.i = c.i; /*get the row and column of the cell*/
    r.j = c.j;
    r.n = g.cell[c.i][c.j].choices[0]; /*get the value of the cell*/
    return r;
}