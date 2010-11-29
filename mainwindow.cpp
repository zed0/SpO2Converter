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
	if(ui->inputTextbox->text() != "")
	{
		QFile input(ui->inputTextbox->text());

		input.open(QIODevice::ReadOnly);
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
		QString year = QString::number(readInt(input, in, 0x0420));
		QString month = QString::number(readInt(input, in, 0x0424));
		QString day = QString::number(readInt(input, in, 0x0428));
		QString hour = QString::number(readInt(input, in, 0x042C));
		QString minute = QString::number(readInt(input, in, 0x0430));
		QString second = QString::number(readInt(input, in, 0x0434));
		QString duration = QString::number(readInt(input, in, 0x0438));

		QStringList spo2Percentage;

		int offset = 0x043C;
		while(offset < input.size())
		{
			spo2Percentage.append(QString::number(readByte(input, in, offset)));
			offset += 2;
		}

		QStringList pulseRate;
		offset = 0x043D;
		while(offset < input.size())
		{
			pulseRate.append(QString::number(readByte(input, in, offset)));
			offset += 2;
		}

		ui->textBrowser->append("Name: " + name);
		ui->textBrowser->append("Height:" + height + "cm");
		ui->textBrowser->append("Weight: " + weight + "kg");
		ui->textBrowser->append("Age: " + age);
		ui->textBrowser->append("Sex: " + sex);
		ui->textBrowser->append("Comment: " + comment);
		ui->textBrowser->append("Nationality: " + nationality);
		ui->textBrowser->append("Model Number: " +modelNumber);
		ui->textBrowser->append("Username: " + username);
		ui->textBrowser->append("Manufacturer: " + manufacturer);
		ui->textBrowser->append("Date: " + day + "/" + month + "/" + year);
		ui->textBrowser->append("Time: " + hour + ":" + minute + ":" + second);
		ui->textBrowser->append("Number of readings: " + duration);

		ui->tableWidget->setRowCount(duration.toInt());
		QString current;
		int row = 0;
		foreach(current, spo2Percentage)
		{
			QTableWidgetItem *newItem = new QTableWidgetItem(current);
			ui->tableWidget->setItem(row++, 0, newItem);
		}
		row = 0;
		foreach(current, pulseRate)
		{
			QTableWidgetItem *newItem = new QTableWidgetItem(current);
			ui->tableWidget->setItem(row++, 1, newItem);
		}
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

qint8 MainWindow::readByte(QFile &input, QDataStream &in, int offset)
{
	qint8 result;
	input.seek(offset);
	in >> result;
	return result;
}
