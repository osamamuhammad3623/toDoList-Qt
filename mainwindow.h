#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QVector>
#include <tuple>
#include <QTableWidgetItem>
#include <QJsonDocument>
#include <QFile>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
using namespace  std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void n_activities_changed(int n);

private slots:
    void on_actionNew_triggered();

    void on_actionSave_activities_triggered();

    void on_actionLoad_Activities_triggered();

    void on_actionRemove_activity_triggered();

    void on_n_activities_changed(int n);

    void on_actionClear_all_activities_triggered();
private:
    Ui::MainWindow *ui;
    int n_activities{0};
    // a vector that holds pointers to each table cell
    QVector<tuple<QTableWidgetItem*,QTableWidgetItem*,QCheckBox*>> m_activities{n_activities};

};
#endif // MAINWINDOW_H
