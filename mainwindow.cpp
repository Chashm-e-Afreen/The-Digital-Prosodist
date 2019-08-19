#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "meters-def.h"

#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <chrono>

#define TOTAL_DICT_WORDS 84474

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  push_button_shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);

  connect(this->push_button_shortcut, SIGNAL(activated()), this, SLOT(on_pushButton_clicked()));

  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));


  QString dict_file_path = "C:/Users/Muhammad Rehan/Documents/GitHub/murgh-e-chaman/data/words_murrab_weight_unique.txt";

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
      const QString& word = user_entered_line[i];

      QChar first_letter = word.front(); // Checking the first letter of current word

      auto LetterToLine_find_iterator =  LetterToLine_map.find(first_letter.unicode()); // Find first character of user entered word in our letter map and its starting position in dictionary
      if (LetterToLine_find_iterator == LetterToLine_map.end()) // We did't find the letter in LetterToLine_map
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
          continue;
        }

      auto dict_cache_find_iterator = dict_cache.find(user_entered_line[i]);

      if (dict_cache_find_iterator != dict_cache.end())
        {
          words_murrabs_weights[i] = dict_cache_find_iterator.value();
          found_cache.insert(word, dict_cache_find_iterator.value());
        }
      else
        {
          rejected_cache.insert(word);
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

          ui->textEdit->insertPlainText(rukan + " ");
        }
      else
        {
          ui->textEdit->insertHtml(u8"<span style='color:red'>'X' </span>");
        }

    }

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Displaying Arkans: " << end.count() << "\n";
}

void MainWindow::display_meters(const QVector<QStringList>& words_murrab_weight_per_line)
{
  auto start = std::chrono::high_resolution_clock::now();

  int size = words_murrab_weight_per_line.size();

  if(size <= 0)
    return;

  QVector<QString> accumulated_weights(1); // There will always be one accumulated weight

  int new_accumulated_weight_size = accumulated_weights.size();
  int prev_accumulated_weight_size = new_accumulated_weight_size;

  bool found_meter = false;

  for (int i = 0; i < size; i++) // Iterating every word
    {
      if (words_murrab_weight_per_line[i].size() != 3) continue;

      const QString& individual_word = words_murrab_weight_per_line[i][0];
      const QString& individual_weight = words_murrab_weight_per_line[i][2];

      QChar last_letter = individual_word.back();
      QChar last_weight = individual_weight.back();

      for (int j = 0; j < accumulated_weights.size(); j++)
        {
          accumulated_weights[j] += individual_weight;
        }

      if (individual_word.size() > 1 && last_weight != L'1' && (last_letter == L'ا' || last_letter == L'ہ' ||
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

      prev_accumulated_weight_size = new_accumulated_weight_size;
    }

  ui->textEdit->insertPlainText(u8"\nافاعیل: ");
    int index = 0;
  for (int i = 0; i < accumulated_weights.size(); i++)
    {

      auto meters_find_iterator = Meter_map.find(accumulated_weights[i].toStdWString());
        index = i;
      if (meters_find_iterator != Meter_map.end())
        {
          const QString meter_value = QString::fromStdWString(meters_find_iterator->second);

          ui->textEdit->insertPlainText(meter_value + " (" + accumulated_weights[i] + ")");

          found_meter = true;

          break;
        }

    }

  if (!found_meter)
    {
      ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی مانوس بحر نہیں مل سکی </span> (" ")");
    }



    ui->textEdit->insertPlainText(u8"\nبحر: ");
  auto meters_find_iterator = Names_map.find(accumulated_weights[index].toStdWString());

  if (meters_find_iterator != Names_map.end())
    {
      const QString name_value = QString::fromStdWString(meters_find_iterator->second);

      ui->textEdit->insertPlainText(name_value);
    }    else
    {
      ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی بحر نہیں مل سکی </span>");
    }

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Displaying Names: " << end.count() << "\n";
}

//void MainWindow::display_names(const QVector<QStringList>& words_murrab_weight_per_line)
//{ auto start = std::chrono::high_resolution_clock::now();
//  int size = words_murrab_weight_per_line.size();

//  if(size <= 0)
//    return;

//  ui->textEdit->insertPlainText(u8"\nبحر: ");

//  QString accumulated_weight;

//  for (int i = 0; i < size; i++)
//    {
//      if (words_murrab_weight_per_line[i].size() != 3) continue;

//      accumulated_weight += words_murrab_weight_per_line[i][2];
//    }

//  auto meters_find_iterator = Names_map.find(accumulated_weight.toStdWString());

//  if (meters_find_iterator != Names_map.end())
//    {
//      const QString name_value = QString::fromStdWString(meters_find_iterator->second);

//      ui->textEdit->insertPlainText(name_value);
//    }    else
//    {
//      ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی بحر نہیں مل سکی </span>");
//    }

//  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

//  QTextStream(stdout) << "Displaying Names: " << end.count() << "\n";
//}

void MainWindow::on_pushButton_clicked()
{

  auto start = std::chrono::high_resolution_clock::now();

  QVector<QStringList> user_entered_lines = get_user_input();

  QVector<QStringList> words_murrabs_weights_per_line;

  foreach (const QStringList line, user_entered_lines)
    {
      words_murrabs_weights_per_line = get_murrab_weight(line);

      display_meters(words_murrabs_weights_per_line);
      display_arkans(words_murrabs_weights_per_line);
//      display_names(words_murrabs_weights_per_line);
    }

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Time elapsed: " << end.count() << "\n ---------------------------- \n";
}
