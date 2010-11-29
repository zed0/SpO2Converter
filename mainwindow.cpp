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
	if(ui->inputTextbox->text() != "" && ui->outputTextbox->text() != "")
	{
		QFile input(ui->inputTextbox->text());
		//QFile output(ui->outputTextbox->text());

		input.open(QIODevice::ReadOnly);
		//output.open(QIODevice::WriteOnly);
		QDataStream in(&input);
		in.setByteOrder(QDataStream::LittleEndian);
		QString name = readUnicode(input, in, 0x0018, 64);
		QString comment = readUnicode(input, in, 0x0118, 255);
		QString nationality = readUnicode(input, in, 0x0318, 32);
		QString modelNumber = readUnicode(input, in, 0x0360, 32);
		QString username = readUnicode(input, in, 0x03A0, 32);
		QString manufacturer = readUnicode(input, in, 0x03E0, 32);
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
