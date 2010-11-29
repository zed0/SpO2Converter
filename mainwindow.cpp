#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::browseInput()
{
	QString s = QFileDialog::getOpenFileName(this, "Input file", "./", "*.SpO2");
	ui->inputTextbox->setText(s);
}

void MainWindow::browseOutput()
{
	QString s = QFileDialog::getOpenFileName(this, "Output file", "./");
	ui->outputTextbox->setText(s);
}
