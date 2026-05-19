#include "mainwindow.h"
#include <QFileSystemModel>
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->toolBar->setContextMenuPolicy(Qt::PreventContextMenu);

    QString sPath = "/home";
    dirModel = new QFileSystemModel(this);
    dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirModel->setRootPath(sPath);

    ui->treeView->setModel(dirModel);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->setColumnWidth(0, 200);

    QModelIndex index = dirModel->index(sPath, 0); // this line is not in the video.
    ui->treeView->setRootIndex(index);             // this line is not in the video.

    fileModel = new QFileSystemModel(this);
    fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    dirModel->setRootPath(sPath);

    ui->listView->setModel(fileModel);
    index = fileModel->index(sPath, 0); // this line is not in the video.
    ui->listView->setRootIndex(index);  // this line is not in the video.
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_treeView_clicked(const QModelIndex &index) {
    QString path = dirModel->fileInfo(index).absoluteFilePath();
    ui->listView->setRootIndex(fileModel->setRootPath(path));
}
