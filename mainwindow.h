#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
};

#endif // MAINWINDOW_H
