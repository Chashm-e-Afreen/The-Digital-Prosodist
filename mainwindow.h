#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <QVector>
#include <QHash>
#include <QSet>

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

    QHash<QString, QStringList> dict_cache;
    QHash<QString, QStringList> found_cache;
    QSet<QString> rejected_cache;

    QVector<QStringList> get_user_input();
    QVector<QStringList> get_murrab_weight(const QStringList& user_entered_line);

    QVector<QString> get_accumulated_weight(const QVector<QStringList>& words_murrab_weight_per_line);

    void display_arkans(const QVector<QStringList>& words_murrab_weight_per_line);
    void display_meters(const QVector<QStringList>& words_murrab_weight_per_line);
    void display_names(const QVector<QStringList>& words_murrab_weight_per_line);
};

#endif // MAINWINDOW_H
