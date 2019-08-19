#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <QTextStream>
#include <QFile>
#include <QTextStream>
#include <QVector>

#include <map>
#include <unordered_set>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QShortcut* push_button_shortcut;

    std::map<QString, QStringList> found_cache;
    std::unordered_set<std::wstring> rejected_cache;

    QFile file_read;
    QTextStream text_stream;
    QString dict_file_path;


    QVector<QStringList> get_user_input();
    QVector<QStringList> get_murrab_weight(const QStringList& user_entered_line);

    void display_arkans(const QVector<QStringList>& words_murrab_weight_per_line);
    void display_meters(const QVector<QStringList>& words_murrab_weight_per_line);
    void display_names(const QVector<QStringList>& words_murrab_weight_per_line);
};

#endif // MAINWINDOW_H
