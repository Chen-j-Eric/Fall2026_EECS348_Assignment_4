/*
  name: EECS 348 Assignment 4
  purpose: Sudoku Solver
  outputs: Using a recersive solution, solve a sodoku problem and the sodoku solution must be valid
  collaborators: Claude Sonnet 5.5 | Thinking
  author: Eric Chen
  creation date: 10/15/2026

  
    prologue comments about operations:
    anything like if the code doesn't run yk so any other notes about that will be put here

*/

// Sudoku brute-force solver (depth-first search with backtracking).
// Object-oriented design: Grid, PuzzleReader, Solver, Reporter.
// Finds ALL solutions of each puzzle file given on the command line.
//
// Build:  g++ -std=c++17 -O2 -o sudoku_claude sudoku_claude.cpp
// Run:    ./sudoku_claude puzzle1.txt puzzle2.txt ... puzzle5.txt

#include <array>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Grid: a 9x9 board that knows its own rules (row / column / block checks).
// ---------------------------------------------------------------------------
class Grid {
public:
    static constexpr int SIZE  = 9;
    static constexpr int BLOCK = 3;
    static constexpr int EMPTY = 0;

    Grid() { for (auto& r : cells_) r.fill(EMPTY); }

    int  get(int r, int c) const        { return cells_[r][c]; }
    void set(int r, int c, int value)   { cells_[r][c] = value; }
    void clear(int r, int c)            { cells_[r][c] = EMPTY; }
    bool isEmpty(int r, int c) const    { return cells_[r][c] == EMPTY; }

    // True if `value` does not already appear in the row, column or block of (r,c).
    bool canPlace(int r, int c, int value) const {
        for (int i = 0; i < SIZE; ++i) {
            if (cells_[r][i] == value) return false;   // row
            if (cells_[i][c] == value) return false;   // column
        }
        const int br = (r / BLOCK) * BLOCK, bc = (c / BLOCK) * BLOCK;
        for (int i = 0; i < BLOCK; ++i)
            for (int j = 0; j < BLOCK; ++j)
                if (cells_[br + i][bc + j] == value) return false; // block
        return true;
    }

    // Candidate digits for an empty cell.
    std::vector<int> candidates(int r, int c) const {
        std::vector<int> out;
        for (int v = 1; v <= SIZE; ++v)
            if (canPlace(r, c, v)) out.push_back(v);
        return out;
    }

    // Are the initial givens consistent with each other?
    bool givensAreConsistent() const {
        Grid copy = *this;
        for (int r = 0; r < SIZE; ++r)
            for (int c = 0; c < SIZE; ++c)
                if (!isEmpty(r, c)) {
                    copy.clear(r, c);
                    if (!copy.canPlace(r, c, get(r, c))) return false;
                    copy.set(r, c, get(r, c));
                }
        return true;
    }

    int filledCount() const {
        int n = 0;
        for (const auto& row : cells_) for (int v : row) if (v != EMPTY) ++n;
        return n;
    }

    friend std::ostream& operator<<(std::ostream& os, const Grid& g) {
        for (int r = 0; r < SIZE; ++r) {
            for (int c = 0; c < SIZE; ++c) {
                if (g.cells_[r][c] == EMPTY) os << "_ ";
                else                         os << g.cells_[r][c] << ' ';
            }
            os << '\n';
        }
        return os;
    }

private:
    std::array<std::array<int, SIZE>, SIZE> cells_;
};

// ---------------------------------------------------------------------------
// PuzzleReader: loads a Grid from a text file ('_' = blank).
// ---------------------------------------------------------------------------
class PuzzleReader {
public:
    static Grid read(const std::string& path) {
        std::ifstream in(path);
        if (!in) throw std::runtime_error("Cannot open file: " + path);

        Grid g;
        int count = 0;
        char ch;
        while (in >> ch && count < Grid::SIZE * Grid::SIZE) {
            int r = count / Grid::SIZE, c = count % Grid::SIZE;
            if (ch == '_')                g.set(r, c, Grid::EMPTY);
            else if (ch >= '1' && ch <= '9') g.set(r, c, ch - '0');
            else throw std::runtime_error(std::string("Bad character '") + ch + "' in " + path);
            ++count;
        }
        if (count != Grid::SIZE * Grid::SIZE)
            throw std::runtime_error("Expected 81 cells in " + path);
        return g;
    }
};

// ---------------------------------------------------------------------------
// Solver: recursive depth-first search with backtracking.
// Does not stop at the first solution; collects every solution.
// ---------------------------------------------------------------------------
class Solver {
public:
    explicit Solver(Grid puzzle) : grid_(puzzle) {}

    // Returns the number of solutions found; they are stored in solutions().
    std::size_t solveAll() {
        solutions_.clear();
        if (grid_.givensAreConsistent()) search();
        return solutions_.size();
    }

    const std::vector<Grid>& solutions() const { return solutions_; }

private:
    Grid grid_;
    std::vector<Grid> solutions_;

    // Pick the empty cell with the fewest candidates (keeps the search fast).
    // Returns false if there are no empty cells (grid is full).
    bool pickCell(int& row, int& col, std::vector<int>& cands) const {
        bool found = false;
        for (int r = 0; r < Grid::SIZE; ++r)
            for (int c = 0; c < Grid::SIZE; ++c) {
                if (!grid_.isEmpty(r, c)) continue;
                std::vector<int> cs = grid_.candidates(r, c);
                if (!found || cs.size() < cands.size()) {
                    row = r; col = c; cands = cs; found = true;
                    if (cands.empty()) return true;   // dead end, bail out early
                }
            }
        return found;
    }

    void search() {
        int r = 0, c = 0;
        std::vector<int> cands;
        if (!pickCell(r, c, cands)) {          // k == 81: a complete solution
            solutions_.push_back(grid_);
            return;
        }
        for (int v : cands) {                  // empty list => no-solution-found
            grid_.set(r, c, v);                // try candidate
            search();                          // recurse with k+1 cells filled
            grid_.clear(r, c);                 // backtrack
        }
    }
};

// ---------------------------------------------------------------------------
// Reporter: prints file name, puzzle and all solutions.
// ---------------------------------------------------------------------------
class Reporter {
public:
    static void report(const std::string& name, const Grid& puzzle,
                       const std::vector<Grid>& solutions) {
        std::cout << "==============================\n";
        std::cout << "Puzzle file: " << name << "\n\n";
        std::cout << "Puzzle:\n" << puzzle << '\n';
        if (solutions.empty()) {
            std::cout << "No solution found\n\n";
            return;
        }
        for (std::size_t i = 0; i < solutions.size(); ++i) {
            std::cout << "Solution " << (i + 1) << ":\n" << solutions[i] << '\n';
        }
        std::cout << "Solutions found: " << solutions.size() << "\n\n";
    }
};

// ---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    std::vector<std::string> files;
    for (int i = 1; i < argc; ++i) files.push_back(argv[i]);
    if (files.empty())
        for (int i = 1; i <= 5; ++i) files.push_back("puzzle" + std::to_string(i) + ".txt");

    for (const auto& f : files) {
        try {
            Grid puzzle = PuzzleReader::read(f);
            Solver solver(puzzle);
            solver.solveAll();
            Reporter::report(f, puzzle, solver.solutions());
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << '\n';
        }
    }
    return 0;
}