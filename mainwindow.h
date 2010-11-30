#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsTextItem>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public slots:
	void browseInput();
	void browseOutput();
	void convert();

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	QGraphicsScene graph;
	QGraphicsTextItem spText;
	QGraphicsTextItem plsText;

private:
	Ui::MainWindow *ui;
	QString readUnicode(QFile &input, QDataStream &in, int offset, int chars);
	qint32 readInt(QFile &input, QDataStream &in, int offset);
	qint8 readByte(QFile &input, QDataStream &in, int offset);
};

#endif // MAINWINDOW_H
