#include "mainwindow.h"
#include <QSet>
#include "meters-def.h"
bool MainWindow::has_different_weights(const QString& word)
{

    QSet<QString> different_unique_weights = {};

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

//      if (found_ea)
//        {
//          word = word.chopped(2);
//        }

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

          word.chop(2);

          dict_cache_find_iterator = dict_cache.find(word);

          if (dict_cache_find_iterator != dict_cache.end())
            {
              words_murrabs_weights[i] = dict_cache_find_iterator.value();
              words_murrabs_weights[i][0] += u8"ئے";
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
                  word+= u8"وں";
                }

              else if (found_oun) // we were unable to find the word even after chopping the oun and yun, if the word only had oun at its end we would like to add 'he' at its place and then check for word again (here the word will be already chopped so we just need to append 'he')
                {
                  word += u8"ہ";

                  dict_cache_find_iterator = dict_cache.find(word);

                  if (dict_cache_find_iterator != dict_cache.end())
                    {
                      words_murrabs_weights[i] = dict_cache_find_iterator.value();
                      words_murrabs_weights[i][0] = user_entered_line[i];
                      word+=u8"وں";
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

          else if (found_oun_hamzawao && dict_cache_find_iterator==dict_cache.end())
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

      else if (found_bariye && dict_cache_find_iterator==dict_cache.end())
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

      else if (word.back()==L'ا' && last_two_letters != u8"یا" && dict_cache_find_iterator==dict_cache.end())
        {

          word = word.chopped(1);
          dict_cache_find_iterator = dict_cache.find(word);

          if (dict_cache_find_iterator != dict_cache.end())
            {
              words_murrabs_weights[i] = dict_cache_find_iterator.value();
              words_murrabs_weights[i][2].back()= L'1';
              words_murrabs_weights[i][2] += L'0';
              words_murrabs_weights[i][0] = user_entered_line[i];

            }


        }
      else if (word.back()==L'ی' && last_two_letters != u8"گی" && dict_cache_find_iterator==dict_cache.end())
        {

          word = word.chopped(1);
          dict_cache_find_iterator = dict_cache.find(word);

          if (dict_cache_find_iterator != dict_cache.end())
            {
              words_murrabs_weights[i] = dict_cache_find_iterator.value();
              words_murrabs_weights[i][2].back()= L'1';
              words_murrabs_weights[i][2] += L'0';
              words_murrabs_weights[i][0] = user_entered_line[i];

            }

        }
      else if (last_two_letters ==u8"یا" && dict_cache_find_iterator==dict_cache.end())
        {

          word = word.chopped(2);

          dict_cache_find_iterator = dict_cache.find(word);

          if (dict_cache_find_iterator != dict_cache.end())
            {
              words_murrabs_weights[i] = dict_cache_find_iterator.value();
              words_murrabs_weights[i][2] += u8"10";
              words_murrabs_weights[i][0] = user_entered_line[i];
            }

        }
      else if (last_two_letters ==u8"گی" && dict_cache_find_iterator==dict_cache.end())
        {

          word = word.chopped(2);
          word+= u8"ہ";
          dict_cache_find_iterator = dict_cache.find(word);

          if (dict_cache_find_iterator != dict_cache.end())
            {
              words_murrabs_weights[i] = dict_cache_find_iterator.value();
              words_murrabs_weights[i][2].chop(1);
              words_murrabs_weights[i][2].back()= L'1';
              words_murrabs_weights[i][2] += u8"10";
              words_murrabs_weights[i][0] = user_entered_line[i];
            }

        }

      else if (found_wao && dict_cache_find_iterator==dict_cache.end())
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

      else if (found_an && dict_cache_find_iterator==dict_cache.end())
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

          else if (found_gan && dict_cache_find_iterator==dict_cache.end())
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

QList<QString> MainWindow::get_different_weights_of_word(const QString& word)
{
    QSet<QString> different_unique_weights = {};

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

              accumulated_weights[j] += individual_weight;
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
                  accumulated_weights.push_back(weight);

                  new_accumulated_weight_size++;
                }

              else
                {
                  for (int k = 0; k < prev_accumulated_weight_size; k++)
                    {
                      accumulated_weights.push_back(accumulated_weights[k] + weight);

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

      if (i != 0  && (prev_word_last_letter != L'ا' && prev_word_last_letter != L'ہ' && prev_word_last_letter != L'ۂ' &&
                                                    prev_word_last_letter != L'ے' && prev_word_last_letter != L'ؤ' && prev_word_last_letter != L'ں'))
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
              accumulated_weights.pop_front();
              --new_accumulated_weight_size;

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
              accumulated_weights.pop_front();
              --new_accumulated_weight_size;
            }

          prev_accumulated_weight_size = new_accumulated_weight_size;
        }


      if (individual_word.size() > 3 && last_two_letters == u8"ئے" && dict_cache.find(individual_word) == dict_cache.end())
        {
          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {

              accumulated_weights[k] += L'1';

              QString new_accumulated_weight_one = accumulated_weights[k] + L'0';

              if (last_three_letters== u8"وئے")
                {
                  QString new_accumulated_weight_two = accumulated_weights[k];
                  new_accumulated_weight_two.chop(1);
                  new_accumulated_weight_two.back() = L'1';

                  QString new_accumulated_weight_three = new_accumulated_weight_two;
                  new_accumulated_weight_three += L'0';

                  accumulated_weights.push_back(new_accumulated_weight_two);
                  accumulated_weights.push_back(new_accumulated_weight_three);

                  new_accumulated_weight_size += 2;
                }

              accumulated_weights.push_back(new_accumulated_weight_one);

              new_accumulated_weight_size++;
            }
        }

      else if(last_letter == L'ِ' || last_letter == L'ۂ')
        {
          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {
              accumulated_weights[k].back() = '1';

              QString new_accumulated_weight = accumulated_weights[k];

              new_accumulated_weight += '0';

              accumulated_weights.push_back(new_accumulated_weight);

              new_accumulated_weight_size++;
            }
        }

      else if (individual_word.size() > 1 && individual_word != u8"اے" && last_weight != L'1' && (last_letter == L'ا' || last_letter == L'ہ' ||
                                                                                               last_letter == L'ی' || last_letter == L'ے' ||
                                                                                               last_letter == L'و' || last_letter == L'ؤ' )){
          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {
              QString new_accumulated_weight = accumulated_weights[k];
              new_accumulated_weight.chop(1);

              accumulated_weights.push_back(new_accumulated_weight);

              new_accumulated_weight_size++;
            }

        }

      else if (individual_word == u8"و" || ((individual_word.size() > 3 && ((last_two_letters  == u8"یں" && last_three_letters != u8"ئیں")|| last_two_letters == u8"وں")) && dict_cache.find(individual_word) == dict_cache.end()))
          {
          for (int k = 0; k < prev_accumulated_weight_size; k++)
            {
//              if(accumulated_weights[k].back() != L'ا' && accumulated_weights[k].back() != L'و')
//                {
                if(prev_word_last_letter ==L'ی' && individual_word == u8"و")
                {
                    accumulated_weights[k]+= u8"1";
                    QString new_accumulated_weight = accumulated_weights[k] + u8"0";
                    QString new_accumulated_weight2 = accumulated_weights[k];
                    new_accumulated_weight2.chop(1);
                    new_accumulated_weight2+= u8"1";
                    QString new_accumulated_weight3 = new_accumulated_weight2 + u8"0";
                    accumulated_weights.push_back(new_accumulated_weight);
                    accumulated_weights.push_back(new_accumulated_weight2);
                    accumulated_weights.push_back(new_accumulated_weight3);
                    new_accumulated_weight_size += 3;
                }
                else {


                        accumulated_weights[k].back() = '1';

                      QString new_accumulated_weight = accumulated_weights[k] + '0';

                      accumulated_weights.push_back(new_accumulated_weight);

                      new_accumulated_weight_size++;
                }

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
                    accumulated_weights[k] += L'1';

                    QString new_accumulated_weight = accumulated_weights[k] + L'0';

                    accumulated_weights.push_back(new_accumulated_weight);

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
