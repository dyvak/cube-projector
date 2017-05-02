#include <QDebug>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>

int main(int argc, char *argv[])
{
	if (argc != 3)
		return 1;

	QString inputFilename = QString::fromLatin1(argv[1]);
	QString outputFilename = QString::fromLatin1(argv[2]);

	QImage input = QImage(inputFilename);
	QImage output = input;
	output.save(outputFilename);

	return 0;
}
