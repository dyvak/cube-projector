#include <algorithm>
#include <stdlib.h>

#include <QDebug>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>

int main(int argc, char *argv[])
{
	if (argc != 4) {
		qDebug() << "Wrong number of args";
		return 1;
	}

	QString inputFilename = QString::fromLatin1(argv[1]);
	QString outputFilename = QString::fromLatin1(argv[2]);
	int size = atoi(argv[3]);

	// read
	QImageReader reader{inputFilename};
	QImage input = reader.read();
	QImage::Format imageFormat = reader.imageFormat();
	QByteArray format = QImageReader::imageFormat(inputFilename);

	QImage output[6];
	for (int i = 0; i < 6; ++i) {
		output[i] = QImage(size, size, imageFormat);
	}

	// process


	// write
	for (int i = 0; i < 6; ++i) {
		QImageWriter writer{outputFilename + QString::number(i) + "." + QString::fromLatin1(format).toLower()};
		writer.setFormat(format);
	//	writer.setQuality(50);
	//	writer.setCompression(0);
		writer.write(output[i]);
	}

	return 0;
}
