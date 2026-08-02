#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

const char* DATA_FILE = "data.db";

struct Record {
    char index[65];  // 64 bytes + null terminator
    int value;
    bool deleted;

    Record() : value(0), deleted(false) {
        memset(index, 0, sizeof(index));
    }

    Record(const string& idx, int val) : value(val), deleted(false) {
        memset(index, 0, sizeof(index));
        strncpy(index, idx.c_str(), 64);
    }
};

void insert(const string& index, int value) {
    // Check if this exact entry already exists
    ifstream infile(DATA_FILE, ios::binary);
    if (infile.is_open()) {
        Record rec;
        while (infile.read((char*)&rec, sizeof(Record))) {
            if (!rec.deleted && strcmp(rec.index, index.c_str()) == 0 && rec.value == value) {
                infile.close();
                return;  // Entry already exists, don't insert duplicate
            }
        }
        infile.close();
    }

    // Append new record
    ofstream outfile(DATA_FILE, ios::binary | ios::app);
    Record rec(index, value);
    outfile.write((char*)&rec, sizeof(Record));
    outfile.close();
}

void deleteEntry(const string& index, int value) {
    // Open file for reading and writing
    fstream file(DATA_FILE, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        return;  // File doesn't exist, nothing to delete
    }

    Record rec;
    while (file.read((char*)&rec, sizeof(Record))) {
        if (!rec.deleted && strcmp(rec.index, index.c_str()) == 0 && rec.value == value) {
            // Mark as deleted
            rec.deleted = true;
            file.seekp(-static_cast<long>(sizeof(Record)), ios::cur);
            file.write((char*)&rec, sizeof(Record));
            file.close();
            return;  // Only delete one matching entry
        }
    }

    file.close();
}

void find(const string& index) {
    vector<int> values;

    ifstream infile(DATA_FILE, ios::binary);
    if (infile.is_open()) {
        Record rec;
        while (infile.read((char*)&rec, sizeof(Record))) {
            if (!rec.deleted && strcmp(rec.index, index.c_str()) == 0) {
                values.push_back(rec.value);
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
