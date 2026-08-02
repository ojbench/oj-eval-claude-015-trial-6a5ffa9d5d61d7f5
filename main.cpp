#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

const char* DATA_FILE = "storage.dat";

struct Record {
    char index[65];
    int value;
    int active;  // Changed from char to int for better alignment
};

bool recordExists(const string& idx, int val) {
    ifstream file(DATA_FILE, ios::binary);
    if (!file.is_open()) return false;

    Record rec;
    while (file.read((char*)&rec, sizeof(Record))) {
        if (rec.active && strcmp(rec.index, idx.c_str()) == 0 && rec.value == val) {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}

void insert(const string& index, int value) {
    if (recordExists(index, value)) return;

    ofstream file(DATA_FILE, ios::binary | ios::app);
    if (!file.is_open()) return;

    Record rec;
    memset(&rec, 0, sizeof(Record));
    strncpy(rec.index, index.c_str(), 64);
    rec.value = value;
    rec.active = 1;

    file.write((char*)&rec, sizeof(Record));
    file.close();
}

void deleteEntry(const string& index, int value) {
    fstream file(DATA_FILE, ios::binary | ios::in | ios::out);
    if (!file.is_open()) return;

    Record rec;
    streampos pos;

    while (true) {
        pos = file.tellg();
        if (!file.read((char*)&rec, sizeof(Record))) break;

        if (rec.active && strcmp(rec.index, index.c_str()) == 0 && rec.value == value) {
            rec.active = 0;
            file.seekp(pos);
            file.write((char*)&rec, sizeof(Record));
            file.close();
            return;
        }
    }
    file.close();
}

void find(const string& index) {
    vector<int> values;

    ifstream file(DATA_FILE, ios::binary);
    if (file.is_open()) {
        Record rec;
        while (file.read((char*)&rec, sizeof(Record))) {
            if (rec.active && strcmp(rec.index, index.c_str()) == 0) {
                values.push_back(rec.value);
            }
        }
        file.close();
    }

    if (values.empty()) {
        cout << "null\n";
    } else {
        sort(values.begin(), values.end());
        for (size_t i = 0; i < values.size(); i++) {
            if (i > 0) cout << " ";
            cout << values[i];
        }
        cout << "\n";
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n;
    cin >> n;

    for (int i = 0; i < n; i++) {
        string cmd;
        cin >> cmd;

        if (cmd == "insert") {
            string idx;
            int val;
            cin >> idx >> val;
            insert(idx, val);
        } else if (cmd == "delete") {
            string idx;
            int val;
            cin >> idx >> val;
            deleteEntry(idx, val);
        } else if (cmd == "find") {
            string idx;
            cin >> idx;
            find(idx);
        }
    }

    return 0;
}
