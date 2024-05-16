//
// Created by whitby on 5/5/24.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

void processCSV(const string& inputFileName, const string& outputFileName) {
    ifstream inputFile(inputFileName);
    ofstream outputFile(outputFileName);

    if (!inputFile.is_open()) {
        cout << "Error opening input file." << endl;
        return;
    }

    if (!outputFile.is_open()) {
        cout << "Error opening output file." << endl;
        inputFile.close();
        return;
    }

    // Skip header line
    string line;
    getline(inputFile, line);

    // Process each line
    while (getline(inputFile, line)) {
        stringstream ss(line);
        string cell;
        bool firstCell = true;

        while (getline(ss, cell, ',')) {
            if (!firstCell) {
                outputFile << " "; // Replace comma with space
            } else {
                cell.insert(cell.length() - 9, 1, '.');
                firstCell = false;
            }
            outputFile << cell;
        }
        outputFile << endl;
    }

    // Close files
    inputFile.close();
    outputFile.close();

    cout << "CSV file has been processed and converted to TXT." << endl;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <csv file> <txt file>" << endl;
        return -1;
    }
    string inputFileName = argv[1]; // Input CSV file name
    string outputFileName = argv[2]; // Output TXT file name

    processCSV(inputFileName, outputFileName);

    return 0;
}
