/*
 * Copyright 2023 Axel Waggershauser
 */
// SPDX-License-Identifier: Apache-2.0

#include "BarcodeFormat.h"
#include "CreateBarcode.h"
#include "WriteBarcode.h"

#include <QDebug>
#include <QImage>

namespace ZXingQt {

QImage WriteBarcode(QStringView text, ZXing::Symbology symbology)
{
	using namespace ZXing;

	auto barcode = CreateBarcodeFromText(text.toString().toStdString(), symbology);
	auto bitmap = WriteBarcodeToImage(barcode);

	return QImage(bitmap.data(), bitmap.width(), bitmap.height(), bitmap.width(), QImage::Format::Format_Grayscale8).copy();
}

} // namespace ZXingQt

int main(int argc, char* argv[])
{
	if (argc != 4) {
		qDebug() << "usage: ZXingQtWriter <format> <text> <filename>";
		return 1;
	}

	auto symbology = ZXing::Symbology(argv[1]);
	auto text = QString(argv[2]);
	auto filename = QString(argv[3]);

	auto result = ZXingQt::WriteBarcode(text, symbology);

	result.save(filename);

	return 0;
}
