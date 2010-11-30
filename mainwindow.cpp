#include "mainwindow.h"
#include "ui_mainwindow.h"

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
		ui->statusBar->showMessage("Converting...");
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
		qint32 year = readInt(input, in, 0x0420);
		qint32 month = readInt(input, in, 0x0424);
		qint32 day = readInt(input, in, 0x0428);
		qint32 hour = readInt(input, in, 0x042C);
		qint32 minute = readInt(input, in, 0x0430);
		qint32 second = readInt(input, in, 0x0434);
		qint32 duration = readInt(input, in, 0x0438);

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
		ui->textBrowser->append("Date: " + QString::number(day) + "/" + QString::number(month) + "/" + QString::number(year));
		ui->textBrowser->append("Time: " + QString::number(hour) + ":" + QString::number(minute) + ":" + QString::number(second));
		ui->textBrowser->append("Number of readings: " + QString::number(duration));

		QString durationHours = QString::number(duration/3600);
		QString durationMinutes = QString::number((duration/60)%60);
		QString durationSeconds = QString::number(duration%60);
ui->textBrowser->append("Duration of readings: " + durationHours + ":" + durationMinutes.rightJustified(2,'0') + ":" + durationSeconds.rightJustified(2,'0'));

		ui->tableWidget->clear();
		ui->tableWidget->setRowCount(duration);
		QString current;
		int row = 0;
		int initialSeconds = hour*3600 + minute*60 + second;
		while(row < duration)
		{
			QString sHour = QString::number(((initialSeconds+row)/3600)%24);
			QString sMinute = QString::number(((initialSeconds+row)/60)%60);
			QString sSecond = QString::number((initialSeconds+row)%60);
			QTableWidgetItem *newItem = new QTableWidgetItem(sHour.rightJustified(2,'0') + ":" + sMinute.rightJustified(2,'0') + ":" + sSecond.rightJustified(2,'0'));
			ui->tableWidget->setItem(row++, 0, newItem);
		}
		row = 0;
		foreach(current, spo2Percentage)
		{
			QTableWidgetItem *newItem = new QTableWidgetItem(current);
			ui->tableWidget->setItem(row++, 1, newItem);
		}
		row = 0;
		foreach(current, pulseRate)
		{
			QTableWidgetItem *newItem = new QTableWidgetItem(current);
			ui->tableWidget->setItem(row++, 2, newItem);
		}

		graph.clear();

		spText.setPlainText("SpO2%");
		spText.setDefaultTextColor(QColor(Qt::red));
		spText.setPos(-40, -spo2Percentage.at(0).toInt());
		graph.addItem(&spText);
		for(int i=0; i<spo2Percentage.size()-1; ++i)
		{
			graph.addLine(i, -spo2Percentage.at(i).toInt(), i+1, -spo2Percentage.at(i+1).toInt(), QPen(QColor(Qt::red)));
		}

		plsText.setPlainText("Pulse Rate");
		plsText.setDefaultTextColor(QColor(Qt::blue));
		plsText.setPos(-60, -pulseRate.at(0).toInt());
		graph.addItem(&plsText);
		for(int i=0; i<pulseRate.size()-1; ++i)
		{
			graph.addLine(i, -pulseRate.at(i).toInt(), i+1, -pulseRate.at(i+1).toInt(), QPen(QColor(Qt::blue)));
		}
		ui->graphicsView->setScene(&graph);
		ui->graphicsView->show();

		ui->statusBar->clearMessage();
	}
}

QString MainWindow::readUnicode(QFile &input, QDataStream &in, int offset, int chars)
{
	QString result = "";
	qint16 currentByte;
	input.seek(offset);
	for(int i=0; i<chars; ++i)
	{
		in >> currentByte;
		if(currentByte)
		{
			result.append(QChar(currentByte));
		}
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
