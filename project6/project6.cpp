#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <string>
#include <boost/regex.hpp>
using boost::regex;
using boost::smatch;
using boost::regex_replace;
using boost::regex_search;
using namespace std;

string normalizeSpacesAndLines(const string& code) {
    stringstream ss(code);
    string line, result;
    while (getline(ss, line)) {
        line = regex_replace(line, regex("^\\s+|\\s+$"), "");
        line = regex_replace(line, regex("[ \t]+"), " ");
        if (!line.empty()) {
            result += line + "\n";
        }
    }
    return result;
}

string removeComments(const string& code) {
    string noComments = regex_replace(code, regex("/\\*([^*]|\\*(?!/))*\\*/"), "");
    noComments = regex_replace(noComments, regex("//[^\n]*"), "");
    return noComments;
}

string normalizeVariables(string code) {
    unordered_map<string, string> variableMap = {
        {"a", "var1"}, {"b", "var2"}, {"sum", "var3"},
        {"x", "var4"}, {"y", "var5"}, {"result", "var6"},
        {"c", "var7"}, {"total", "var9"}, {"i", "var9"}
    };

    for (const auto& [original, replacement] : variableMap) {
        code = regex_replace(code, regex("\\b" + original + "\\b"), replacement);
    }

    return code;
}

vector<string> tokenize(const string& code) {
    vector<string> tokens;
    regex pattern("(\".*?\")|([a-zA-Z_][a-zA-Z0-9_]*)|(\\d+(\\.\\d+)?)|(\\+\\+|--|==|!=|<=|>=)|([=+\\-*/%<>&|^!;:.,()\\[\\]{}])");
    smatch match;
    string input = code;
    while (regex_search(input, match, pattern)) {
        tokens.push_back(match.str(0));
        input = match.suffix().str();
    }
    return tokens;
}

vector<string> createKGrams(const vector<string>& tokens, int k) {
    vector<string> kgrams;
    if (tokens.size() < k) return kgrams;
    for (size_t i = 0; i <= tokens.size() - k; ++i) {
        string gram;
        for (int j = 0; j < k; ++j) {
            gram += tokens[i + j];
            if (j < k - 1) gram += " ";
        }
        kgrams.push_back(gram);
    }
    return kgrams;
}

unsigned long simpleHash(const string& s) {
    unsigned long hash = 5381;
    for (char c : s) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

unordered_set<unsigned long> hashKGrams(const vector<string>& kgrams) {
    unordered_set<unsigned long> hashes;
    for (const auto& gram : kgrams) {
        hashes.insert(simpleHash(gram));
    }
    return hashes;
}

double jaccardSimilarity(const unordered_set<unsigned long>& setA, const unordered_set<unsigned long>& setB) {
    if (setA.empty() && setB.empty()) return 1.0;
    int common = 0;
    for (auto x : setA) if (setB.count(x)) ++common;
    int total = setA.size() + setB.size() - common;
    return (double)common / total;
}

void printSimilarityMatrix(const vector<unordered_set<unsigned long>>& allHashes, const vector<string>& fileNames) {
    cout.setf(ios::fixed);
    cout.precision(2);

    // Header row
    for (const auto& name : fileNames) {
        cout << name << " ";
    }
    cout << "\n";

    for (size_t i = 0; i < fileNames.size(); ++i) {
        cout << fileNames[i] << " ";
        for (size_t j = 0; j < fileNames.size(); ++j) {
            cout << jaccardSimilarity(allHashes[i], allHashes[j]);
            if (j != fileNames.size() - 1) cout << " ";
        }
        cout << "\n";
    }
}

int main() {
    vector<string> fileNames = {"test1.cpp", "test2.cpp", "test3.cpp", "test4.cpp", "test5.cpp", "test6.cpp"};
    int k = 3;
    vector<unordered_set<unsigned long>> allHashes;
    for (auto& fn : fileNames) {
        ifstream in(fn);
        if (!in) { cerr << "Cannot open " << fn << "\n"; continue; }
        string code((istreambuf_iterator<char>(in)), {});
        string clean = normalizeSpacesAndLines(code);
        clean = removeComments(clean);
        clean = normalizeVariables(clean);
        auto tok = tokenize(clean);
        cout << "Tokens for " << fn << ":\n| ";
        for (auto& t : tok) cout << t << " ";
        cout << "\n";
        allHashes.push_back(hashKGrams(createKGrams(tok, k)));
    }
    printSimilarityMatrix(allHashes, fileNames);
    return 0;
}



