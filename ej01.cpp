#include <iostream>
#include <vector>
// ORIGINAL: #include "vector" debe ser <vector>, no con comillas
#include <string>
#include <algorithm>
// ORIGINAL: #include <algorithm< el cierre es '>', no '<'
#include <cmath> //Aunque no es necesario.
using namespace std;
// ORIGINAL: using namespace stand; es "std", no "stand"

int main() {
    vector<string> words;
    // ORIGINAL: vector <string> words faltaba el ';'

    string s;

    while (cin >> s && s != "quit") {
        words.push_back(s);
    }
    // ORIGINAL: while cin >> s && s != "quit"
    //               words.push_back(s);
    // faltaban parentesis en la condicion

    sort(words.begin(), words.end());

    vector<string> w2;
    if (0 < words.size()) {
        w2.push_back(words[0]);
    }

    for (int i = 1; i < (int)words.size(); ++i) {
        if (words[i - 1] != words[i]) {
            w2.push_back(words[i]);
        }
    }
    // ORIGINAL: for (int i = 1; i < words.size(); ++i)
    //               if (words[i - 1] != words[i])
    //                   w2.push_back(words[i]);
    //           }
    //faltaba { de apertura y sobraba un } suelto

    int duplicadosDistintos = 0;
    for (int i = 1; i < (int)words.size(); ++i) {
        if (words[i] == words[i - 1] &&
            (i == 1 || words[i - 1] != words[i - 2])) {
            duplicadosDistintos++;
        }
    }
    cout << "found " << duplicadosDistintos << " duplicates\n";
    // ORIGINAL: cout << "found " << words.size() - w2.size() << " duplicates\n";
    //error de logica: eso contaba copias repetidas, no palabras distintas

    for (int i = 0; i < (int)w2.size(); ++i) {
        cout << w2[i] << "\n";
    }
    // ORIGINAL: ...  w2.size(), ++i ...
    //debia ser ';' en vez de ','

    return 0;
}