#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

using namespace std;

const string validFileExtension[] = {".m3u", ".m3u8"};


vector<string> readFile(const string &path) {

    if (!filesystem::exists(path))
        throw runtime_error("File \"" + path + "\" does not exist");

    ifstream file;

    try {
        vector<string> lines;
        file.open(path);

        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                lines.push_back(line);
            }
        }

        file.close();
        return lines;
    }
    catch (const exception &err) {
        file.close();
        throw err;
    }
}

void writeTextFile(const string &path, const vector<string> &content) {

    ofstream file;
    file.open(path);

    for (auto &line: content) {
        file << line << "\n";
    }

    file.close();
}

string getRelativePath(const string &from, const string &to) {
    filesystem::path pathFrom(from);
    filesystem::path pathTo(to);
    filesystem::path relative = filesystem::relative(to, from);
    return relative.string();
}

string processLine(const string &folder, const string &line) {

    // Comment line, skip
    if (line.starts_with("#"))
        return line;

    // Looks like its already a relative path, skip
    if (line.starts_with("."))
        return line;

    // Whatever this is, its not a file
    if (!filesystem::exists(line) || !filesystem::is_regular_file(line))
        return line;

    return getRelativePath(folder, line);
}

void makePathesInM3URelative(const string &file) {

    filesystem::path filePath(file);
    string folder = filePath.parent_path().string();

    vector<string> lines = readFile(file);
    vector<string> newLines;

    for (string &line: lines) {
        string newLine = processLine(folder, line);
        newLines.push_back(newLine);
    }

    writeTextFile(file, newLines);
}

bool isValidFileExtension(const string &file) {

    for (auto &ext: validFileExtension) {
        if (file.ends_with(ext))
            return true;
    }

    return false;
}

int main(int argc, char *argv[]) {
    try {

        if (argc < 2) {
            cerr << "Please provide the a .m3u or .m3u8 file" << endl;
            return 1;
        }

        const string &file = argv[1];
        if (!isValidFileExtension(file)) {
            cerr << "Please provide the a .m3u or .m3u8 file" << endl;
            return 1;
        }

        if (!filesystem::exists(file)) {
            cerr << "The provided file \"" << file << "\" does not exist." << endl;
            return 1;
        }

        makePathesInM3URelative(file);
    }
    catch (const exception &err) {
        cerr << err.what() << endl;
        return 1;
    }

    return 0;
}
