#include <stdlib.h>
#include <assert.h>
#include <algorithm>
#include <math.h>

#include <QDebug>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>

template <typename T>
T clip(const T &n, const T &lower, const T &upper) {
  return std::max(lower, std::min(n, upper));
}

struct vec3f
{
	float x, y, z;
};

QRgb interpolateXyzToColor(const vec3f &point, const QImage &image, int edge)
{
	int width = image.width();
	int height = image.height();

	double theta = atan2(point.y, point.x);// # range -pi to pi
	double r = hypot(point.x, point.y);
	double phi = atan2(point.z, r);// # range -pi/2 to pi/2

	// source img coords
	double uf = ( 2.0 *edge* (theta + M_PI)/ M_PI)*width/(4*edge);
	double vf = ( 2.0 *edge* (M_PI/2 - phi)/ M_PI)*width/(4*edge);
	// Use bilinear interpolation between the four surrounding pixels
	int ui = floor(uf);  //# coord of pixel to bottom left
	int vi = floor(vf);
	int u2 = ui+1;       //# coords of pixel to top right
	int v2 = vi+1;
	double mu = uf-ui;      //# fraction of way across pixel
	double nu = vf-vi;
	// Pixel values of four nearest corners
	QRgb A = {image.pixel(ui % width, clip(vi, 0, height - 1))};
	QRgb B = {image.pixel(u2 % width, clip(vi, 0, height - 1))};
	QRgb C = {image.pixel(ui % width, clip(v2, 0, height - 1))};
	QRgb D = {image.pixel(u2 % width, clip(v2, 0, height - 1))};
	// Interpolate color
	int rc = qRed  (A)*(1-mu)*(1-nu) + qRed  (B)*(mu)*(1-nu) + qRed  (C)*(1-mu)*nu+qRed  (D)*mu*nu;
	int gc = qGreen(A)*(1-mu)*(1-nu) + qGreen(B)*(mu)*(1-nu) + qGreen(C)*(1-mu)*nu+qGreen(D)*mu*nu;
	int bc = qBlue (A)*(1-mu)*(1-nu) + qBlue (B)*(mu)*(1-nu) + qBlue (C)*(1-mu)*nu+qBlue (D)*mu*nu;

	return qRgb(rc, gc, bc);
}

vec3f outImgCoordToXyz(int i, int j, int face, int size)
{
	float a = 2.0f * float(i) / size;
	float b = 2.0f * float(j) / size;

	switch (face) {
		case 0: // back
			return {-1.f, 1.f - a, 1.f - b};
		case 1: // left
			return {a, -1.f, 1.f - b};
		case 2: // front
			return {1.f, a - 1.f, 1.f - b};
		case 3: // right
			return {1.f - a, 1.f, 1.f -b};
		case 4: // top
			return {b - 1.f, a - 1.f, 1.f};
		case 5: // bottom
			return {1.f - b, a - 1.f, -1.f};
		default: // wrong face
			return {-1.f, -1.f, -1.f};
	}
}

void convertToCubemap(QImage &input, QImage output[6]) {
	assert(output[0].width() == output[0].height());

	int size = output[0].width();
	qDebug() << "image size: " << size;

	for (size_t face = 0; face < 6; ++face) {
		QRgb val = qRgb(120, 100, 0);

		for (int row = 0; row < size; ++row) {
			for (int col = 0; col < size; ++col) {
				vec3f xyz = outImgCoordToXyz(row, col, face, size);
				QRgb color = interpolateXyzToColor(xyz, input, size);
				output[face].setPixel(row, col, color);
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
