// Copyright 2026 Axel Waggershauser
// SPDX-License-Identifier: Apache-2.0

package zxingcpp

/*
#cgo !zxing_local pkg-config: zxing

#cgo zxing_local CFLAGS: -I${SRCDIR}/../../../core/src -I${SRCDIR}/../../../build/core
#cgo zxing_local LDFLAGS: -L${SRCDIR}/../../../build/core -lZXing
#cgo zxing_local,darwin LDFLAGS: -Wl,-rpath,${SRCDIR}/../../../build/core
#cgo zxing_local,linux LDFLAGS: -Wl,-rpath,${SRCDIR}/../../../build/core

#include "zxingcgo.h"
*/
import "C"

// Version returns the zxing-cpp version string of the linked native library.
func Version() string {
	return C.GoString(C.ZXing_Version())
}
