#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <QVector>
#include <QHash>

namespace Ui {
class MainWindow;
}

enum class ProgramMode { TAQTI, ISLAH };

struct Accumulated_Weight
{
  QString bin;
  QStringList words;
  QStringList weights;
  QVector<bool> rejected;
  int rejected_count = 0;
  bool is_tasbeegh_o_azala = false;
  bool has_meter = false;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_actionExit_triggered();

    void on_taqtiButton_clicked();

    void on_islahButton_clicked();

    void on_actionFont_Size_triggered();

private:
    Ui::MainWindow *ui;
    QShortcut* push_button_shortcut;

    QHash<QString, QStringList> dict_cache;

    QString remove_symbols(const QString& user_entered_word);
    QVector<QStringList> get_user_input();
    QVector<QStringList> get_murrab_weight(const QStringList& user_entered_line);
    QList<QString> get_different_weights_of_word(const QString& word);
    QVector<Accumulated_Weight> get_accumulated_weight(const QVector<QStringList>& words_murrab_weight_per_line);

    ProgramMode mode;

    QString taqti_but_stylesheet;
    QString islah_but_stylesheet;

    QStringList meters_in_bin;

    bool has_different_weights(const QString& word);

    void display_arkans(const QVector<QStringList>& words_murrab_weight_per_line);
    void display_meters(const QVector<QStringList>& words_murrab_weight_per_line);
    void display_names(const QVector<QStringList>& words_murrab_weight_per_line);
};

#endif // MAINWINDOW_H
