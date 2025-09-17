#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <string>
#include <regex>
using namespace std;

// --- Hash Function Provided ---
unsigned long simpleHash(const string& s) {
    const int base = 257;
    const int mod = 1000000007;
    unsigned long hash = 0;
    for (char c : s) {
        hash = (hash * base + c) % mod;
    }
    return hash;
}

// --- Helper Functions ---
string readFile(const string& filename) {
    ifstream file(filename);
    string content, line;
    while (getline(file, line)) {
        content += line + "\n";
    }
    return content;
}

string normalizeText(const string& rawText) {
    string text = rawText;
    // Manually lowercase (no <algorithm>)
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] >= 'A' && text[i] <= 'Z') {
            text[i] = text[i] - 'A' + 'a';
        }
    }
    // Collapse multiple spaces to a single space
    text = regex_replace(text, regex(R"(\s+)"), " ");
    return text;
}

unordered_set<string> loadStopwords(const string& filename) {
    unordered_set<string> stopwords;
    ifstream file(filename);
    string word;
    while (file >> word) {
        stopwords.insert(word);
    }
    return stopwords;
}

vector<string> tokenizeText(const string& text, const unordered_set<string>* stopwords = nullptr) {
    vector<string> tokens;
    regex wordPattern(R"([a-zA-Z0-9]+)");
    auto wordsBegin = sregex_iterator(text.begin(), text.end(), wordPattern);
    auto wordsEnd = sregex_iterator();
    for (auto it = wordsBegin; it != wordsEnd; ++it) {
        string token = it->str();
        if (!stopwords || stopwords->find(token) == stopwords->end()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

vector<string> createKGrams(const vector<string>& tokens, int k) {
    vector<string> kgrams;
    if (tokens.size() < k) return kgrams;
    for (size_t i = 0; i <= tokens.size() - k; ++i) {
        string gram = tokens[i];
        for (int j = 1; j < k; ++j) {
            gram += " " + tokens[i + j];
        }
        kgrams.push_back(gram);
    }
    return kgrams;
}

unordered_set<unsigned long> hashKGrams(const vector<string>& kgrams) {
    unordered_set<unsigned long> hashes;
    for (const string& g : kgrams) {
        cout << "K-gram: " << g << endl;
        hashes.insert(simpleHash(g));
    }
    return hashes;
}

string formatDouble(double d) {
    long long scaled = (long long)(d * 1000000 + 0.5); // round to 6 decimal digits
    if (scaled == 1000000) return "1";
    if (scaled == 0) return "0";

    string result = "";
    long long whole = scaled / 1000000;
    long long frac = scaled % 1000000;

    result += to_string(whole);
    result += ".";
    string fracStr = to_string(frac);
    while (fracStr.length() < 6) fracStr = "0" + fracStr;

    // Trim trailing zeros
    while (!fracStr.empty() && fracStr.back() == '0') {
        fracStr.pop_back();
    }

    result += fracStr;
    return result;
}

void printSimilarityMatrix(const vector<unordered_set<unsigned long>>& allHashes, const vector<string>& fileNames) {
    int n = allHashes.size();

    cout << "Similarity Matrix:" << endl;
    cout << "   ";
    for (const string& name : fileNames) {
        cout << name << " ";
    }
    cout << endl;

    for (int i = 0; i < n; ++i) {
        cout << fileNames[i] << " ";
        for (int j = 0; j < n; ++j) {
            double sim = 0.0;
            const auto& A = allHashes[i];
            const auto& B = allHashes[j];
            size_t intersectionSize = 0;
            for (auto& h : A) {
                if (B.find(h) != B.end()) ++intersectionSize;
            }
            size_t unionSize = A.size() + B.size() - intersectionSize;
            if (unionSize > 0) {
                sim = (double)intersectionSize / unionSize;
            }
            cout << formatDouble(sim);
            if (j != n - 1) cout << " ";
        }
        cout << endl;
    }
}

// --- Function to process files ---
vector<unordered_set<unsigned long>> processFiles(
    const vector<string>& fileNames, int k, const unordered_set<string>* stopwords = nullptr
) {
    vector<unordered_set<unsigned long>> allHashedKGrams;
    for (const string& filename : fileNames) {
        string rawText = readFile(filename);
        string cleanText = normalizeText(rawText);
        vector<string> tokens = tokenizeText(cleanText, stopwords);
        vector<string> kgrams = createKGrams(tokens, k);
        unordered_set<unsigned long> hashed = hashKGrams(kgrams);
        allHashedKGrams.push_back(hashed);
    }
    return allHashedKGrams;
}

// --- Main Program ---
int main() {
    vector<string> fileNames = {"t1.txt", "t2.txt", "t3.txt", "t4.txt", "t5.txt", "t6.txt", "t7.txt", "t8.txt"};
    int k = 3;

    // Load stopwords
    unordered_set<string> stopwords = loadStopwords("stopwords.txt");

    // --- Without Removing Stopwords ---
    cout << "=== Similarity Matrix without Stopwords Removed ===\n";
    vector<unordered_set<unsigned long>> allHashedKGramsWithout = processFiles(fileNames, k);
    printSimilarityMatrix(allHashedKGramsWithout, fileNames);

    // --- With Removing Stopwords ---
    cout << "\n=== Similarity Matrix with Stopwords Removed ===\n";
    vector<unordered_set<unsigned long>> allHashedKGramsWith = processFiles(fileNames, k, &stopwords);
    printSimilarityMatrix(allHashedKGramsWith, fileNames);

    return 0;
}

