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

  QString dict_file_path  = "C:/Users/Muhammad Rehan/Documents/GitHub/murgh-e-chaman/words_murrab_weight_unique.txt";

  QFile file_read(dict_file_path);
  int count = 1;
  if (!file_read.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QMessageBox::warning(this, "Error", "Unable to open dictionary to fetch words");
      return;
    }

  QTextStream text_stream(&file_read);

  int line_being_read = 0;

  foreach(QString word, user_entered_individual_words)
    {
      QChar first_letter = word.front();

      int line_to_be_read_from = 0;

      auto LetterToLine_find_iterator =  LetterToLine_map.find(first_letter.unicode());

      if (LetterToLine_find_iterator != LetterToLine_map.end())
        {
          line_to_be_read_from = LetterToLine_find_iterator->second;
          //ui->plainTextEdit->appendPlainText("");
        }
      else
        {
          ui->plainTextEdit->insertPlainText("Not_Found ");
          continue;
        }

      text_stream.seek(0);

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

          if (word_murrab_weight_individual[0] == word)
            {
              QString weight = word_murrab_weight_individual[2];

              auto arkan_find_iterator = Arkan_map.find(weight.toStdWString());

                if (count==1)
                {
                     ui->plainTextEdit->insertPlainText("\n");
                }
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
                ++count;
              break;
            }
        }
    }

  file_read.close();

}
