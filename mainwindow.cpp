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

  QString user_entered_combined_words = ui->plainTextEdit->toPlainText();
  QStringList user_entered_individual_words = user_entered_combined_words.split(QRegExp("\\s+"), QString::SkipEmptyParts);

  QString dict_file_path  = "C:/Users/Shayan Ali Abbasi/Documents/GitHub/murgh-e-chaman/data/words_murrab_weight_unique.txt";

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

  int total_user_entered_words = user_entered_individual_words.size();

  for (int i = 0; i < total_user_entered_words; i++)
    {
      const QString& word = user_entered_individual_words[i];

      if (i == 0)
        {
          ui->plainTextEdit->insertPlainText("\n");
        }

      if (rejected_cache.find(word.toStdWString()) != rejected_cache.end())
        {
          ui->plainTextEdit->insertPlainText("WordNotFound ");
          continue;
        }

      QChar first_letter = word.front();

      int line_to_be_read_from = 0;

      word_found = false;

      auto LetterToLine_find_iterator =  LetterToLine_map.find(first_letter.unicode());

      if (LetterToLine_find_iterator != LetterToLine_map.end())
        {
          line_to_be_read_from = LetterToLine_find_iterator->second;
        }
      else
        {
          rejected_cache.insert(word.toStdWString());
          ui->plainTextEdit->insertPlainText("WordNotFound ");
          continue;
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

              QString weight = word_murrab_weight_individual[2];

              found_cache.insert({word, word_murrab_weight_individual});

              auto arkan_find_iterator = Arkan_map.find(weight.toStdWString());

              if (arkan_find_iterator != Arkan_map.end())
                {
                  std::wstring arkan_value_ws = arkan_find_iterator->second;
                  QString arkan_value = QString::fromStdWString(arkan_value_ws);

                  ui->plainTextEdit->insertPlainText(arkan_value + " ");
                }

              else
                {
                  ui->plainTextEdit->insertPlainText("NoRukanFound ");
                }

              if (i + 1 < total_user_entered_words)
                {
                  QChar succeeding_word_first_letter = user_entered_individual_words[i + 1].front();

                  auto find_iterator = LetterToLine_map.find(succeeding_word_first_letter.unicode());

                  if (find_iterator != LetterToLine_map.end() && find_iterator->second > line_being_read)
                  {
                    start_reading_from_beginning = false;
                    break;
                  }
                }

              start_reading_from_beginning = true;
              break;
            }
        }

          if (!word_found)
            {
              rejected_cache.insert(word.toStdWString());
              start_reading_from_beginning = true;
              ui->plainTextEdit->insertPlainText("WordNotFound ");
            }
    }

  file_read.close();

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Time elapsed: " << end.count() << "\n";
}
