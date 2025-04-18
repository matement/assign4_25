#include "sudoku.h"
#include <string.h>
int main(int argc, char *argv[]){
    if(argc == 1 && strcmp(argv[0], "sudoku") == 0){ /*if the argumenst are sudoku*/
        
        return 0;
    }
    else if(argc == 2 && (strcmp(argv[0], "sudoku") == 0 && strcmp(argv[1], "-c") == 0)){ /*if the arguments are sudoku -c*/

        return 0;
    }
    else{ /*if the arguments are sudoku -g nelts -u */

        return 0;
    }

}