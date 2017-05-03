#include <algorithm>
#include <stdlib.h>
#include <assert.h>

#include <QDebug>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>

void convertToCubemap(QImage &input, QImage output[6]) {
	assert(output[0].width() == output[0].height());

	int size = output[0].width();
	qDebug() << "image size: " << size;

	for (size_t i = 0; i < 6; ++i) {
		QRgb val = qRgb(120, 100, 0);

		for (int row = 0; row < size; ++row) {
			for (int col = 0; col < size; ++col) {
				output[i].setPixel(row, col, input.pixel(row, col));
			}
		}
	}
}

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
	convertToCubemap(input, output);

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
