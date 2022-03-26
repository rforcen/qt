#pragma once

#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>

class Wordle {

public:
  Wordle() {}
  const QString DEFAULT_DICTIONARY = "../wordle/0_palabras_todas.txt";

  int nletters = 5;

  QVector<QPair<QString, QString>> probes;
  QStringList dict, words, matchWords;

  void changeNLetters(int nletters) {
    this->nletters = nletters;
    probes.clear();
  }

  void loadDict(QString dictFile = "") { // load dict & words

    QString accents = "áéíóúàèìòùäëïöü", vowels = "aeiou";

    QFile file(dictFile == "" ? DEFAULT_DICTIONARY : dictFile);
    if (!file.open(QIODevice::ReadOnly))
      return;

    QTextStream in(&file);

    dict.clear();

    while (!in.atEnd()) {
      QString line = in.readLine();

      // replace accents / vowels
      for (int i = 0; i < line.size(); i++) {
        if (accents.contains(line[i]))
          line[i] = vowels[accents.indexOf(line[i]) % vowels.size()];
      }

      if (line[0] != '(' && line.size() == nletters)
        dict.append(line);
    }
    file.close();

    dict.sort();
    dict.removeDuplicates();

    words = dict;
  }

  void reset() { words = dict; }

  void resetProbes() { probes.clear(); }

  void addProbe(QString tw, QString th) { probes.push_back({tw, th}); }

  void check(void) {

    // lgs: letters containing 'g' hit
    QVector<QString> lgs, lys;
    for (auto &wp : probes) {
      auto wrd = wp.first, pp = wp.second;
      QString lg, ly;
      for (int i = 0; i < nletters; i++) {
        auto l = wrd[i], p = pp[i];

        if (p == 'g')
          lg += l;
        if (p == 'y')
          ly += l;
      }
      lgs.push_back(lg);
      lys.push_back(ly);
    }

    // find match words

    matchWords.clear();
    for (auto &word : words) { // all dict
      bool isValid = true;

      for (auto ix = 0; ix < probes.size() and isValid; ix++) { // all test

        auto testWrd = probes[ix].first, testHit = probes[ix].second;

        for (auto i = 0; i < testWrd.size(); i++) { // chars in test

          auto letter = testWrd[i], hit = testHit[i];
          auto w_in_lt = word.contains(letter);

          if (hit == 'g' and word[i] != letter) {
            isValid = false;
            break;
          }
          if (hit == 'y' and (!w_in_lt or word[i] == letter)) {
            isValid = false;
            break;
          }
          if (hit == '-' and w_in_lt and
              (!lgs[ix].contains(letter) and !lys[ix].contains(letter))) {

            isValid = false;
            break;
          }
        }
      }
      if (isValid)
        matchWords.push_back(word);
    }

    words = matchWords;
  }
};
