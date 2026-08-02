#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

const char* DATA_FILE = "data.db";

struct Record {
    string index;
    int value;
    bool deleted;
};

void insert(const string& index, int value) {
    // Check if this exact entry already exists
    ifstream infile(DATA_FILE);
    if (infile.is_open()) {
        string line;
        while (getline(infile, line)) {
            if (line.empty()) continue;

            istringstream iss(line);
            string idx;
            int val;
            int del;

            if (iss >> del >> idx >> val) {
                if (del == 0 && idx == index && val == value) {
                    infile.close();
                    return;  // Entry already exists
                }
            }
        }
        infile.close();
    }

    // Append new record
    ofstream outfile(DATA_FILE, ios::app);
    if (outfile.is_open()) {
        outfile << "0 " << index << " " << value << "\n";
        outfile.close();
    }
}

void deleteEntry(const string& index, int value) {
    ifstream infile(DATA_FILE);
    if (!infile.is_open()) {
        return;  // File doesn't exist
    }

    const char* TEMP_FILE = "data.tmp";
    ofstream outfile(TEMP_FILE);
    if (!outfile.is_open()) {
        infile.close();
        return;
    }

    string line;
    bool found = false;

    while (getline(infile, line)) {
        if (line.empty()) {
            outfile << line << "\n";
            continue;
        }

        istringstream iss(line);
        string idx;
        int val;
        int del;

        if (iss >> del >> idx >> val) {
            if (!found && del == 0 && idx == index && val == value) {
                // Mark as deleted
                outfile << "1 " << idx << " " << val << "\n";
                found = true;
            } else {
                outfile << line << "\n";
            }
        } else {
            outfile << line << "\n";
        }
    }
    infile.close();
    outfile.close();

    if (found) {
        remove(DATA_FILE);
        rename(TEMP_FILE, DATA_FILE);
    } else {
        remove(TEMP_FILE);
    }
}

void find(const string& index) {
    vector<int> values;

    ifstream infile(DATA_FILE);
    if (infile.is_open()) {
        string line;
        while (getline(infile, line)) {
            if (line.empty()) continue;

            istringstream iss(line);
            string idx;
            int val;
            int del;

            if (iss >> del >> idx >> val) {
                if (del == 0 && idx == index) {
                    values.push_back(val);
                }
            }
        }
        infile.close();
    }

    if (values.empty()) {
        cout << "null" << endl;
    } else {
        sort(values.begin(), values.end());
        for (size_t i = 0; i < values.size(); i++) {
            if (i > 0) cout << " ";
            cout << values[i];
        }
        cout << endl;
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n;
    cin >> n;

    string command;
    for (int i = 0; i < n; i++) {
        cin >> command;

        if (command == "insert") {
            string index;
            int value;
            cin >> index >> value;
            insert(index, value);
        } else if (command == "delete") {
            string index;
            int value;
            cin >> index >> value;
            deleteEntry(index, value);
        } else if (command == "find") {
            string index;
            cin >> index;
            find(index);
        }
    }

    return 0;
}
