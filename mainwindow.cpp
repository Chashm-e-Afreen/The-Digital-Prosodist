#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "meters-def.h"

#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#include <Windows.h>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_pushButton_clicked()
{

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

  bool start_reading_from_beginning = true;

  int total_user_entered_words = user_entered_individual_words.size();


  for (int i = 0; i < total_user_entered_words; i++)
    {
      const QString& word = user_entered_individual_words[i];

      QChar first_letter = word.front();

      int line_to_be_read_from = 0;

      auto LetterToLine_find_iterator =  LetterToLine_map.find(first_letter.unicode());

      if (LetterToLine_find_iterator != LetterToLine_map.end())
        {
          line_to_be_read_from = LetterToLine_find_iterator->second;
          ui->plainTextEdit->insertPlainText("\n");
        }
      else
        {
          ui->plainTextEdit->insertPlainText("\nNot_Found ");
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

          QStringList word_murrab_weight_individual = word_murrab_weight_combined.split(",");

          if (word_murrab_weight_individual[0] == word || word_murrab_weight_individual[1] == word)
            {
              QString weight = word_murrab_weight_individual[2];

              auto arkan_find_iterator = Arkan_map.find(weight.toStdWString());

              if (arkan_find_iterator != Arkan_map.end())
                {
                  std::wstring arkan_value_ws = arkan_find_iterator->second;
                  QString arkan_value = QString::fromStdWString(arkan_value_ws);

                  ui->plainTextEdit->insertPlainText(arkan_value + " ");
                }

              else
                {
                  ui->plainTextEdit->insertPlainText("No Rukan found");
                }

              if (i + 1 < total_user_entered_words)
                {
                  QChar succeeding_word_first_letter = user_entered_individual_words[i + 1].front();

                  auto find_iterator = LetterToLine_map.find(succeeding_word_first_letter.unicode());

                  int letters_line_position = find_iterator->second;
                  if (find_iterator != LetterToLine_map.end() && letters_line_position > line_being_read)
                  {
                    start_reading_from_beginning = false;
                    break;
                  }
                }

              start_reading_from_beginning = true;
              break;
            }
        }
    }

  file_read.close();

}
