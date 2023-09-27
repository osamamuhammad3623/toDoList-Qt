#include "mainwindow.h"
#include "ui_mainwindow.h"
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, this->n_activities_changed, this, this->on_n_activities_changed);
    emit n_activities_changed(n_activities);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    // increase no. of activities & no. of rows in the table
    n_activities++;
    ui->table->setRowCount(n_activities);
    emit n_activities_changed(n_activities);

    QTableWidgetItem* activityItemPtr = new QTableWidgetItem();
    ui->table->setItem(n_activities-1,0,activityItemPtr);

    QTableWidgetItem* DescriptionItemPtr = new QTableWidgetItem();
    ui->table->setItem(n_activities-1,1,DescriptionItemPtr);

    // create a checkbox and add it to the Done column
    QCheckBox* doneItemPtr =new QCheckBox();
    doneItemPtr->setText("is Completed ?");
    ui->table->setCellWidget(n_activities-1, 2, doneItemPtr);

    m_activities.push_back(make_tuple(activityItemPtr,DescriptionItemPtr,doneItemPtr));
}


void MainWindow::on_actionSave_activities_triggered()
{
    // create a JSON array to store the activities
    QJsonArray activitiesArray{n_activities};
    for(int i =0; i< n_activities;i++){
        // create a JSON object and store activity data into it
        QJsonObject obj;
        obj["Activity"] = get<0>(m_activities[i])->text();
        obj["Description"] = get<1>(m_activities[i])->text();
        obj["Done"] = get<2>(m_activities[i])->isChecked();
        // append the JSON object to the array
        activitiesArray.append(obj);
    }

    // create a document of the the JSON array
    QJsonDocument activitiesDocument(activitiesArray);

    // let the user choose where to store the JSON document
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::Directory);
    QString path{};
    if(fileDialog.exec()){
        path = fileDialog.selectedFiles()[0];
        // create a file named activites to write the JSON document into it
        QFile file(path + "/activites");
        file.open(QIODevice::WriteOnly);
        QTextStream outStream(&file);
        outStream << activitiesDocument.toJson();
        file.close();
    }
}

void MainWindow::on_actionLoad_Activities_triggered()
{
    // let the user choose the JSON document
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::AnyFile);
    QString path{};
    if(fileDialog.exec()){
        path = fileDialog.selectedFiles()[0];

        QFile file(path);
        file.open(QIODevice::ReadOnly);
        QString line;
        QString content{}; // content of the JSON file
        QTextStream inStream(&file);
        while(inStream.readLineInto(&line)){
            content += line;
        }

        // convert from JSON file to a JSON Document object
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(content.toStdString()));
        // parse from the JSON Document into a JSON array
        QJsonArray activitiesArray = doc.array();

        n_activities = activitiesArray[0].toInt();
        ui->table->setRowCount(n_activities);
        m_activities.resize(n_activities);

        /* Need urgent update here */

        // a temp JSON Object to hold 1 JSON record from the array at a time, to be able to parse
        QJsonObject tempObj;
        QTableWidgetItem* tempItem;
        QCheckBox* tempCheckItem;

        for(int i=0; i< n_activities;i++){
            tempObj = activitiesArray[i+1].toObject();

            // parse & set the text of the Activity column
//            get<0>(m_activities[i]) = ui->table->item(i,0);
//            get<0>(m_activities[i])->setText(tempObj["Activity"].toString());
            tempItem = new QTableWidgetItem();
            tempItem->setText(tempObj["Activity"].toString());
            ui->table->setItem(i,0,tempItem);
            // parse & set the text of the Description column
//           get<1>(m_activities[i])->setText(tempObj["Description"].toString());
            tempItem = new QTableWidgetItem();
            tempItem->setText(tempObj["Description"].toString());
            ui->table->setItem(i,1,tempItem);
            // parse & set the state of the Done column
//            get<2>(m_activities[i])->setChecked(tempObj["Done"].toBool());
            tempCheckItem = new QCheckBox();
            tempCheckItem->setText("is Completed ?");
            if (tempObj["Done"] == true){
                tempCheckItem->setChecked(true);
            }else{
                tempCheckItem->setChecked(false);
            }
            ui->table->setCellWidget(i,2,tempCheckItem);
        }

        file.close();
    }
}

void MainWindow::on_actionRemove_activity_triggered()
{
    // remove/hide the row with the active cell
    int selectedRow = ui->table->currentRow();
    ui->table->removeRow(selectedRow);
    if ((m_activities.size() > 0) && (selectedRow >= 0)){
        m_activities.removeAt(selectedRow);
    }
    n_activities = ui->table->rowCount(); // decrease no. of activities
    emit n_activities_changed(n_activities);
}

void MainWindow::on_n_activities_changed(int n)
{
    if (n>= 0){
        ui->activitiesLabel->setText("All activities: " + QString::fromStdString(to_string(n)));
    }
}

void MainWindow::on_actionClear_all_activities_triggered()
{
    m_activities.clear();
    ui->table->clearContents();
    ui->table->setRowCount(0);
    n_activities = 0;
    emit n_activities_changed(n_activities);
}
