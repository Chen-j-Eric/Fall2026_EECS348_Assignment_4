/*
  name: EECS 348 Assignment 4
  purpose: Sudoku Solver
  outputs: Using a recersive solution, solve a sodoku problem and the sodoku solution must be valid
  collaborators: KIMI K2.5 | Thinking
  author: Eric Chen
  creation date: 10/15/2026

  
    prologue comments about operations:
    anything like if the code doesn't run yk so any other notes about that will be put here

*/


#include <array> // fixed-size array type used for the 9x9 Sudoku grid
#include <cctype> // checks whether a character is a decimal digit
#include <fstream> // std::ifstream, which reads puzzle data from a file
#include <iostream> //std::cout and std::cerr for normal output and error messages.
#include <string> // Provides std::string for filenames and input tokens.
#include <vector> //std::vector for lists of candidate digits and filenames.

class Sudoku { // The calss that defines and contains the solving logic 
public:
    bool read(const std::string& filename) {
        name = filename; // Sames Name
        std::ifstream in(filename); // Opens File
        if (!in) { // if not in file
            std::cerr << "Could not open " << filename << "\n"; // sends error message and breaks
            // returns false so main knows it's invalid
            return false;
        }

        grid.fill({}); // Resets every cell in grid
        std::string tok; // Stores each white space
        int count = 0; // how many cell values have been read so far

        while (in >> tok) { //Reas one whitespaced seperated value
            if (count >= 81) return false; 
            // A sudoku must have 81 cells
            int value = 0; // start with zero
            if (tok == "_") {// repersents empty cell
                value = 0; // Keeps value equal to zero for an empty cell
            } else if (tok.size() == 1 && std::isdigit(static_cast<unsigned char>(tok[0]))) { // only takes it if it's valid
                value = tok[0] - '0'; // Sudoku must be from 1-9
                if (value < 1 || value > 9) return false; // tells value that digit is valid
            } else {
                return false; // rejects the puzzle because token is invalid
            }

            grid[count / 9][count % 9] = value; // Convert the linear cell count 
            ++count;
        }

        if (count != 81) return false;
        return initialGridIsConsistent();
    }

    void run() {
        std::cout << "File: " << name << "\n";  // Reset the number of solutions for this run.
        printGrid(); // prints grid?
/
        if (!consistent) { // not consistent 
            std::cout << "No solution found\n\n"; // does not exist
            return;
        }

        solve(); //recursive backtracking

        if (solutions == 0) { // if did not find, it doesn't exist
            std::cout << "No solution found\n"; 
        } else {
            std::cout << "Solutions found: " << solutions << "\n";//otherwise at least one solution exist
        }
        std::cout << "\n";
    }

private: // private methods that solve this sudoku
    std::string name; 
    std::array<std::array<int, 9>, 9> grid{}; // 9x9 sudoku grid
    bool consistent = true; //if it's true
    int solutions = 0; // how many complete sotutions

    bool initialGridIsConsistent() { //returns false if it's not true
        for (int r = 0; r < 9; ++r) { //Visit each base of nine rows
            for (int c = 0; c < 9; ++c) { // vist each of the nine cols
                int v = grid[r][c]; // reads curr cell into shorter value
                if (v == 0) continue;

                for (int c2 = c + 1; c2 < 9; ++c2) { // checks cells to right in the same row for dups
                    if (grid[r][c2] == v) return false;  //checks inconsistancies
                }
                for (int r2 = r + 1; r2 < 9; ++r2) { // checks cell below the current cell in the same column
                    if (grid[r2][c] == v) return false; //checks for inconsistancies
                }

                int br = r / 3 * 3; //finds frst row of index in 3x3 block
                int bc = c / 3 * 3; //firt col
                for (int r2 = br; r2 < br + 3; ++r2) { //visits each col
                    for (int c2 = bc; c2 < bc + 3; ++c2) { // checks if it appears twice
                        if ((r2 != r || c2 != c) && grid[r2][c2] == v) return false;
                    }
                }
            }
        }
        return true;
    }

    // I don't to explain it line by line but it goes through all of the indexes and checks
    // if any of them are empty
    bool findEmpty(int& er, int& ec) const {
        for (int r = 0; r < 9; ++r) {
            for (int c = 0; c < 9; ++c) {
                if (grid[r][c] == 0) {
                    er = r;
                    ec = c;
                    return true;
                }
            }
        }
        return false;
    }

    // Builds and returns all digitis 
    std::vector<int> candidates(int r, int c) const {
        std::vector<int> out; //creates new vector
        if (grid[r][c] != 0) return out;

        bool used[10] = {}; // makrs all valyes already present in the cell's rows

        for (int c2 = 0; c2 < 9; ++c2) used[grid[r][c2]] = true; // mark all values present in this cell's rows
        for (int r2 = 0; r2 < 9; ++r2) used[grid[r2][c]] = true; // mark all values present in col

        int br = r / 3 * 3; // calc the top row of the 3x3 block
        int bc = c / 3 * 3; // calc the leftmost col of the 3x3 contain this cell
        for (int r2 = br; r2 < br + 3; ++r2) { //vist the curr block
            for (int c2 = bc; c2 < bc + 3; ++c2) {
                used[grid[r2][c2]] = true;// marks each valhye in the block as unuseable for this cell
            }
        }

        for (int v = 1; v <= 9; ++v) {
            if (!used[v]) out.push_back(v); // adds digit if there is no row, colum or block
        } // returns the list of possible digits to the solver
        return out;
    }

    void solve() { // rec serach for soltuon by trying and backtraciing
        int r, c;
        if (!findEmpty(r, c)) {
            ++solutions; // adds one everyt time
            std::cout << "Solution " << solutions << ":\n"; // lable this competed grid
            printGrid(); // prints grid
            std::cout << "\n"; // seperates from any pre solution
            return;
        }

        std::vector<int> vals = candidates(r, c);// calc the digits allowed
        for (int v : vals) {
            grid[r][c] = v; // place this candiate in the empt cell
            solve(); 
            grid[r][c] = 0; // undos gess so they can backtrack
        }
    }

    void printGrid() const { //prints grid
        for (int r = 0; r < 9; ++r) { // one row
            for (int c = 0; c < 9; ++c) { // one col
                if (grid[r][c] == 0) std::cout << "_ "; // underscore = 0
                else std::cout << grid[r][c] << ' ';
            }
            std::cout << "\n"; // ends current print row
        }
    }
};


// program entry point
int main(int argc, char** argv) {
    // stores the names if very puzzle
    std::vector<std::string> files;

    // Filenames can be passed as command-line arguments.
    for (int i = 1; i < argc; ++i) {
        files.emplace_back(argv[i]);
    }

    // If no filenames were supplied, ask the user instead of assuming puzzle1.txt, etc.
    if (files.empty()) {
        //explain that files names caanbe enter one line at a time
        std::cout << "Enter puzzle filenames one at a time. Press Enter on a blank line when finished.\n";
        while (true) {
            std::cout << "Puzzle filename: "; // na,e
            std::string filename;
            if (!std::getline(std::cin, filename)) break; //blank line
            if (filename.empty()) break;
            files.push_back(filename); //input collection
        }
    }

    if (files.empty()) { //explain why can't run
        std::cerr << "Error: No puzzle filenames were provided.\n";
        return 1;
    }

    int failedFiles = 0;
    for (const std::string& filename : files) {
        Sudoku sudoku;
        if (sudoku.read(filename)) { //checks and remebers why it failed
            sudoku.run();
        } else {
            ++failedFiles;
            // Continue with the remaining files so one bad file does not stop the run.
        }
    }

    if (failedFiles > 0) {
        std::cerr << "Finished with " << failedFiles //explains why it failed
                  << " file(s) that had errors. Check File for Strings or misformatting.\n";
        return 1;
    }

    return 0;
}