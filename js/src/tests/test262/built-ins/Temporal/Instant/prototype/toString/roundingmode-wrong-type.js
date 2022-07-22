// |reftest| skip -- Temporal is not supported
// Copyright (C) 2021 Igalia, S.L. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
esid: sec-temporal.instant.prototype.tostring
description: Type conversions for roundingMode option
includes: [compareArray.js, temporalHelpers.js]
features: [Temporal]
---*/

const instant = new Temporal.Instant(1_000_000_000_123_987_500n);
TemporalHelpers.checkStringOptionWrongType("roundingMode", "trunc",
  (roundingMode) => instant.toString({ smallestUnit: "microsecond", roundingMode }),
  (result, descr) => assert.sameValue(result, "2001-09-09T01:46:40.123987Z", descr),
);

reportCompare(0, 0);
