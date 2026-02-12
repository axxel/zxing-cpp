// Copyright 2026 Axel Waggershauser
// SPDX-License-Identifier: Apache-2.0

package zxingcpp

/*
#include <stdlib.h>
#include "zxingcgo.h"
*/
import "C"

import (
	"runtime"
	"unsafe"
)

func (f BarcodeFormat) Symbology() BarcodeFormat {
	return BarcodeFormat(C.ZXing_BarcodeFormatSymbology(C.ZXing_BarcodeFormat(f)))
}

func (f BarcodeFormat) String() string {
	return zxString2Go(C.ZXing_BarcodeFormatToString(C.ZXing_BarcodeFormat(f)))
}

func ParseBarcodeFormat(s string) (BarcodeFormat, error) {
	cs := C.CString(s)
	defer C.free(unsafe.Pointer(cs))

	fmtVal := BarcodeFormat(C.ZXing_BarcodeFormatFromString(cs))
	if fmtVal == BarcodeFormatInvalid {
		return BarcodeFormatInvalid, zxLastError()
	}
	return fmtVal, nil
}

func ListBarcodeFormats(filter BarcodeFormat) []BarcodeFormat {
	var count C.int
	ptr := C.ZXing_BarcodeFormatsList(C.ZXing_BarcodeFormat(filter), &count)
	if ptr == nil {
		panic("ZXing_BarcodeFormatsList failed")
	}
	return zxBarcodeFormats2Go(ptr, count)
}

func ParseBarcodeFormats(s string) ([]BarcodeFormat, error) {
	cs := C.CString(s)
	defer C.free(unsafe.Pointer(cs))

	var count C.int
	ptr := C.ZXing_BarcodeFormatsFromString(cs, &count)
	if ptr == nil {
		return nil, zxLastError()
	}
	return zxBarcodeFormats2Go(ptr, count), nil
}

func BarcodeFormatsString(formats []BarcodeFormat) string {
	if len(formats) == 0 {
		return ""
	}
	ptr := C.ZXing_BarcodeFormatsToString(
		(*C.ZXing_BarcodeFormat)(unsafe.Pointer(unsafe.SliceData(formats))),
		C.int(len(formats)),
	)
	runtime.KeepAlive(formats)
	if ptr == nil {
		panic("ZXing_BarcodeFormatsToString failed")
	}
	return zxString2Go(ptr)
}
