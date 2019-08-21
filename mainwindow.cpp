#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "meters-def.h"

#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <chrono>

#include <QProcess>
#define TOTAL_DICT_WORDS 99455

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  push_button_shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);

  connect(this->push_button_shortcut, SIGNAL(activated()), this, SLOT(on_pushButton_clicked()));

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
      QStringList individual_words = user_entered_individual_lines[i].split(" ", QString::SkipEmptyParts);

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

      if (word.size() > 2)
        {
          last_two_letters = word.mid(word.size() - 2, 2);
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
        }

      bool found_ea = (last_two_letters == u8"ئے");

      if (found_ea)
        {
          word = word.chopped(2);
        }
      QChar first_letter = word.front(); // Checking the first letter of current word

      auto AllowedFirstLetter_find_iterator =  AllowedFirstLetter_set.find(first_letter.unicode()); // Find first character of user entered word in our letter map and its starting position in dictionary
      if (AllowedFirstLetter_find_iterator == AllowedFirstLetter_set.end()) // We did't find the letter in AllowedFirstLetter_set
        {
          rejected_cache.insert(word);
          continue;
        }



      if (rejected_cache.find(word) != rejected_cache.end())
        {
          continue;
        }

      auto found_cache_find_iterator = found_cache.find(word);
      if (found_cache_find_iterator != found_cache.end())
        {
          words_murrabs_weights[i] = found_cache_find_iterator.value();
          if (found_zaer || found_ea || found_hamza_e_izafat)
            {
              words_murrabs_weights[i][0] = user_entered_line[i];
            }
          continue;
        }

      auto dict_cache_find_iterator = dict_cache.find(word);
      if (dict_cache_find_iterator != dict_cache.end())
        {
          words_murrabs_weights[i] = dict_cache_find_iterator.value();

          if (found_zaer || found_ea || found_hamza_e_izafat)
            {
              words_murrabs_weights[i][0] = user_entered_line[i];
            }

          found_cache.insert(user_entered_line[i], dict_cache_find_iterator.value());

        }
      else
        {

          if (found_ea)
            {
              word += u8"ئے";

              dict_cache_find_iterator = dict_cache.find(word);

              if (dict_cache_find_iterator != dict_cache.end())
                {
                  words_murrabs_weights[i] = dict_cache_find_iterator.value();

                  found_cache.insert(user_entered_line[i], dict_cache_find_iterator.value());
                }
              else
                {
                  rejected_cache.insert(word);
                }

            }
          else
            {
              rejected_cache.insert(word);
            }
        }
    }

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Fetching Weights: " << end.count() << "\n";

  return words_murrabs_weights;
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

  //ui->textEdit->setPlainText(ui->textEdit->toHtml());

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Displaying Arkans: " << end.count() << "\n";
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

QVector<QString> MainWindow::get_accumulated_weight(const QVector<QStringList>& words_murrab_weight_per_line)
{
  auto start = std::chrono::high_resolution_clock::now();

  int size = words_murrab_weight_per_line.size();

  if(size <= 0)
    return QVector<QString>();

  QVector<QString> accumulated_weights(1); // There will always be one accumulated weight

  QChar prev_word_last_letter;

  int new_accumulated_weight_size = accumulated_weights.size();
  int prev_accumulated_weight_size = new_accumulated_weight_size;

  for (int i = 0; i < size; i++) // Iterating every word
    {

      if (words_murrab_weight_per_line[i].size() != 3) continue;

      QString individual_word = words_murrab_weight_per_line[i][0];
      QString last_two_letters = individual_word;

      if (individual_word.size() > 1)
        last_two_letters = individual_word.mid(individual_word.size() - 2, 2);

      QString individual_weight = words_murrab_weight_per_line[i][2];

      QChar last_letter = individual_word.back();
      QChar first_letter = individual_word.front();
      QChar last_weight = individual_weight.back();

      for (int j = 0; j < accumulated_weights.size(); j++)
        {
          if (individual_word == u8"و")
            break;
          
          accumulated_weights[j] += individual_weight;
        }

      if (has_different_weights(individual_word))
        {
          QList<QString> different_weights = get_different_weights_of_word(individual_word);

          accumulated_weights.reserve(accumulated_weights.size() * different_weights.size());

          for (QString weight: different_weights)
            {
              if (weight == individual_weight) continue;

              for (int k = 0; k < prev_accumulated_weight_size; k++)
                {
                  accumulated_weights.push_back(accumulated_weights[k] + weight);

                  new_accumulated_weight_size++;
                }
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

                  QString new_accumulated_weight = accumulated_weights[k];

                  new_accumulated_weight[accumulated_weights[k].size() - individual_weight.size() - 1] = '1';
                  new_accumulated_weight.remove(accumulated_weights[k].size() - individual_weight.size(), 1);

                  accumulated_weights.push_back(new_accumulated_weight);

                  new_accumulated_weight_size++;
                }

              prev_accumulated_weight_size = new_accumulated_weight_size;
            }

          else if (first_letter == L'آ')
            {
              for (int k = 0; k < prev_accumulated_weight_size; k++)
                {
                  QString new_accumulated_weight = accumulated_weights[k];

                  new_accumulated_weight[accumulated_weights[k].size() - individual_weight.size() - 1] = '1';
                  new_accumulated_weight[accumulated_weights[k].size() - individual_weight.size() + 1] = '0';
                  new_accumulated_weight.remove(accumulated_weights[k].size() - individual_weight.size(), 1);

                  accumulated_weights.push_back(new_accumulated_weight);

                  new_accumulated_weight_size++;
                }
            }

          prev_accumulated_weight_size = new_accumulated_weight_size;
        }


      if (last_two_letters == u8"ئے")
        {
          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {
              QString new_accumulated_weight_one = accumulated_weights[k] + u8"10";
              QString new_accumulated_weight_two = accumulated_weights[k] + u8"1";

              accumulated_weights.push_back(new_accumulated_weight_one);
              accumulated_weights.push_back(new_accumulated_weight_two);
            }

          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {
              accumulated_weights.pop_front();
            }

          new_accumulated_weight_size *= 2;
        }

      else if(last_letter == L'ِ' || last_letter == L'ۂ')
        {
          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {
              QString new_accumulated_weight_one = accumulated_weights[k];

              new_accumulated_weight_one.back() = '1';

              QString new_accumulated_weight_two = new_accumulated_weight_one;

              new_accumulated_weight_two += '0';

              accumulated_weights.push_back(new_accumulated_weight_one);
              accumulated_weights.push_back(new_accumulated_weight_two);

              new_accumulated_weight_size += 2;
            }
        }

      else if (individual_word.size() > 1 && last_weight != L'1' && (last_letter == L'ا' || last_letter == L'ہ' ||
                                                                     last_letter == L'ی' || last_letter == L'ے' ||
                                                                     last_letter == L'و'))
        {
          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {
              QString new_accumulated_weight = accumulated_weights[k];
              new_accumulated_weight.chop(1);

              accumulated_weights.push_back(new_accumulated_weight);

              new_accumulated_weight_size++;
            }

        }
      else if (individual_word == u8"و")
        {
          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {
              accumulated_weights[k].back() = '1';

              QString new_accumulated_weight = accumulated_weights[k] + '0';

              accumulated_weights.push_back(new_accumulated_weight);

              new_accumulated_weight_size++;
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
    return;

  QVector<QString> accumulated_weights = get_accumulated_weight(words_murrab_weight_per_line);

  bool found_meter = false;

  ui->textEdit->insertPlainText(u8"\nافاعیل: ");
  int index = 0;

  for (int i = 0; i < accumulated_weights.size(); i++)
    {

        if(accumulated_weights[i].back()=='1')
            accumulated_weights[i].chop(1);

      auto meters_find_iterator = Meter_map.find(accumulated_weights[i].toStdWString());
      index = i;
      if (meters_find_iterator != Meter_map.end())
        {
          const QString meter_value = QString::fromStdWString(meters_find_iterator->second);

          ui->textEdit->insertPlainText(meter_value + " " +"(" + accumulated_weights[i] + ")");
          found_meter = true;

          break;
        }
    }

  if (!found_meter)
    {
      ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی مانوس بحر نہیں مل سکی </span>|");
    }


  ui->textEdit->insertPlainText(u8"\nبحر: ");
  auto meters_find_iterator = Names_map.find(accumulated_weights[index].toStdWString());

  if (meters_find_iterator != Names_map.end())
    {
      const QString name_value = QString::fromStdWString(meters_find_iterator->second);

      ui->textEdit->insertPlainText(name_value);
    }
  else
    {
      ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی بحر نہیں مل سکی </span>|");
    }

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Displaying Names: " << end.count() << "\n";
}


void MainWindow::on_pushButton_clicked()
{

  auto start = std::chrono::high_resolution_clock::now();

  QVector<QStringList> user_entered_lines = get_user_input();

  QVector<QStringList> words_murrabs_weights_per_line;

  foreach (const QStringList line, user_entered_lines)
    {
      words_murrabs_weights_per_line = get_murrab_weight(line);
//      if (words_murrabs_weights_per_line.back().back().back()=='1' && words_murrabs_weights_per_line.size()>1)
//            words_murrabs_weights_per_line.back().back().chop(1);

      display_meters(words_murrabs_weights_per_line);
      display_arkans(words_murrabs_weights_per_line);
      //      display_names(words_murrabs_weights_per_line);
    }

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Time elapsed: " << end.count() << "\n ---------------------------- \n";
}

void MainWindow::on_pushButton_2_clicked()
{
//    qApp->quit();
//    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    ui->textEdit->clear();

}
