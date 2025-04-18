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

    for(i = 0; i<9; i++){
        for(j = 0; j<9; j++){
            if(v[i][j] == 0){ /*if the value is 0 ithe cell doesnt have a valuse and has all posible values from 1 to 9*/
                g.cell[i][j].choices[0] = 0;
                g.cell[i][j].count = 9;
                for(k = 1; k<10; k++){  /*set all the values from 1 to 9 as possible values*/
                    g.cell[i][j].choices[k] = 1;
                }
            }
            else{   /*if it isnt zero then call grid_update_value to initalise the sigle value*/
                g = grid_update_value(g, i, j, v[i][j]);
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
                n = 1;
            }
            j = 0; /*reset the column*/
        }
    }
    /*if there is no value return (0, 0, 0)*/
    none.i = 0;
    none.j = 0;
    none.n = 0;
    return none;
}