#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "meters-def.h"

#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QShortcut>
#include <chrono>

#define FILE_LINE_SIZE 53

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  push_button_shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);

  connect(this->push_button_shortcut, SIGNAL(activated()), this, SLOT(on_pushButton_clicked()));

  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_pushButton_clicked()
{

  auto start = std::chrono::high_resolution_clock::now();

  QString user_entered_combined_words = ui->textEdit->toPlainText();
  QString dict_file_path  = "words_murrab_weight_unique.txt";

  QStringList user_entered_individual_words = user_entered_combined_words.split(QRegExp("\\s+"), QString::SkipEmptyParts);
  QStringList required_word_murrab_weight;

  QFile file_read(dict_file_path);

  if (!file_read.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QMessageBox::warning(this, "Error", "Unable to open dictionary to fetch words");
      return;
    }

  QTextStream text_stream(&file_read);

  int line_being_read = 0;

  bool word_found = false;
  bool start_reading_from_beginning = true;
  bool word_found_in_cache = false;

  int total_user_entered_words = user_entered_individual_words.size();

  for (int i = 0; i < total_user_entered_words; i++) // Start checking every word entered by the user
    {
      const QString& word = user_entered_individual_words[i];

      if (i == 0) // If we are currently at first word add a newline
        {
          ui->textEdit->insertPlainText("\n");
        }

      QChar first_letter = word.front(); // Checking the first letter of current word

      int line_to_be_read_from = 0; // Stores from where we would like text stream to actually read line

      word_found = false; // Stores whether we have found the word in our dictionary
      word_found_in_cache = false; // Stores whether we have found the word in our found_cache

      auto LetterToLine_find_iterator =  LetterToLine_map.find(first_letter.unicode()); // Find first character of use entered word in our letter map and its starting position in dictionary

      if (LetterToLine_find_iterator != LetterToLine_map.end()) // We found the letter in
        {
          line_to_be_read_from = LetterToLine_find_iterator->second;
        }
      else
        {
          rejected_cache.insert(word.toStdWString());
          ui->textEdit->insertPlainText("WordNotFound ");
          continue;
        }

      if (rejected_cache.find(word.toStdWString()) != rejected_cache.end())
        {
          ui->textEdit->insertPlainText("WordNotFound ");
          continue;
        }
      else
        {
          auto found_cache_iterator = found_cache.find(word);
          if (found_cache_iterator != found_cache.end())
            {
              word_found_in_cache = true;
              required_word_murrab_weight = found_cache_iterator->second;
            }
        }

      if (start_reading_from_beginning)
        {
          line_being_read = 0;
          text_stream.seek(0);
        }

      if (!word_found_in_cache)
        {

          while(!text_stream.atEnd())
            {

              if (++line_being_read < line_to_be_read_from)
                {
                  text_stream.readLine();
                  continue;
                }

              QString word_murrab_weight_combined;

              text_stream >> word_murrab_weight_combined;

              if (word_murrab_weight_combined.isEmpty())
                {
                  continue;
                }

              QStringList word_murrab_weight_individual = word_murrab_weight_combined.split(",", QString::SkipEmptyParts);

              if (word_murrab_weight_individual.size() != 3) continue;

              if (word_murrab_weight_individual[0] == word || word_murrab_weight_individual[1] == word)
                {
                  word_found = true;
                  required_word_murrab_weight = word_murrab_weight_individual;
                  found_cache.insert({word, required_word_murrab_weight});

                  break;
                }
              else if (word_murrab_weight_individual[0].front() != first_letter) // If the first letter of user entered word no longer matches that read from file we stop reading from file
                {
                  break;
                }
            }
        }

      if (word_found || word_found_in_cache) // We found the word in our dictionary or cache
        {
          QString weight = required_word_murrab_weight[2];

          auto arkan_find_iterator = Arkan_map.find(weight.toStdWString());

          if (arkan_find_iterator != Arkan_map.end())
            {
              std::wstring arkan_value_ws = arkan_find_iterator->second;
              QString arkan_value = QString::fromStdWString(arkan_value_ws);

              ui->textEdit->insertPlainText(arkan_value + " ");
            }

          else
            {
              ui->textEdit->insertPlainText("NoRukanFound ");
            }

          if (i + 1 < total_user_entered_words)
            {
              QChar succeeding_word_first_letter = user_entered_individual_words[i + 1].front();

              auto find_iterator = LetterToLine_map.find(succeeding_word_first_letter.unicode());

              if (find_iterator != LetterToLine_map.end() && find_iterator->second > line_being_read)
                {
                  start_reading_from_beginning = false;
                  continue;
                }
            }

          start_reading_from_beginning = true;
        }

      else // We didn't find the word entered by user in our dictionary and cache
        {
          rejected_cache.insert(word.toStdWString());

          ui->textEdit->insertPlainText("WordNotFound ");

          start_reading_from_beginning = true;
        }
    }

  file_read.close();

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Time elapsed: " << end.count() << "\n";
}
