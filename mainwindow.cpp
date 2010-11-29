#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

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

void MainWindow::convert()
{
	ui->textBrowser->clear();
	if(ui->inputTextbox->text() != "" && ui->outputTextbox->text() != "")
	{
		QFile input(ui->inputTextbox->text());
		//QFile output(ui->outputTextbox->text());

		input.open(QIODevice::ReadOnly);
		//output.open(QIODevice::WriteOnly);
		QDataStream in(&input);
		in.setByteOrder(QDataStream::LittleEndian);
		QString height = QString::number((float)readInt(input, in, 0x0008)/1000.0f);
		QString weight = QString::number((float)readInt(input, in, 0x000C)/1000.0f);
		QString age = QString::number(readInt(input, in, 0x0010));
		QString sex;
		switch(readInt(input, in, 0x0014))
		{
			case 0:
				sex = "Undefined";
				break;
			case 1:
				sex = "Male";
				break;
			case 2:
				sex = "Female";
				break;
		}
		QString name = readUnicode(input, in, 0x0018, 64);
		QString comment = readUnicode(input, in, 0x0118, 255);
		QString nationality = readUnicode(input, in, 0x0318, 32);
		QString modelNumber = readUnicode(input, in, 0x0360, 32);
		QString username = readUnicode(input, in, 0x03A0, 32);
		QString manufacturer = readUnicode(input, in, 0x03E0, 32);
		ui->textBrowser->append(height);
		ui->textBrowser->append(weight);
		ui->textBrowser->append(age);
		ui->textBrowser->append(sex);
		ui->textBrowser->append(name);
		ui->textBrowser->append(comment);
		ui->textBrowser->append(nationality);
		ui->textBrowser->append(modelNumber);
		ui->textBrowser->append(username);
		ui->textBrowser->append(manufacturer);
	}
}

QString MainWindow::readUnicode(QFile &input, QDataStream &in, int offset, int chars)
{
	QString result;
	qint16 currentByte;
	input.seek(offset);
	for(int i=0; i<chars; ++i)
	{
		in >> currentByte;
		result.append(QChar(currentByte));
	}
	return result;
}

qint32 MainWindow::readInt(QFile &input, QDataStream &in, int offset)
{
	qint32 result;
	input.seek(offset);
	in >> result;
	return result;
}
