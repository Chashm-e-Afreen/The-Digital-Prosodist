#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "meters-def.h"

#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <chrono>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  push_button_shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);

  connect(this->push_button_shortcut, SIGNAL(activated()), this, SLOT(on_pushButton_clicked()));

  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

  dict_file_path  = "data/words_murrab_weight_unique.txt";

  file_read.setFileName(dict_file_path);

  if (!file_read.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QMessageBox::warning(this, "Error", "Unable to open dictionary to fetch words");
      return;
    }

  text_stream.setDevice(&file_read);

  std::map<QString, QString> my_map;

  while(text_stream.atEnd())
    {

      QStringList line = text_stream.readLine().split(',');

      my_map.insert({line[0], line[1]});
    }

}

MainWindow::~MainWindow()
{
  file_read.close();
  delete ui;
}

QVector<QStringList> MainWindow::get_user_input()
{
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

  return user_input;
}

QVector<QStringList> MainWindow::get_murrab_weight(const QStringList& user_entered_line)
{

  QVector<QStringList> words_murrabs_weights;

  words_murrabs_weights.resize(user_entered_line.size());

  int line_being_read = 0;

  bool word_found = false;
  bool start_reading_from_beginning = true;

  int total_user_entered_words = user_entered_line.size();

  for (int i = 0; i < total_user_entered_words; i++) // Start checking every word entered by the user in current line
    {
      const QString& word = user_entered_line[i];

      QChar first_letter = word.front(); // Checking the first letter of current word

      int line_to_be_read_from = 0; // Stores from where we would like text stream to actually read line

      word_found = false; // Stores whether we have found the word in our dictionary

      auto LetterToLine_find_iterator =  LetterToLine_map.find(first_letter.unicode()); // Find first character of user entered word in our letter map and its starting position in dictionary

      if (LetterToLine_find_iterator != LetterToLine_map.end()) // We found the letter in LetterToLine_map
        {
          line_to_be_read_from = LetterToLine_find_iterator->second;
        }
      else
        {
          rejected_cache.insert(word.toStdWString());
          continue;
        }

      if (rejected_cache.find(word.toStdWString()) != rejected_cache.end())
        {
          continue;
        }
      else
        {
          auto found_cache_iterator = found_cache.find(word);
          if (found_cache_iterator != found_cache.end())
            {
              words_murrabs_weights[i] = found_cache_iterator->second;
              continue;
            }
        }

      if (start_reading_from_beginning)
        {
          line_being_read = 0;
          text_stream.seek(0);
        }


      while(!text_stream.atEnd())
        {

          if (++line_being_read < line_to_be_read_from)
            {
              text_stream.readLine();
              continue;
            }

          QString word_murrab_weight_combined;

          word_murrab_weight_combined = text_stream.readLine();

          if (word_murrab_weight_combined.isEmpty())
            {
              continue;
            }

          QStringList word_murrab_weight_individual = word_murrab_weight_combined.split(",", QString::SkipEmptyParts);

          if (word_murrab_weight_individual.size() != 3) continue;

          if (word_murrab_weight_individual[0] == word || word_murrab_weight_individual[1] == word)
            {
              word_found = true;

              words_murrabs_weights[i] = word_murrab_weight_individual;

              found_cache.insert({word, word_murrab_weight_individual});

              break;
            }
          else if (word_murrab_weight_individual[0].front() != first_letter) // If the first letter of user entered word no longer matches that read from file we stop reading from file
            {
              break;
            }
        }

      if (word_found) // We found the word in our dictionary
        {
          if (i + 1 < total_user_entered_words)
            {
              QChar succeeding_word_first_letter = user_entered_line[i + 1].front();

              auto find_iterator = LetterToLine_map.find(succeeding_word_first_letter.unicode());

              if (find_iterator != LetterToLine_map.end() && find_iterator->second > line_being_read)
                {
                  start_reading_from_beginning = false;
                  continue;
                }
            }

          start_reading_from_beginning = true;
        }

      else // We didn't find the word entered by user in our dictionary
        {
          rejected_cache.insert(word.toStdWString());

          start_reading_from_beginning = true;
        }
    }

  return words_murrabs_weights;
}

void MainWindow::display_arkans(const QVector<QStringList>& words_murrab_weight_per_line)
{
  int size = words_murrab_weight_per_line.size();

  if (size <= 0)
       return;

  ui->textEdit->insertPlainText(u8"\nتحلیلِ الفاظ: ");

  for (int i = 0; i < size; i++)
    {
      if (words_murrab_weight_per_line[i].size() != 3)
        {
          ui->textEdit->insertHtml("<span style='color:red'> X </span>");
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
           ui->textEdit->insertHtml("<span style='color:red'> X </span>");
        }

    }

}

void MainWindow::display_meters(const QVector<QStringList>& words_murrab_weight_per_line)
{
  int size = words_murrab_weight_per_line.size();

  if(size <= 0)
    return;

  ui->textEdit->insertPlainText(u8"\nافاعیل: ");

  QString accumulated_weight;

  for (int i = 0; i < size; i++)
    {
      if (words_murrab_weight_per_line[i].size() != 3) continue;

      accumulated_weight += words_murrab_weight_per_line[i][2];
    }

    auto meters_find_iterator = Meter_map.find(accumulated_weight.toStdWString());

    if (meters_find_iterator != Meter_map.end())
      {
        const QString meter_value = QString::fromStdWString(meters_find_iterator->second);

        ui->textEdit->insertPlainText(meter_value + " (" + accumulated_weight + ")");
      }
    else
      {
       ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی مانوس بحر نہیں مل سکی </span> (" + accumulated_weight + ")");
      }
}

void MainWindow::display_names(const QVector<QStringList>& words_murrab_weight_per_line)
{
  int size = words_murrab_weight_per_line.size();

  if(size <= 0)
    return;

  ui->textEdit->insertPlainText(u8"\nبحر: ");

  QString accumulated_weight;

  for (int i = 0; i < size; i++)
    {
      if (words_murrab_weight_per_line[i].size() != 3) continue;

      accumulated_weight += words_murrab_weight_per_line[i][2];
    }

    auto meters_find_iterator = Names_map.find(accumulated_weight.toStdWString());

    if (meters_find_iterator != Names_map.end())
      {
        const QString name_value = QString::fromStdWString(meters_find_iterator->second);

        ui->textEdit->insertPlainText(name_value);
      }    else
      {
       ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی بحر نہیں مل سکی </span> (" + accumulated_weight + ")");
      }
}

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
      display_names(words_murrabs_weights_per_line);
    }

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Time elapsed: " << end.count() << "\n";
}
