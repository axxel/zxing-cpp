/*
* Copyright 2016 Nu-book Inc.
* Copyright 2016 ZXing authors
* Copyright 2022 Axel Waggershauser
*/
// SPDX-License-Identifier: Apache-2.0

#include "QRReader.h"

#include "BarcodeData.h"
#include "BinaryBitmap.h"
#include "ConcentricFinder.h"
#include "DecoderResult.h"
#include "DetectorResult.h"
#include "LogMatrix.h"
#include "QRDecoder.h"
#include "QRDetector.h"
#include "ReaderOptions.h"

#include <utility>

namespace ZXing::QRCode {

static BarcodeData readPure(const BitMatrix* binImg, const ReaderOptions& _opts)
{
	DetectorResult detectorResult;
	if (_opts.hasSymbology(Symbology::QRCode))
		detectorResult = DetectPureQR(*binImg);
	if (_opts.hasSymbology(Symbology::MicroQRCode) && !detectorResult.isValid())
		detectorResult = DetectPureMQR(*binImg);
	if (_opts.hasSymbology(Symbology::RMQRCode) && !detectorResult.isValid())
		detectorResult = DetectPureRMQR(*binImg);

	if (!detectorResult.isValid())
		return {};

	auto decoderResult = Decode(detectorResult.bits());
	auto symbology = detectorResult.bits().width() != detectorResult.bits().height() ? Symbology::RMQRCode
					 : detectorResult.bits().width() < 21                            ? Symbology::MicroQRCode
																					 : Symbology::QRCode;

	return MatrixBarcode(std::move(decoderResult), std::move(detectorResult), symbology);
}

void logFPSet(const FinderPatternSet& fps [[maybe_unused]])
{
#ifdef PRINT_DEBUG
	auto drawLine = [](PointF a, PointF b) {
		int steps = maxAbsComponent(b - a);
		PointF dir = bresenhamDirection(PointF(b - a));
		for (int i = 0; i < steps; ++i)
			log(a + i * dir, 2);
	};

	drawLine(fps.bl, fps.tl);
	drawLine(fps.tl, fps.tr);
	drawLine(fps.tr, fps.bl);
#endif
}

BarcodesData Reader::read(const BinaryBitmap& image, int maxSymbols) const
{
	auto binImg = image.getBitMatrix();
	if (binImg == nullptr)
		return {};

#ifdef PRINT_DEBUG
	LogMatrixWriter lmw(log, *binImg, 5, "qr-log.pnm");
#endif

	if (_opts.isPure())
		return ToVector(readPure(binImg, _opts));

	auto allFPs = FindFinderPatterns(*binImg, _opts.tryHarder());

#ifdef PRINT_DEBUG
	printf("allFPs: %d\n", Size(allFPs));
#endif

	std::vector<ConcentricPattern> usedFPs;
	BarcodesData res;
	
	if (_opts.hasSymbology(Symbology::QRCode)) {
		auto allFPSets = GenerateFinderPatternSets(allFPs);
		for (const auto& fpSet : allFPSets) {
			if (Contains(usedFPs, fpSet.bl) || Contains(usedFPs, fpSet.tl) || Contains(usedFPs, fpSet.tr))
				continue;

			logFPSet(fpSet);

			auto detectorResult = SampleQR(*binImg, fpSet);
			if (detectorResult.isValid()) {
				auto decoderResult = Decode(detectorResult.bits());
				if (decoderResult.isValid()) {
					usedFPs.push_back(fpSet.bl);
					usedFPs.push_back(fpSet.tl);
					usedFPs.push_back(fpSet.tr);
				}
				if (decoderResult.isValid(_opts.returnErrors())) {
					res.emplace_back(MatrixBarcode(std::move(decoderResult), std::move(detectorResult), Symbology::QRCode));
					if (maxSymbols && Size(res) == maxSymbols)
						break;
				}
			}
		}
	}
	
	if (_opts.hasSymbology(Symbology::MicroQRCode) && !(maxSymbols && Size(res) == maxSymbols)) {
		for (const auto& fp : allFPs) {
			if (Contains(usedFPs, fp))
				continue;

			auto detectorResult = SampleMQR(*binImg, fp);
			if (detectorResult.isValid()) {
				auto decoderResult = Decode(detectorResult.bits());
				if (decoderResult.isValid(_opts.returnErrors())) {
					res.emplace_back(MatrixBarcode(std::move(decoderResult), std::move(detectorResult), Symbology::MicroQRCode));
					if (maxSymbols && Size(res) == maxSymbols)
						break;
				}

			}
		}
	}
	
	if (_opts.hasSymbology(Symbology::RMQRCode) && !(maxSymbols && Size(res) == maxSymbols)) {
		// TODO proper
		for (const auto& fp : allFPs) {
			if (Contains(usedFPs, fp))
				continue;

			auto detectorResult = SampleRMQR(*binImg, fp);
			if (detectorResult.isValid()) {
				auto decoderResult = Decode(detectorResult.bits());
				if (decoderResult.isValid(_opts.returnErrors())) {
					res.emplace_back(MatrixBarcode(std::move(decoderResult), std::move(detectorResult), Symbology::RMQRCode));
					if (maxSymbols && Size(res) == maxSymbols)
						break;
				}

			}
		}
	}

	return res;
}

} // namespace ZXing::QRCode
