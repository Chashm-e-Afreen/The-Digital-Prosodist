#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "levenshtein-sse.h"
#include "meters-def.h"



#include <QTextCodec>
#include <QFile>

#include <QMessageBox>
#include <QSet>
#include <cmath>
#include <chrono>
#include <QProcess>
#include "edit_dist.h"

#define TOTAL_DICT_WORDS 99421

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


QString MainWindow::remove_symbols(const QString& user_entered_word)
{
  QString new_word;

  for (int i = 0; i < user_entered_word.size(); i++)
    {

      if ((user_entered_word[i] >= 1613 && user_entered_word[i] <= 1618) && (user_entered_word[i] != 1616 || i != user_entered_word.size() - 1) && (user_entered_word[i] == 1537) && (user_entered_word[i] == 1548) && (user_entered_word[i] == 1567) && (user_entered_word[i] == 1563) && (user_entered_word[i] == 1617))
          continue;

      new_word += user_entered_word[i];
    }

  return new_word;

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



void MainWindow::display_meters(const QVector<QStringList>& words_murrab_weight_per_line)
{
  auto start = std::chrono::high_resolution_clock::now();

  int size = words_murrab_weight_per_line.size();

//  QVector<unsigned long long> meter_vector = {};

//  for(auto&i: Meters_in_Decimal)
//  {
//    meter_vector.push_back(i.first);
//  }
//   std::sort(meter_vector.begin(),meter_vector.end());




  if(size <= 0)
  {
    return;
  }

  QVector<QString> accumulated_weights = get_accumulated_weight(words_murrab_weight_per_line);
    bool tasbeegh_o_azala = false;
    QString add_weight ="";
  bool found_meter = false;

  ui->textEdit->insertPlainText(u8"\nافاعیل: ");
  int index = 0;


  for (int i = 0; i < accumulated_weights.size(); i++)
    {
        tasbeegh_o_azala = false;
      if(!accumulated_weights[i].isEmpty() && accumulated_weights[i].back()=='1')
      {
        accumulated_weights[i].chop(1);
        tasbeegh_o_azala = true;
      }

      index = i;

      auto meters_find_iterator = Meter_map.find(accumulated_weights[i].toStdWString());
      if (meters_find_iterator != Meter_map.end())
        {
           QString meter_value = QString::fromStdWString(meters_find_iterator->second);
           if(accumulated_weights.size()>= 2)
           {
               if(accumulated_weights[i].back() == L'1')
                   tasbeegh_o_azala = true;
           }
           if(tasbeegh_o_azala)
           {
                    meter_value.insert(meter_value.size()-1,L'ا');
                    add_weight = "1";
           }
          ui->textEdit->insertPlainText(meter_value + " " +"(" + accumulated_weights[i]+ add_weight + ")");
          found_meter = true;

          break;
        }
    }



    std::wstring key = L"";
    int closest_meter_index = 0;
  if (!found_meter)
    {

      unsigned int distance = 0;
      int count =0;
      for(int i=0; i<accumulated_weights.size(); ++i)
      {
//			   auto it = std::lower_bound(Ordered_Meters.begin(), Ordered_Meters.end(), accumulated_weights[i]);
//               auto key_address = (it - Ordered_Meters.begin());
//               int value= distanceLevenshtein(Ordered_Meters[key_address], accumulated_weights[i]);
//               if (value<distance || count == 0)
//               {
//                   distance = value;
//                   key = Ordered_Meters[key_address].toStdWString();
//                   ++count;
//                   closest_meter_index = i;


//               }

            for(auto&j: Meter_map)
            {

              // size_t value = levenshteinSSE::levenshtein(j.first,accumulated_weights[i].toStdWString());
               unsigned int value = levenshteinDist(j.first,accumulated_weights[i].toStdWString());
               if( value<distance || count ==0)
               {
                    distance=value;
                    key = j.first;
                    ++count;
                    closest_meter_index = i;

               }
            }
      }
            auto meters_find_iterator = Meter_map.find(key);
            if(meters_find_iterator!=Meter_map.end())
            {

                QString meter_value = QString::fromStdWString(meters_find_iterator->second);
                QString original_weight = accumulated_weights[closest_meter_index];

                ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی مانوس بحر نہیں مل سکی </span>| ");
                ui->textEdit->insertPlainText("\n");
                ui->textEdit->insertHtml(u8"<span style= 'color:#5900b3'> نزدیک ترین بحر کے ارکان : </span>");
                ui->textEdit->insertHtml(u8"<span style= 'color:black'></span>"+ meter_value);
//                ui->textEdit->insertPlainText("\n");
//                ui->textEdit->insertHtml(u8"<span style= 'color:#5900b3'>الفاظ : </span>");
//				ui->textEdit->insertHtml(u8"(");

//			 if(meter_value.size()<original_weight.size())
//			 {
//				for(int i=0; i<original_weight.size(); ++i)
//				{
//						if(i<meter_value.size())
//						{
//							if(original_weight[i] == meter_value[i])
//							{
//								ui->textEdit->insertHtml(u8"<span style= 'color:black'></span>"+ original_weight[i]);
//							}
//							else
//							{
//								 ui->textEdit->insertHtml(u8"<span style= 'color:red'></span>"+ original_weight[i]);
//							}
//						}
//						else
//						{
//							ui->textEdit->insertHtml(u8"<span style= 'color:red'></span>"+ original_weight[i]);
//						}
//				}
//			 }
//             else
//             {
//			   for(int i=0; i<original_weight.size(); ++i)
//			   {
//				 if(original_weight[i] == meter_value[i])
//				 {
//					 ui->textEdit->insertHtml(u8"<span style= 'color:red'></span>" + original_weight[i]);
//				 }
//				 else
//				 {
//					  ui->textEdit->insertHtml(u8"<span style= 'color:red'> </span>" + original_weight[i]);
//				 }

//			   }
//				ui->textEdit->insertPlainText(")");

//            }
          }
            else
                 ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی مانوس بحر نہیں مل سکی </span>|");

    }



  auto meters_find_iterator = Names_map.find(accumulated_weights[index].toStdWString());

  if (meters_find_iterator != Names_map.end())
    {
      ui->textEdit->insertPlainText(u8"\nبحر: ");
       QString name_value = QString::fromStdWString(meters_find_iterator->second);
       QString additional_zuhaf = "";
        if(tasbeegh_o_azala)
        {
            QString rukn = accumulated_weights[index].mid(accumulated_weights[index].size()-4,4);
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
      auto it = Names_map.find(key);
      if(it!= Names_map.end())
      {
            QString name_value = QString::fromStdWString(it->second);
          //  ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی مانوس بحر نہیں مل سکی </span>|");
            ui->textEdit->insertPlainText("\n");
            ui->textEdit->insertHtml(u8"<span style='color:#5900b3'>نزدیک ترین بحر کا نام :   </span>");
            ui->textEdit->insertHtml(u8"<span style= 'color:black'></span>"+ name_value);
      }
//      else {
//          ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی بحر نہیں مل سکی </span>|");
//      }

    }

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Displaying Names: " << end.count() << "\n";
}




void MainWindow::on_pushButton_clicked()
{

  auto start = std::chrono::high_resolution_clock::now();


  QVector<QStringList> user_entered_lines = get_user_input();

  QVector<QStringList> words_murrabs_weights_per_line = {};
  ui->textEdit->clear();
  for (auto& line: user_entered_lines)
    {

       for(auto& i:line)
       {

      ui->textEdit->insertPlainText(i);
      ui->textEdit->insertPlainText(" ");
        }
      words_murrabs_weights_per_line = get_murrab_weight(line);
      display_meters(words_murrabs_weights_per_line);
      display_arkans(words_murrabs_weights_per_line);
      if(user_entered_lines.size()>1 && line!=user_entered_lines.back())
      {
        ui->textEdit->insertPlainText("\n\n");
      }

    }

  std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

  QTextStream(stdout) << "Time elapsed: " << end.count() << "\n ---------------------------- \n";
}


void MainWindow::on_pushButton_2_clicked()
{

  ui->textEdit->clear();

}
