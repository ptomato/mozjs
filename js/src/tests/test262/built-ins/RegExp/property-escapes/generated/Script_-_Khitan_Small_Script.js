// Copyright 2021 Mathias Bynens. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
author: Mathias Bynens
description: >
  Unicode property escapes for `Script=Khitan_Small_Script`
info: |
  Generated by https://github.com/mathiasbynens/unicode-property-escapes-tests
  Unicode v14.0.0
esid: sec-static-semantics-unicodematchproperty-p
features: [regexp-unicode-property-escapes]
includes: [regExpUtils.js]
---*/

const matchSymbols = buildString({
  loneCodePoints: [
    0x016FE4
  ],
  ranges: [
    [0x018B00, 0x018CD5]
  ]
});
testPropertyEscapes(
  /^\p{Script=Khitan_Small_Script}+$/u,
  matchSymbols,
  "\\p{Script=Khitan_Small_Script}"
);
testPropertyEscapes(
  /^\p{Script=Kits}+$/u,
  matchSymbols,
  "\\p{Script=Kits}"
);
testPropertyEscapes(
  /^\p{sc=Khitan_Small_Script}+$/u,
  matchSymbols,
  "\\p{sc=Khitan_Small_Script}"
);
testPropertyEscapes(
  /^\p{sc=Kits}+$/u,
  matchSymbols,
  "\\p{sc=Kits}"
);

const nonMatchSymbols = buildString({
  loneCodePoints: [],
  ranges: [
    [0x00DC00, 0x00DFFF],
    [0x000000, 0x00DBFF],
    [0x00E000, 0x016FE3],
    [0x016FE5, 0x018AFF],
    [0x018CD6, 0x10FFFF]
  ]
});
testPropertyEscapes(
  /^\P{Script=Khitan_Small_Script}+$/u,
  nonMatchSymbols,
  "\\P{Script=Khitan_Small_Script}"
);
testPropertyEscapes(
  /^\P{Script=Kits}+$/u,
  nonMatchSymbols,
  "\\P{Script=Kits}"
);
testPropertyEscapes(
  /^\P{sc=Khitan_Small_Script}+$/u,
  nonMatchSymbols,
  "\\P{sc=Khitan_Small_Script}"
);
testPropertyEscapes(
  /^\P{sc=Kits}+$/u,
  nonMatchSymbols,
  "\\P{sc=Kits}"
);

reportCompare(0, 0);
