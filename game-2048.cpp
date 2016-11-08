/*
* Baron Khan
* baron.khan14@imperial.ac.uk
* bak14
*
* References:
* 1. Integer Length - http://stackoverflow.com/questions/22648978/c-how-to-find-the-length-of-an-integer - Accessed on 28/2/15.
* 2. Random Number Generation - http://www.cplusplus.com/reference/cstdlib/rand/ - Accessed on 3/3/15.
* 3. Initialise Random Number Generation - http://www.cplusplus.com/reference/cstdlib/srand/ - Accessed on 3/3/15.
*/

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>           //Used to place a 2 in a random empty space on the grid after each move.
#include <ctime>             //Used to randomise the seed based on the current time. See Reference 3.

#define MOVING_LEFT false
#define MOVING_RIGHT true
#define MOVING_UP false
#define MOVING_DOWN true

using namespace std;

void draw_grid(const vector<int>& grid);
int integer_length(int n);
bool execute_command(vector<int>& grid, string command);

bool move_row(vector<int>& grid, int row_start_index, bool moving_right);
void extract_row(const vector<int>& grid, int row_start_index, vector<int>& row);

void move_tiles(vector<int>& tiles_list, bool moving_dir);

bool check_for_zeros(const vector<int>& tiles_list, bool moving_dir);

bool move_col(vector<int>& grid, int col_start_index, bool moving_down);
void extract_col(const vector<int>& grid, int col_start_index, vector<int>& col);

void insert_tile(vector<int>& grid);
bool check_grid(const vector<int>& grid);


int main() {

    ifstream input_file;
    vector<int> grid;
    bool done = false;
    string input_name, command;

    //Randomise the seed based on the time.
    srand(time(NULL));

    cout << "enter initial configuration file name:" << endl;
    cin >> input_name;

    input_file.open(input_name.c_str());

    if (!input_file.is_open()) {
        cout << "file not found, using start configuration" << endl;
        //Create start configuration.
        for (int i=0; i < 15; i++) {
            grid.push_back(0);
        }
        grid.push_back(2);
    }
    else {
        int temp_num;
        while (input_file >> temp_num) {
            grid.push_back(temp_num);
        }
    }

    draw_grid(grid);

    if (check_grid(grid) == false) {
        cout << "game over" << endl;
        done = true;
    }

    while (!done) {
        cin >> command;
        if (execute_command(grid, command) == true) {

            insert_tile(grid);

            draw_grid(grid);
            if (check_grid(grid) == false) {
                cout << "game over" << endl;
                done = true;
            }
        }
    }

    return 0;
}


void draw_grid(const vector<int>& grid) {
    cout << endl;
    for (int i=0; i < 4; i++) {
        for (int j=0; j < 4; j++) {
            cout << grid[(i*4)+j];
            for (int k=0; k < 8-integer_length(grid[(i*4)+j]); k++) {
                cout << " ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

int integer_length(int n) {
    //Returns the number of digits in the input. See Reference 1.
    int length = 1;
    if (n > 0) {
        for (length=0; n > 0; length++) {
            n = n/10;
        }
    }
    return length;
}

bool execute_command(vector<int>& grid, string command) {
    //Returns true if the commands change the grid in any way.
    //If the user moved left/right, extract each row,
    //move tiles accordingly and insert back into
    //the grid.
    int executions = 0;
    if (command == "d") {
        for (int i=0; i < 4; i++) {
            executions += move_row(grid, i*4, MOVING_RIGHT);    //Accumulate executions each time a row has changed.
        }
    }
    else if (command == "a") {
        for (int i=0; i < 4; i++) {
            executions += move_row(grid, i*4, MOVING_LEFT);
        }
    }
    else if (command == "s") {
        for (int i=0; i < 4; i++) {
            executions += move_col(grid, i, MOVING_DOWN);
        }
    }
    else if (command == "w") {
        for (int i=0; i < 4; i++) {
            executions += move_col(grid, i, MOVING_UP);
        }
    }
    if (executions > 0) {
        return true;
    }
    else {
        return false;
    }
}


bool move_row(vector<int>& grid, int row_start_index, bool moving_right) {
    //Returns true if the row changed at all.
    //First extract the row, move them around and put back into grid.
    vector<int> row;
    bool moved = false;
    extract_row(grid, row_start_index, row);
    bool full = true, no_tiles = true;
    for (int i=0; i < row.size(); i++) {
        if (row[i] == 0 || (i != row.size()-1 && row[i] == row[i+1])) {
            full = false;
        }
        if (row[i] != 0) {
            no_tiles = false;
        }
    }
    if (full || no_tiles) {
        return false;
    }
    else {
        if (moving_right) {
            move_tiles(row, MOVING_RIGHT);
        }
        else {
            move_tiles(row, MOVING_LEFT);
        }
        //Now insert back into the grid.
        for (int i=0; i < row.size(); i++) {
            if (grid[row_start_index+i] != row[i]) {
                grid[row_start_index+i] = row[i];
                moved = true;
            }
        }
    }
    return moved;
}

void extract_row(const vector<int>& grid, int row_start_index, vector<int>& row) {
    for (int i = 0; i < 4; i++) {
        row.push_back(grid[row_start_index+i]);
    }
}


void move_tiles(vector<int>& tiles_list, bool moving_dir) {

    //If moving_dir is true, we are moving right/down. If false, we are moving left/up.
    if (moving_dir == true) {

        //First search for any 0s starting from the right. If there is one, move all
        //tiles on the left of the zero to the right. Repeat for the next 0.
        //Finally, check for any adjacent tiles if they are the same and join them.

        while (check_for_zeros(tiles_list, moving_dir) == true || tiles_list[tiles_list.size()-1] == 0) {
            int i = tiles_list.size()-1;
            while (i != 0) {
                if (tiles_list[i] == 0) {
                    int j=i-1;
                    do {
                        tiles_list[j+1] = tiles_list[j];
                        j--;
                    } while (j >= 0);
                    tiles_list[j+1] = 0;
                }
                i--;
            }
        }

        //Now check for any adjacent tiles.
        for (int i=tiles_list.size()-1; i > 0; i--) {
            if (tiles_list[i] == tiles_list[i-1] && tiles_list[i] != 0) {
                tiles_list[i] *= 2;
                //Now shift the tiles *before* index i.
                int j=i-2;
                while (j >= 0) {
                    tiles_list[j+1] = tiles_list[j];
                    j--;
                }
                tiles_list[j+1] = 0;

            }
        }
    }
    else	//Moving the tiles towards the left/upwards.
    {
        while (check_for_zeros(tiles_list, moving_dir) == true || tiles_list[0] == 0) {
            int i = 0;
            while (i != tiles_list.size()-1) {
                if (tiles_list[i] == 0) {
                    int j=i;
                    do {
                        tiles_list[j] = tiles_list[j+1];
                        j++;
                    } while (j < tiles_list.size()-1);
                    tiles_list[j] = 0;

                }
                i++;
            }
        }
        //Now check for any adjacent tiles.
        for (int i=0; i < tiles_list.size()-1; i++) {
            if (tiles_list[i] == tiles_list[i+1] && tiles_list[i] != 0) {
                tiles_list[i] *= 2;
                //Now shift the tiles *after* index i.
                int j=i+1;
                while (j < tiles_list.size()-1) {
                    tiles_list[j] = tiles_list[j+1];
                    j++;
                }
                tiles_list[j] = 0;
            }
        }
    }

}




bool check_for_zeros(const vector<int>& tiles_list, bool moving_dir) {
    //Returns true if there are any zeros between the first and last cell in a row/col.
    int zero_num = 0;
    bool found = false;
    bool found_non_zero = false;
    if (moving_dir == true) {   //Check across a row towards the right, or down a column.
        for (int i=0; i < tiles_list.size()-1; i++) {
            //First find the first non-zero tile and then count the zeros until the next non-zero tile if found (if any).
            found_non_zero = false;
            if (tiles_list[i] != 0) {
            //Found the first non-zero tile. Now search for the next one and count all the zeros as you go through.
                int j=i+1;
                while (!found_non_zero && j < tiles_list.size()) {
                    if (tiles_list[j] == 0) {
                        zero_num++;
                    }
                    else {
                        found = true;
                        found_non_zero = true;
                    }
                    j++;
                }
            }
        }
    }
    else {      //Check across a row towards the left, or up a column.
        for (int i=tiles_list.size()-1; i >=1; i--) {
            //First find the first non-zero tile and then count the zeros until the next non-zero tile (if it exists).
            found_non_zero = false;
            if (tiles_list[i] != 0) {
                int j=i-1;
                while (!found_non_zero && j >=0) {
                    if (tiles_list[j] == 0) {
                        zero_num++;
                    }
                    else {
                        found = true;
                        found_non_zero = true;
                    }
                    j--;
                }
            }
        }
    }
    if (found && zero_num > 0) {
        return true;
    }
    else {
        return false;
    }
}


bool move_col(vector<int>& grid, int col_start_index, bool moving_down) {
    //Returns true if the column moved at all.
    //First extract the column, move them around and put back into grid.
    vector<int> col;
    bool moved = false;
    extract_col(grid, col_start_index, col);

    bool full = true, no_tiles = true;
    for (int i=0; i < col.size(); i++) {
        if (col[i] == 0 || (i != col.size()-1 && col[i] == col[i+1])) {
            full = false;
        }
        if (col[i] != 0) {
            no_tiles = false;
        }
    }
    if (full || no_tiles) {
        return false;
    }
    else {
        if (moving_down) {
            move_tiles(col, MOVING_DOWN);
        }
        else {
            move_tiles(col, MOVING_UP);
        }
        //Now insert back into grid.
        for (int i=0; i < col.size(); i++) {
            if (grid[col_start_index+(i*4)] != col[i]) {
                grid[col_start_index+(i*4)] = col[i];
                moved = true;
            }
        }
    }
    return moved;
}

void extract_col(const vector<int>& grid, int col_start_index, vector<int>& col) {
    for (int i = 0; i < 4; i++) {
        col.push_back(grid[col_start_index+(i*4)]);
    }
}


void insert_tile(vector<int>& grid) {
    //First, store all of the indexes in the grid containing zeros in a vector.
    vector<int> zeros;
    for (int i=0; i < grid.size(); i++) {
        if (grid[i] == 0) {
            zeros.push_back(i);
        }
    }
    //Exit if there are no zeros.
    if (zeros.size() == 0) {
        return;
    }

    //Now place a 2 in an empty space with equal probability. See Reference 2.
    if (zeros.size() == 1 ){
        grid[zeros[0]] = 2;
    }
    else {
        //Change the seed again.
        srand(time(NULL));
        grid[zeros[rand() % (zeros.size())]] = 2;
    }

}

bool check_grid(const vector<int>& grid) {
    //Returns true if it is not game over.
    //First check for any zeros.
    for (int i=0; i < grid.size(); i++) {
        if (grid[i] == 0) {
            return true;
        }
    }
    //Next, extract each row and see if any of the adjacent tiles are the same.
    for (int i=0; i < 4; i++) {
        vector<int> temp_row;
        extract_row(grid, i*4, temp_row);
        for (int j=0; j < temp_row.size()-1; j++) {
            if (temp_row[j] == temp_row[j+1]) {
                return true;
            }
        }
    }
    //Now check the columns.
    for (int i=0; i < 4; i++) {
        vector<int> temp_col;
        extract_col(grid, i, temp_col);
        for (int j=0; j < temp_col.size()-1; j++) {
            if (temp_col[j] == temp_col[j+1]) {
                return true;
            }
        }
    }
    return false;
}
