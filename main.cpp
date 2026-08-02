#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <functional>

using namespace std;

const int NUM_BUCKETS = 16;
const char* DATA_PREFIX = "db_";

string getFileName(const string& index) {
    hash<string> hasher;
    size_t h = hasher(index);
    int bucket = h % NUM_BUCKETS;
    return string(DATA_PREFIX) + to_string(bucket) + ".dat";
}

void insert(const string& index, int value) {
    string filename = getFileName(index);

    // Check if this exact entry already exists
    ifstream infile(filename);
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
    ofstream outfile(filename, ios::app);
    if (outfile.is_open()) {
        outfile << "0 " << index << " " << value << "\n";
        outfile.close();
    }
}

void deleteEntry(const string& index, int value) {
    string filename = getFileName(index);

    ifstream infile(filename);
    if (!infile.is_open()) {
        return;  // File doesn't exist
    }

    string tempfile = filename + ".tmp";
    ofstream outfile(tempfile);
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
        remove(filename.c_str());
        rename(tempfile.c_str(), filename.c_str());
    } else {
        remove(tempfile.c_str());
    }
}

void find(const string& index) {
    string filename = getFileName(index);
    vector<int> values;

    ifstream infile(filename);
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
