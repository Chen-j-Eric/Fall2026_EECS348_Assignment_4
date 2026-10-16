/*
  name: EECS 348 Assignment 4
  purpose: Sudoku Solver
  outputs: Using a recersive solution, solve a sodoku problem and the sodoku solution must be valid
  collaborators: KIMI K2.5 High | Thinking
  author: Eric Chen
  creation date: 10/15/2026

  
    prologue comments about operations:
    anything like if the code doesn't run yk so any other notes about that will be put here

*/


#include <array>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Sudoku {
public:
    bool read(const std::string& filename) {
        name = filename;
        std::ifstream in(filename);
        if (!in) {
            std::cerr << "Could not open " << filename << "\n";
            return false;
        }

        grid.fill({});
        std::string tok;
        int count = 0;

        while (in >> tok) {
            if (count >= 81) return false;

            int value = 0;
            if (tok == "_") {
                value = 0;
            } else if (tok.size() == 1 && std::isdigit(static_cast<unsigned char>(tok[0]))) {
                value = tok[0] - '0';
                if (value < 1 || value > 9) return false;
            } else {
                return false;
            }

            grid[count / 9][count % 9] = value;
            ++count;
        }

        if (count != 81) return false;
        return initialGridIsConsistent();
    }

    void run() {
        std::cout << "File: " << name << "\n";
        printGrid();

        if (!consistent) {
            std::cout << "No solution found\n\n";
            return;
        }

        solve();

        if (solutions == 0) {
            std::cout << "No solution found\n";
        } else {
            std::cout << "Solutions found: " << solutions << "\n";
        }
        std::cout << "\n";
    }

private:
    std::string name;
    std::array<std::array<int, 9>, 9> grid{};
    bool consistent = true;
    int solutions = 0;

    bool initialGridIsConsistent() {
        for (int r = 0; r < 9; ++r) {
            for (int c = 0; c < 9; ++c) {
                int v = grid[r][c];
                if (v == 0) continue;

                for (int c2 = c + 1; c2 < 9; ++c2) {
                    if (grid[r][c2] == v) return false;
                }
                for (int r2 = r + 1; r2 < 9; ++r2) {
                    if (grid[r2][c] == v) return false;
                }

                int br = r / 3 * 3;
                int bc = c / 3 * 3;
                for (int r2 = br; r2 < br + 3; ++r2) {
                    for (int c2 = bc; c2 < bc + 3; ++c2) {
                        if ((r2 != r || c2 != c) && grid[r2][c2] == v) return false;
                    }
                }
            }
        }
        return true;
    }

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

    std::vector<int> candidates(int r, int c) const {
        std::vector<int> out;
        if (grid[r][c] != 0) return out;

        bool used[10] = {};

        for (int c2 = 0; c2 < 9; ++c2) used[grid[r][c2]] = true;
        for (int r2 = 0; r2 < 9; ++r2) used[grid[r2][c]] = true;

        int br = r / 3 * 3;
        int bc = c / 3 * 3;
        for (int r2 = br; r2 < br + 3; ++r2) {
            for (int c2 = bc; c2 < bc + 3; ++c2) {
                used[grid[r2][c2]] = true;
            }
        }

        for (int v = 1; v <= 9; ++v) {
            if (!used[v]) out.push_back(v);
        }
        return out;
    }

    void solve() {
        int r, c;
        if (!findEmpty(r, c)) {
            ++solutions;
            std::cout << "Solution " << solutions << ":\n";
            printGrid();
            std::cout << "\n";
            return;
        }

        std::vector<int> vals = candidates(r, c);
        for (int v : vals) {
            grid[r][c] = v;
            solve();
            grid[r][c] = 0;
        }
    }

    void printGrid() const {
        for (int r = 0; r < 9; ++r) {
            for (int c = 0; c < 9; ++c) {
                if (grid[r][c] == 0) std::cout << "_ ";
                else std::cout << grid[r][c] << ' ';
            }
            std::cout << "\n";
        }
    }
};

int main(int argc, char** argv) {
    std::vector<std::string> files;

    if (argc > 1) {
        for (int i = 1; i < argc; ++i) files.emplace_back(argv[i]);
    } else {
        files = {"puzzle1.txt", "puzzle2.txt", "puzzle3.txt",
                 "puzzle4.txt", "puzzle5.txt"};
    }

    for (const auto& f : files) {
        Sudoku s;
        if (s.read(f)) {
            s.run();
        }
    }
}