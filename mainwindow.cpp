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

    // creating pointers to hold each row data
    QTableWidgetItem* activityItemPtr = new QTableWidgetItem();
    QTableWidgetItem* DescriptionItemPtr = new QTableWidgetItem();
    QCheckBox* doneItemPtr =new QCheckBox();
    // set each pointer to point to a table item
    ui->table->setItem(n_activities-1,0,activityItemPtr);
    ui->table->setItem(n_activities-1,1,DescriptionItemPtr);
    ui->table->setCellWidget(n_activities-1, 2, doneItemPtr);
    doneItemPtr->setText("is Completed ?");
    // convert the pointers to a tuple and add it to the activities vector
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
        QFile file(path + "/activities");
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
        if(!path.endsWith("activities")){
            return;
        }
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

        // parse number of activities from JSON and update variables & table size
        n_activities = activitiesArray[0].toInt();
        emit n_activities_changed(n_activities);
        ui->table->setRowCount(n_activities);
        // clear the m_activities vector and resize it to fit the newly parsed n_activities
        m_activities.clear();
        m_activities.resize(n_activities);

        // a temp JSON Object to hold 1 JSON record from the array at a time, to be able to parse
        QJsonObject tempObj;

        for(int i=0; i< n_activities;i++){
            tempObj = activitiesArray[i+1].toObject();

            // parse & set the text of the Activity column            
            QTableWidgetItem* ActivityItemPtr = new QTableWidgetItem();
            ActivityItemPtr->setText(tempObj["Activity"].toString());
            // let the pointer points to the table item
            ui->table->setItem(i,0,ActivityItemPtr);
            // hold a pointer to the item in m_activities vector
            get<0>(m_activities[i]) = ui->table->item(i,0);

            QTableWidgetItem* DescItemPtr = new QTableWidgetItem();
            DescItemPtr->setText(tempObj["Description"].toString());
            // let the pointer points to the table item
            ui->table->setItem(i,1,DescItemPtr);
            // hold a pointer to the item in m_activities vector
            get<1>(m_activities[i]) = ui->table->item(i,1);

            QCheckBox* CheckboxItemPtr = new QCheckBox();
            CheckboxItemPtr->setText("is Completed ?");
            if (tempObj["Done"] == true){
                CheckboxItemPtr->setChecked(true);
            }else{
                CheckboxItemPtr->setChecked(false);
            }
            ui->table->setCellWidget(i,2,CheckboxItemPtr);
            // hold a pointer to the item in m_activities vector, but cast it to QCheckBox* first
            get<2>(m_activities[i]) = (QCheckBox*)(ui->table->item(i,2));
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
    n_activities = ui->table->rowCount(); // update no. of activities
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
