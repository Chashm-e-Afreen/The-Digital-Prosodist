﻿#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "meters-def.h"

#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QSet>
#include <QSpinBox>

#include <chrono>

#define TOTAL_DICT_WORDS 99421

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  push_button_shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);

  connect(this->push_button_shortcut, SIGNAL(activated()), this, SLOT(on_pushButton_clicked()));

  taqti_but_stylesheet = "color: white; background-color: rgb(35, 40, 45); border: none; padding: 5px; margin-bottom: 5px; border-left: 2px solid black;border-right: 2px solid black;";
  islah_but_stylesheet = taqti_but_stylesheet;

  ui->taqtiButton->setStyleSheet(taqti_but_stylesheet + "color: rgb(15, 126, 225); font-weight: bold; border-bottom: 4px solid rgb(15, 126, 225);");
  ui->islahButton->setStyleSheet(islah_but_stylesheet);

  mode = ProgramMode::TAQTI;

  meters_in_bin.reserve(int(Meter_map.size()));

  for (const auto& pair : Meter_map)
    {
      meters_in_bin.push_back(QString::fromStdWString(pair.first));
    }

  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

  QString dict_file_path = "data/words_murrab_weight_unique.txt";

  QFile file_read(dict_file_path);

  if (!file_read.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QMessageBox::warning(this, "Error", "Unable to open dictionary to fetch words");
      return;
    }

  QTextStream text_stream(&file_read);

  dict_cache.reserve(TOTAL_DICT_WORDS * 2);

  while(!text_stream.atEnd())
    {
      QStringList line = text_stream.readLine().split(',');

      if (line.size() != 3) continue;

      dict_cache.insertMulti(line[0], {line[0], line[1],line[2]}); // With word as a key
      dict_cache.insertMulti(line[1], {line[0], line[1], line[2]}); // With murrab also as a key (User can potentially enter word with symbols also so, without this we would have rejected valid word)
    }

  file_read.close();
}

MainWindow::~MainWindow()
{
  delete ui;
}

QString MainWindow::remove_symbols(const QString& user_entered_word)
{
  QString new_word;

  for (int i = 0; i < user_entered_word.size(); i++)
    {

      if ((user_entered_word[i] >= 1613 && user_entered_word[i] <= 1618) && (user_entered_word[i] != 1616 || i != user_entered_word.size() - 1))
        continue;

      new_word += user_entered_word[i];
    }

  return new_word;

}

QVector<QStringList> MainWindow::get_user_input()
{

  auto start = std::chrono::high_resolution_clock::now();

  QVector<QStringList> user_input;

  QString user_entered_combined_words = ui->textEdit->toPlainText();

  QStringList user_entered_individual_lines = user_entered_combined_words.split("\n", QString::SkipEmptyParts);

  int total_lines = user_entered_individual_lines.size();

  user_input.resize(total_lines);

  for(int i = 0; i < total_lines; i++)
    {
      QStringList individual_words = user_entered_individual_lines[i].split(QRegExp(u8"[\\s+،,\"]"), QString::SkipEmptyParts);

      for (int j = 0; j < individual_words.size(); j++)
        {
          individual_words[j] = remove_symbols(individual_words[j]);
        }

      user_input[i] = individual_words;
    }

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Splitting user input: " << end.count() << "\n";

  return user_input;
}

QVector<QStringList> MainWindow::get_murrab_weight(const QStringList& user_entered_line)
{
  auto start = std::chrono::high_resolution_clock::now();
  QVector<QStringList> words_murrabs_weights(user_entered_line.size());


  for (int i = 0; i < user_entered_line.size(); i++)
    {

      QString word = user_entered_line[i];
      QString last_two_letters = word;
      QString last_three_letters = word;

      if (word.size() > 1)
        {
          last_two_letters = word.mid(word.size() - 2, 2);
        }
      if (word.size() > 2)
        {
          last_three_letters = word.mid(word.size() - 3, 3);
        }

      bool found_hamza_e_izafat  = (word.back() == L'ۂ');

      if (found_hamza_e_izafat)
        {
          word.chop(1);
          word += u8"ہِ";
        }

      // Chop (remove) the zer if we found it at the end of the word
      bool found_zaer = ( word.back() == L'ِ' );

      if (found_zaer)
        {
          word.chop(1);

          if (word.size() > 1)
            {
              last_two_letters = word.mid(word.size() - 2, 2);
            }
          if (word.size() > 2)
            {
              last_three_letters = word.mid(word.size() - 3, 3);
            }
        }

      bool found_ea = (word.size() > 3 && last_two_letters == u8"ئے" && word != u8"ہوئے" );

      if (found_ea)
        {
          word = word.chopped(2);
        }
      
      bool found_noon_ghunna = (word.back() == L'ں');

      bool found_oun = (last_two_letters == u8"وں");

      bool found_oun_yen = (last_two_letters == u8"یں" || last_two_letters == u8"وں");
      
      bool found_bariye = (word.back() == L'ے' && last_two_letters != u8"ئے");

      bool found_wao = (word.back() == L'و');

      bool found_an = (word.size() > 3 && last_two_letters == u8"ان");

      bool found_aan = (word.size() > 3 && last_two_letters == u8"اں");

      bool found_oun_hamzawao = (word.size() > 2 && last_two_letters == u8"ؤں");

      bool found_gan = (word.size() > 4 && last_three_letters == u8"گان");

      bool found_gaan = (word.size() > 4 && last_three_letters == u8"گاں");

      bool found_hamza_yen = (word.size() > 4 && last_three_letters == u8"ئیں");

      QChar first_letter = word.front(); // Checking the first letter of current word

      auto AllowedFirstLetter_find_iterator =  AllowedFirstLetter_set.find(first_letter.unicode()); // Find first character of user entered word in our letter map and its starting position in dictionary

      if (AllowedFirstLetter_find_iterator == AllowedFirstLetter_set.end()) // We did't find the letter in AllowedFirstLetter_set
        {
          continue;
        }

      auto dict_cache_find_iterator = dict_cache.find(word);

      if (dict_cache_find_iterator != dict_cache.end())
        {
          words_murrabs_weights[i] = dict_cache_find_iterator.value();

          if (found_zaer || found_ea || found_hamza_e_izafat || found_oun_yen || found_bariye || found_noon_ghunna)
            {
              words_murrabs_weights[i][0] = user_entered_line[i];
            }

        }

      if (found_ea && dict_cache_find_iterator == dict_cache.end())
        {

          word += u8"ئے";

          dict_cache_find_iterator = dict_cache.find(word);

          if (dict_cache_find_iterator != dict_cache.end())
            {
              words_murrabs_weights[i] = dict_cache_find_iterator.value();
            }
        }

      else if (found_noon_ghunna && dict_cache_find_iterator == dict_cache.end()) // found non ghunna at the end of the word
        {
          word.back() = L'ن';

          dict_cache_find_iterator = dict_cache.find(word);

          if (dict_cache_find_iterator != dict_cache.end())
            {
              words_murrabs_weights[i] = dict_cache_find_iterator.value();
              words_murrabs_weights[i][0] = user_entered_line[i];
              words_murrabs_weights[i][2].chop(1);
            }

          else if (found_oun_yen) // we didn't find word after replacing nun-ghuna with nun so, seeing if the word possibly end on oun and yen and checking for word in dict after chopping these letters from user entered word
            {
              word.chop(2);

              dict_cache_find_iterator = dict_cache.find(word);

              if (dict_cache_find_iterator != dict_cache.end())
                {
                  words_murrabs_weights[i] = dict_cache_find_iterator.value();
                  words_murrabs_weights[i][0] = user_entered_line[i];
                }

              else if (found_oun) // we were unable to find the word even after chopping the oun and yun, if the word only had oun at its end we would like to add 'he' at its place and then check for word again (here the word will be already chopped so we just need to append 'he')
                {
                  word += u8"ہ";

                  dict_cache_find_iterator = dict_cache.find(word);

                  if (dict_cache_find_iterator != dict_cache.end())
                    {
                      words_murrabs_weights[i] = dict_cache_find_iterator.value();
                      words_murrabs_weights[i][0] = user_entered_line[i];
                    }
                }

              else if (found_hamza_yen)
                {
                  word.chop(1); // We already chopped two letters for normal 'yen' check

                  dict_cache_find_iterator = dict_cache.find(word);

                  if (dict_cache_find_iterator != dict_cache.end())
                    {
                      words_murrabs_weights[i] = dict_cache_find_iterator.value();
                      words_murrabs_weights[i][0] = user_entered_line[i];
                    }
                }
            }

          else if (found_aan)
            {
              word.chop(2);

              dict_cache_find_iterator = dict_cache.find(word);

              if (dict_cache_find_iterator != dict_cache.end())
                {
                  words_murrabs_weights[i] = dict_cache_find_iterator.value();

                  words_murrabs_weights[i][2].back() = L'1';

                  words_murrabs_weights[i][2] += L'0';

                  words_murrabs_weights[i][0] = user_entered_line[i];
                }

              else if (found_gaan)
                {
                  word.back() = L'ہ';

                  dict_cache_find_iterator = dict_cache.find(word);

                  if (dict_cache_find_iterator != dict_cache.end())
                    {
                      words_murrabs_weights[i] = dict_cache_find_iterator.value();

                      words_murrabs_weights[i][2].chop(1);

                      words_murrabs_weights[i][2].back() = L'1';

                      words_murrabs_weights[i][2] += u8"10";

                      words_murrabs_weights[i][0] = user_entered_line[i];
                    }
                }
            }

          else if (found_oun_hamzawao)
            {
              word.chop(2);

              dict_cache_find_iterator = dict_cache.find(word);

              if (dict_cache_find_iterator != dict_cache.end())
                {
                  words_murrabs_weights[i] = dict_cache_find_iterator.value();

                  words_murrabs_weights[i][0] = user_entered_line[i];
                }
            }

        }

      else if (found_bariye)
        {
          word.back() = L'ا';

          dict_cache_find_iterator = dict_cache.find(word);

          if (dict_cache_find_iterator != dict_cache.end())
            {
              words_murrabs_weights[i] = dict_cache_find_iterator.value();
              words_murrabs_weights[i][0] = user_entered_line[i];

            }

          else
            {
              word.back() = L'ہ';

              dict_cache_find_iterator = dict_cache.find(word);

              if (dict_cache_find_iterator != dict_cache.end())
                {
                  words_murrabs_weights[i] = dict_cache_find_iterator.value();
                  words_murrabs_weights[i][0] = user_entered_line[i];
                }
            }
        }

      else if (found_wao)
        {
          word.chop(1);

          dict_cache_find_iterator = dict_cache.find(word);

          if (dict_cache_find_iterator != dict_cache.end())
            {
              words_murrabs_weights[i] = dict_cache_find_iterator.value();

              words_murrabs_weights[i][2].back() = L'1';

              words_murrabs_weights[i][2] += L'0';

              words_murrabs_weights[i][0] = user_entered_line[i];
            }
        }

      else if (found_an)
        {
          word.chop(2);

          dict_cache_find_iterator = dict_cache.find(word);

          if (dict_cache_find_iterator != dict_cache.end())
            {
              words_murrabs_weights[i] = dict_cache_find_iterator.value();

              words_murrabs_weights[i][2].back() = L'1';

              words_murrabs_weights[i][2] += u8"01";

              words_murrabs_weights[i][0] = user_entered_line[i];
            }

          else if (found_gan)
            {
              word.back() = L'ہ';

              dict_cache_find_iterator = dict_cache.find(word);

              if (dict_cache_find_iterator != dict_cache.end())
                {
                  words_murrabs_weights[i] = dict_cache_find_iterator.value();

                  words_murrabs_weights[i][2].chop(1);

                  words_murrabs_weights[i][2].back() = L'1';

                  words_murrabs_weights[i][2] += u8"101";

                  words_murrabs_weights[i][0] = user_entered_line[i];
                }
            }
        }
    }

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Fetching Weights: " << end.count() << "\n";

  return words_murrabs_weights;
}

bool MainWindow::has_different_weights(const QString& word)
{

  QSet<QString> different_unique_weights;

  auto dict_cache_find_iterator = dict_cache.find(word);

  while (dict_cache_find_iterator != dict_cache.end() && dict_cache_find_iterator.key() == word)
    {
      if (dict_cache_find_iterator.value().size() < 3)
        {
          dict_cache_find_iterator++;
          continue;
        }

      different_unique_weights.insert(dict_cache_find_iterator.value()[2]);

      dict_cache_find_iterator++;

      if (different_unique_weights.size() > 1)
        return true;
    }

  return different_unique_weights.size() > 1;
}

QList<QString> MainWindow::get_different_weights_of_word(const QString& word)
{
  QSet<QString> different_unique_weights;

  auto dict_cache_find_iterator = dict_cache.find(word);

  while(dict_cache_find_iterator != dict_cache.end() && dict_cache_find_iterator.key() == word)
    {
      if (dict_cache_find_iterator.value().size() < 3)
        {
          dict_cache_find_iterator++;
          continue;
        }

      QString weight = dict_cache_find_iterator.value()[2];

      different_unique_weights.insert(weight);

      dict_cache_find_iterator++;
    }

  return different_unique_weights.toList();
}

QString accumulate(const QStringList& weights)
{
  QString accum;

  for (int i = 0; i < weights.size(); i++)
    {
      accum += weights[i];
    }

  return accum;
}

Accumulated_Weight islah(QVector<Accumulated_Weight>& accumulated_weights_per_line, QStringList user_entered_line, const QString& meter)
{
  Accumulated_Weight* best_accumulated_weight = nullptr;
  int min_rejected_count = INT_MAX;

  std::wstring meter_bin = meter.toStdWString();

  for (int i = 0; i < accumulated_weights_per_line.size(); i++)
    {
      size_t cur_loc = 0;
      int aw_cur_word_index = 0;

      for (int j = 0; j < user_entered_line.size(); j++)
        {

          std::wstring cur_weight;

          if(aw_cur_word_index < accumulated_weights_per_line[i].weights.size())
              accumulated_weights_per_line[i].weights[aw_cur_word_index].toStdWString();

          if (aw_cur_word_index >= accumulated_weights_per_line[i].words.size() || user_entered_line[j] != accumulated_weights_per_line[i].words[aw_cur_word_index])
            {
              cur_loc += cur_weight.size();

              aw_cur_word_index--;
            }

          else if (aw_cur_word_index < accumulated_weights_per_line[i].words.size())
            {

              size_t find_loc = meter_bin.find(cur_weight, cur_loc);

              if (find_loc != cur_loc)
                {
                  accumulated_weights_per_line[i].rejected[aw_cur_word_index] = true;
                  accumulated_weights_per_line[i].rejected_count++;
                }

              cur_loc += cur_weight.size();
            }

          aw_cur_word_index++;

        }

      if (accumulated_weights_per_line[i].rejected_count != 0)
        {
          accumulated_weights_per_line[i].has_meter = false;
        }

      if (accumulated_weights_per_line[i].rejected_count < min_rejected_count)
        {
          best_accumulated_weight = &accumulated_weights_per_line[i];
          min_rejected_count = accumulated_weights_per_line[i].rejected_count;
        }
    }

  return (best_accumulated_weight  ? *best_accumulated_weight : Accumulated_Weight());
}

QStringList get_matched_meters(QVector<Accumulated_Weight>& accumulated_weights)
{
  QStringList meters;

  for (int i = 0; i < accumulated_weights.size(); i++)
    {

      if(!accumulated_weights[i].bin.isEmpty() && accumulated_weights[i].bin.back()=='1')
        {
          accumulated_weights[i].bin.chop(1);
          accumulated_weights[i].weights.back().chop(1);
          accumulated_weights[i].is_tasbeegh_o_azala = true;
        }

      QString accumlated_weights = accumulate(accumulated_weights[i].weights);

      auto meter_find_iterator = Meter_map.find(accumulated_weights[i].bin.toStdWString());

      if (meter_find_iterator != Meter_map.end())
        {
          meters.push_back(QString::fromStdWString(meter_find_iterator->first));

          if(accumulated_weights.size() >= 2)
            {
              if(accumulated_weights[i].bin.back() == L'1')
                accumulated_weights[i].is_tasbeegh_o_azala = true;
            }

        };
    }

  return meters;
}

QVector<Accumulated_Weight> MainWindow::get_accumulated_weight(const QVector<QStringList>& words_murrab_weight_per_line)
{
  auto start = std::chrono::high_resolution_clock::now();

  int size = words_murrab_weight_per_line.size();

  if(size <= 0)
    return QVector<Accumulated_Weight>();

  QVector<Accumulated_Weight> accumulated_weights(1, {}); // There will always be one accumulated weight

  QChar prev_word_last_letter;

  int new_accumulated_weight_size = accumulated_weights.size();
  int prev_accumulated_weight_size = new_accumulated_weight_size;

  for (int i = 0; i < size; i++) // Iterating every word
    {

      if (words_murrab_weight_per_line[i].size() != 3) continue;

      QString individual_word = words_murrab_weight_per_line[i][0];
      QString last_two_letters = individual_word;
      QString last_three_letters = individual_word;

      if (individual_word.size() > 1)
        {
          last_two_letters = individual_word.mid(individual_word.size() - 2, 2);
        }
      if (individual_word.size() > 2)
        {
          last_three_letters = individual_word.mid(individual_word.size() - 3, 3);
        }

      QString individual_weight = words_murrab_weight_per_line[i][2];

      QChar last_letter = individual_word.back();
      QChar first_letter = individual_word.front();
      QChar last_weight = individual_weight.back();

      bool has_multiple_weights = has_different_weights(individual_word);

      if (!has_multiple_weights)
        {
          for (int j = 0; j < accumulated_weights.size(); j++)
            {
              if (individual_word == u8"و")
                break;

              accumulated_weights[j].bin += individual_weight;
              accumulated_weights[j].words.push_back(individual_word);
              accumulated_weights[j].weights.push_back(individual_weight);
              accumulated_weights[j].rejected.push_back(false);

              Q_ASSERT(accumulated_weights[j].bin == accumulate(accumulated_weights[j].weights));
            }
        }

      else
        {
          QList<QString> different_weights = get_different_weights_of_word(individual_word);

          accumulated_weights.reserve(accumulated_weights.size() * different_weights.size());

          if (i == 0)
            {
              accumulated_weights.pop_back();
              new_accumulated_weight_size = prev_accumulated_weight_size = 0;
            }

          for (QString weight: different_weights)
            {
              if (i == 0)
                {
                  Accumulated_Weight new_acc_weight;
                  new_acc_weight.bin = weight;
                  new_acc_weight.words.push_back(individual_word);
                  new_acc_weight.weights.push_back(weight);
                  new_acc_weight.rejected.push_back(false);

                  accumulated_weights.push_back(new_acc_weight);

                  Q_ASSERT(new_acc_weight.bin == accumulate(new_acc_weight.weights));

                  new_accumulated_weight_size++;
                }
              else
                {
                  for (int k = 0; k < prev_accumulated_weight_size; k++)
                    {
                      Accumulated_Weight new_acc_weight;
                      new_acc_weight.bin = accumulated_weights[k].bin + weight;
                      new_acc_weight.weights = accumulated_weights[k].weights;
                      new_acc_weight.words = accumulated_weights[k].words;
                      new_acc_weight.rejected = accumulated_weights[k].rejected;

                      new_acc_weight.weights.push_back(weight);
                      new_acc_weight.words.push_back(individual_word);
                      new_acc_weight.rejected.push_back(false);

                      accumulated_weights.push_back(new_acc_weight);


                      Q_ASSERT(new_acc_weight.bin == accumulate(new_acc_weight.weights));

                      new_accumulated_weight_size++;
                    }
                }
            }

          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {
              accumulated_weights.pop_front();
              new_accumulated_weight_size--;
            }

          prev_accumulated_weight_size = new_accumulated_weight_size;
        }

      if (i != 0 && individual_word.size() > 1 && (prev_word_last_letter != L'ا' && prev_word_last_letter != L'ہ' && prev_word_last_letter != L'ۂ' &&
                                                   prev_word_last_letter != L'ی' && prev_word_last_letter != L'ے' && prev_word_last_letter != L'و'))
        {

          if (first_letter == L'ا')
            {
              for (int k = 0; k < prev_accumulated_weight_size; k++)
                {

                  if (i == 1) continue;

                  Accumulated_Weight new_accumulated_weight;

                  new_accumulated_weight.bin = accumulated_weights[k].bin;

                  new_accumulated_weight.bin[accumulated_weights[k].bin.size() - accumulated_weights[k].weights.back().size() - 1] = L'1';
                  new_accumulated_weight.bin.remove(accumulated_weights[k].bin.size() - accumulated_weights[k].weights.back().size(), 1);

                  new_accumulated_weight.weights = accumulated_weights[k].weights;
                  new_accumulated_weight.words = accumulated_weights[k].words;
                  new_accumulated_weight.rejected = accumulated_weights[k].rejected;

                  new_accumulated_weight.weights[new_accumulated_weight.weights.size() - 2].back() = L'1';
                  new_accumulated_weight.weights.back().remove(0, 1);

                  accumulated_weights.push_back(new_accumulated_weight);

                  Q_ASSERT(new_accumulated_weight.bin == accumulate(new_accumulated_weight.weights));
                  new_accumulated_weight_size++;
                }

              prev_accumulated_weight_size = new_accumulated_weight_size;
            }

          else if (first_letter == L'آ')
            {
              for (int k = 0; k < prev_accumulated_weight_size; k++)
                {

                  if (i == 1) continue;

                  Accumulated_Weight new_accumulated_weight;

                  new_accumulated_weight.bin = accumulated_weights[k].bin;

                  new_accumulated_weight.bin[accumulated_weights[k].bin.size() - accumulated_weights[k].weights.back().size() - 1] = L'1';
                  new_accumulated_weight.bin[accumulated_weights[k].bin.size() - accumulated_weights[k].weights.back().size() + 1] = L'0';
                  new_accumulated_weight.bin.remove(accumulated_weights[k].bin.size() - accumulated_weights[k].weights.back().size(), 1);

                  new_accumulated_weight.weights = accumulated_weights[k].weights;
                  new_accumulated_weight.words = accumulated_weights[k].words;
                  new_accumulated_weight.rejected = accumulated_weights[k].rejected;

                  new_accumulated_weight.weights[new_accumulated_weight.weights.size() - 2].back() = L'1';
                  new_accumulated_weight.weights.back().remove(0, 1);
                  new_accumulated_weight.weights.back().front() = L'0';

                  accumulated_weights.push_back(new_accumulated_weight);

                  Q_ASSERT(new_accumulated_weight.bin == accumulate(new_accumulated_weight.weights));

                  new_accumulated_weight_size++;
                }
            }

          prev_accumulated_weight_size = new_accumulated_weight_size;
        }


      if (individual_word.size() > 3 && last_two_letters == u8"ئے" && individual_word != u8"ہوئے" && dict_cache.find(individual_word) == dict_cache.end())
        {
          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {

              accumulated_weights[k].bin += L'1';
              accumulated_weights[k].weights.back() += L'1';

              Accumulated_Weight new_accumulated_weight_one;
              new_accumulated_weight_one.bin = accumulated_weights[k].bin + L'0';
              new_accumulated_weight_one.weights = accumulated_weights[k].weights;
              new_accumulated_weight_one.words = accumulated_weights[k].words;
              new_accumulated_weight_one.rejected = accumulated_weights[k].rejected;

              new_accumulated_weight_one.weights.back() += L'0';

              if (last_three_letters== u8"وئے")
                {
                  Accumulated_Weight new_accumulated_weight_two;
                  new_accumulated_weight_two.bin = accumulated_weights[k].bin;
                  new_accumulated_weight_two.weights = accumulated_weights[k].weights;
                  new_accumulated_weight_two.words = accumulated_weights[k].words;
                  new_accumulated_weight_two.rejected = accumulated_weights[k].rejected;

                  new_accumulated_weight_two.bin.chop(1);
                  new_accumulated_weight_two.bin.back() = L'1';

                  new_accumulated_weight_two.weights.back().chop(1);
                  new_accumulated_weight_two.weights.back().back() = L'1';

                  Accumulated_Weight new_accumulated_weight_three;
                  new_accumulated_weight_three.bin = new_accumulated_weight_two.bin;
                  new_accumulated_weight_three.weights = new_accumulated_weight_two.weights;
                  new_accumulated_weight_three.words = new_accumulated_weight_two.words;
                  new_accumulated_weight_three.rejected = new_accumulated_weight_two.rejected;

                  new_accumulated_weight_three.bin += L'0';
                  new_accumulated_weight_three.weights.back() += L'0';

                  accumulated_weights.push_back(new_accumulated_weight_two);
                  accumulated_weights.push_back(new_accumulated_weight_three);

                  Q_ASSERT(new_accumulated_weight_two.bin == accumulate(new_accumulated_weight_two.weights));
                  Q_ASSERT(new_accumulated_weight_three.bin == accumulate(new_accumulated_weight_three.weights));

                  new_accumulated_weight_size += 2;
                }

              accumulated_weights.push_back(new_accumulated_weight_one);

              Q_ASSERT(new_accumulated_weight_one.bin == accumulate(new_accumulated_weight_one.weights));

              new_accumulated_weight_size++;
            }
        }

      else if(last_letter == L'ِ' || last_letter == L'ۂ')
        {
          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {
              accumulated_weights[k].bin.back() = L'1';
              accumulated_weights[k].weights.back().back() = L'1';

              Accumulated_Weight new_accumulated_weight;

              new_accumulated_weight.bin = accumulated_weights[k].bin + L'0';
              new_accumulated_weight.weights = accumulated_weights[k].weights;
              new_accumulated_weight.words = accumulated_weights[k].words;
              new_accumulated_weight.rejected = accumulated_weights[k].rejected;

              new_accumulated_weight.weights.back() += L'0';

              accumulated_weights.push_back(new_accumulated_weight);

              Q_ASSERT(new_accumulated_weight.bin == accumulate(new_accumulated_weight.weights));

              new_accumulated_weight_size++;
            }
        }

      else if (individual_word.size() > 1 && individual_word != u8"اے" && last_weight != L'1' && (last_letter == L'ا' || last_letter == L'ہ' ||
                                                                                                  last_letter == L'ی' || last_letter == L'ے' ||
                                                                                                  last_letter == L'و' || last_letter == L'ؤ')){
          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {
              Accumulated_Weight new_accumulated_weight;

              new_accumulated_weight.bin = accumulated_weights[k].bin;
              new_accumulated_weight.weights = accumulated_weights[k].weights;
              new_accumulated_weight.words = accumulated_weights[k].words;
              new_accumulated_weight.rejected = accumulated_weights[k].rejected;

              new_accumulated_weight.bin.chop(1);
              new_accumulated_weight.weights.back().chop(1);

              accumulated_weights.push_back(new_accumulated_weight);

              Q_ASSERT(new_accumulated_weight.bin == accumulate(new_accumulated_weight.weights));

              new_accumulated_weight_size++;
            }

        }

      else if (individual_word == u8"و" || (individual_word.size() > 3 && ((last_two_letters  == u8"یں" && last_three_letters != u8"ئیں")|| last_two_letters == u8"وں") && dict_cache.find(individual_word) == dict_cache.end()))
        {
          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {
              //              if(accumulated_weights[k].back() != L'ا' && accumulated_weights[k].back() != L'و')
              //                {

              if (accumulated_weights[k].bin.isEmpty()) continue;

              accumulated_weights[k].bin.back() = L'1';
              accumulated_weights[k].weights.back().back() = L'1';

              Accumulated_Weight new_accumulated_weight;

              new_accumulated_weight.bin = accumulated_weights[k].bin + L'0';
              new_accumulated_weight.weights = accumulated_weights[k].weights;
              new_accumulated_weight.words = accumulated_weights[k].words;
              new_accumulated_weight.rejected = accumulated_weights[k].rejected;

              new_accumulated_weight.weights.back() += L'0';

              accumulated_weights.push_back(new_accumulated_weight);

              Q_ASSERT(new_accumulated_weight.bin == accumulate(new_accumulated_weight.weights));

              new_accumulated_weight_size++;
            }
          //              else
          //                 {
          //                  accumulated_weights[k] += '0';
          //                  QString new_accumulated_weight1 = accumulated_weights[k] + "10";
          //                  accumulated_weights.push_back(new_accumulated_weight1);

          //                  new_accumulated_weight_size++;
          //                }
          //            }
        }

      else if (((individual_word.size() > 3 && last_two_letters == u8"ؤں")
                || (individual_word.size() > 4 && last_three_letters == u8"ئیں"))
               && (dict_cache.find(individual_word) == dict_cache.end()))
        {

          QString chopped_individual_word = individual_word.chopped((last_two_letters == u8"ؤں") ? 2:3 );

          if (chopped_individual_word.back() == L'ا')
            {
              for (int k = 0; k < prev_accumulated_weight_size; k++)
                {

                  accumulated_weights[k].bin += L'1';
                  accumulated_weights[k].weights.back() += L'1';

                  Accumulated_Weight new_accumulated_weight;
                  new_accumulated_weight.bin = accumulated_weights[k].bin + L'0';
                  new_accumulated_weight.weights = accumulated_weights[k].weights;
                  new_accumulated_weight.words = accumulated_weights[k].words;
                  new_accumulated_weight.rejected = accumulated_weights[k].rejected;

                  new_accumulated_weight.weights.back() += L'0';

                  accumulated_weights.push_back(new_accumulated_weight);

                  Q_ASSERT(new_accumulated_weight.bin == accumulate(new_accumulated_weight.weights));

                  new_accumulated_weight_size++;
                }
            }
        }


      prev_accumulated_weight_size = new_accumulated_weight_size;
      prev_word_last_letter = individual_word.back();

    }


  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Fetching Accumulative Weight: " << end.count() << "\n";

  return accumulated_weights;
}

void MainWindow::display_meters(const QVector<QStringList>& words_murrab_weight_per_line)
{
  auto start = std::chrono::high_resolution_clock::now();

  int size = words_murrab_weight_per_line.size();

  if(size <= 0)
    {
      return;
    }

  QVector<Accumulated_Weight> accumulated_weights = get_accumulated_weight(words_murrab_weight_per_line);

  bool tasbeegh_o_azala = false;

  bool found_meter = false;

  ui->textEdit->insertPlainText(u8"\n\nافاعیل: ");
  int index = 0;

  for (int i = 0; i < accumulated_weights.size(); i++)
    {
      tasbeegh_o_azala = false;
      if(!accumulated_weights[i].bin.isEmpty() && accumulated_weights[i].bin.back()=='1')
        {
          accumulated_weights[i].bin.chop(1);
          tasbeegh_o_azala = true;
        }

      index = i;

      auto meters_find_iterator = Meter_map.find(accumulated_weights[i].bin.toStdWString());
      if (meters_find_iterator != Meter_map.end())
        {
          QString meter_value = QString::fromStdWString(meters_find_iterator->second);
          if(accumulated_weights.size()>= 2)
            {
              if(accumulated_weights[i].bin.back() == L'1')
                tasbeegh_o_azala = true;
            }
          if(tasbeegh_o_azala)
            {
              meter_value.insert(meter_value.size()-1,L'ا');
            }
          ui->textEdit->insertPlainText(meter_value + " " +"(" + accumulated_weights[i].bin+"1" + ")");
          found_meter = true;

          break;
        }
    }

  if (!found_meter)
    {
      ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی مانوس بحر نہیں مل سکی </span>|");
    }


  ui->textEdit->insertPlainText(u8"\nبحر: ");
  auto meters_find_iterator = Names_map.find(accumulated_weights[index].bin.toStdWString());

  if (meters_find_iterator != Names_map.end())
    {
      QString name_value = QString::fromStdWString(meters_find_iterator->second);
      QString additional_zuhaf = "";

      if(tasbeegh_o_azala)
        {
          QString rukn = accumulated_weights[index].bin.mid(accumulated_weights[index].bin.size()-4,4);
          if(rukn== u8"0110" && !name_value.contains(u8"مذال"))
            {
              additional_zuhaf = u8"مذال";
            }
          else if(rukn != u8"0110" && !name_value.contains(u8"مسبغ") && !name_value.contains(u8"مذال"))
            {

              additional_zuhaf = u8"مسبغ";
            }

        }
      ui->textEdit->insertPlainText(name_value + " " + additional_zuhaf);
    }
  else
    {
      ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی بحر نہیں مل سکی </span>|");
    }

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Displaying Names: " << end.count() << "\n";
}

void MainWindow::display_arkans(const QVector<QStringList>& words_murrab_weight_per_line)
{
  auto start = std::chrono::high_resolution_clock::now();
  int size = words_murrab_weight_per_line.size();

  if (size <= 0)
    return;

  ui->textEdit->insertPlainText(u8"\nتحلیلِ الفاظ: ");


  for (int i = 0; i < size; i++)
    {
      if (words_murrab_weight_per_line[i].size() != 3)
        {
          ui->textEdit->insertHtml(u8"'<span style='color:red'>X</span>' ");
          continue;
        }

      const QString weight = words_murrab_weight_per_line[i][2];

      auto arkaan_find_iterator = Arkan_map.find(weight.toStdWString());

      if (arkaan_find_iterator != Arkan_map.end())
        {
          const QString rukan = QString::fromStdWString(arkaan_find_iterator->second);

          bool has_multiple_weights = has_different_weights(words_murrab_weight_per_line[i][0]);

          if (has_multiple_weights)
            {
              ui->textEdit->insertHtml(u8"<span style='color:#5900b3'>" + rukan + u8"</span> ");
            }
          else
            {
              ui->textEdit->insertPlainText(rukan + " ");
            }
        }
      else
        {
          ui->textEdit->insertHtml(u8"<span style='color:red'>'X' </span>");
        }
    }

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Displaying Arkans: " << end.count() << "\n";
}

QString max_count_meter(const QVector<QStringList>& found_meters)
{
  QMap<QString, int> meters_count_in_line;
  QString max_meter;
  int count = 0;

  for (int i = 0; i < found_meters.size(); i++)
    {
      for (int j = 0; j < found_meters[i].size(); j++)
        {
          meters_count_in_line[found_meters[i][j]]++;
        }
    }

  for (auto it = meters_count_in_line.begin(); it != meters_count_in_line.end(); it++)
    {
      if (it.value() > count)
        {
          count = it.value();
          max_meter = it.key();
        }
    }

  if (count == 0)
    {
      QString default_meter = "1101010110101011010101101010";  //  ہزج مثمن سالم | مفاعیلن مفاعیلن مفاعیلن مفاعیلن
      max_meter = default_meter;
    }

  return max_meter;
}

void MainWindow::on_pushButton_clicked()
{

  auto start = std::chrono::high_resolution_clock::now();

  QVector<QStringList> user_entered_lines = get_user_input();

  QVector<QStringList> words_murrabs_weights_per_line = {};

  QVector<QStringList> matched_meters_per_line;

  QVector<QStringList> all_matched_meters;

  QVector<QVector<Accumulated_Weight>> accumulated_weights;

  foreach (const QStringList& line, user_entered_lines)
    {
      words_murrabs_weights_per_line = get_murrab_weight(line);
      //      if (words_murrabs_weights_per_line.back().back().back()=='1' && words_murrabs_weights_per_line.size()>1)
      //            words_murrabs_weights_per_line.back().back().chop(1);

      accumulated_weights.push_back(get_accumulated_weight(words_murrabs_weights_per_line));

      all_matched_meters.push_back(get_matched_meters(accumulated_weights.back()));

      display_meters(words_murrabs_weights_per_line);
      display_arkans(words_murrabs_weights_per_line);
      //      display_names(words_murrabs_weights_per_line);
    }

  ui->textEdit->append( u8"\nاصلاح\n");

  QString most_matched_meter = max_count_meter(all_matched_meters);

  for (int i = 0; i < user_entered_lines.size(); i++)
    {
      ui->textEdit->append("\n");

      Accumulated_Weight aw = islah(accumulated_weights[i], user_entered_lines[i], most_matched_meter);

      int aw_cur_word_index = 0;

      for (int j = 0; j < user_entered_lines[i].size(); j++)
        {

          if (aw_cur_word_index >= aw.words.size() || user_entered_lines[i][j] != aw.words[aw_cur_word_index])
            {
              ui->textEdit->insertHtml(u8"<span style='color:red'>" + user_entered_lines[i][j] + u8"</span> ");
              aw_cur_word_index--;
            }

          else if (aw_cur_word_index < aw.words.size())
            {

              if (aw.rejected[aw_cur_word_index])
                {
                  ui->textEdit->insertHtml(u8"<span style='color:red'>" + aw.words[aw_cur_word_index] + u8"</span> ");
                }
              else
                {
                  ui->textEdit->insertHtml(u8"<span style='color:green'>" + aw.words[aw_cur_word_index] + u8"</span> ");
                }
            }

          aw_cur_word_index++;
        }
    }

  ui->textEdit->append("\n" + QString::fromStdWString(Names_map.find(most_matched_meter.toStdWString())->second));
  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Time elapsed: " << end.count() << "\n ---------------------------- \n";
}

void MainWindow::on_pushButton_2_clicked()
{
  //    qApp->quit();
  //    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
  ui->textEdit->clear();
}

void MainWindow::on_actionExit_triggered()
{
  close();
}

void MainWindow::on_taqtiButton_clicked()
{
  mode = ProgramMode::TAQTI;

  ui->taqtiButton->setStyleSheet(taqti_but_stylesheet + "color: rgb(15, 126, 225); font-weight: bold; border-bottom: 4px solid rgb(15, 126, 225);");
  ui->islahButton->setStyleSheet(islah_but_stylesheet + "font-weight: normal; border-bottom: none;");
}

void MainWindow::on_islahButton_clicked()
{
  mode = ProgramMode::ISLAH;

  ui->islahButton->setStyleSheet(islah_but_stylesheet + "color: rgb(15, 126, 225); font-weight: bold; border-bottom: 4px solid rgb(15, 126, 225);");
  ui->taqtiButton->setStyleSheet(taqti_but_stylesheet + "font-weight: normal; border-bottom: none;");
}

void MainWindow::on_actionFont_Size_triggered()
{

}
