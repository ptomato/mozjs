// Copyright 2021 Mathias Bynens. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
author: Mathias Bynens
description: >
  Unicode property escapes for `Script_Extensions=Gurmukhi`
info: |
  Generated by https://github.com/mathiasbynens/unicode-property-escapes-tests
  Unicode v14.0.0
esid: sec-static-semantics-unicodematchproperty-p
features: [regexp-unicode-property-escapes]
includes: [regExpUtils.js]
---*/

const matchSymbols = buildString({
  loneCodePoints: [
    0x000A3C,
    0x000A51,
    0x000A5E
  ],
  ranges: [
    [0x000951, 0x000952],
    [0x000964, 0x000965],
    [0x000A01, 0x000A03],
    [0x000A05, 0x000A0A],
    [0x000A0F, 0x000A10],
    [0x000A13, 0x000A28],
    [0x000A2A, 0x000A30],
    [0x000A32, 0x000A33],
    [0x000A35, 0x000A36],
    [0x000A38, 0x000A39],
    [0x000A3E, 0x000A42],
    [0x000A47, 0x000A48],
    [0x000A4B, 0x000A4D],
    [0x000A59, 0x000A5C],
    [0x000A66, 0x000A76],
    [0x00A830, 0x00A839]
  ]
});
testPropertyEscapes(
  /^\p{Script_Extensions=Gurmukhi}+$/u,
  matchSymbols,
  "\\p{Script_Extensions=Gurmukhi}"
);
testPropertyEscapes(
  /^\p{Script_Extensions=Guru}+$/u,
  matchSymbols,
  "\\p{Script_Extensions=Guru}"
);
testPropertyEscapes(
  /^\p{scx=Gurmukhi}+$/u,
  matchSymbols,
  "\\p{scx=Gurmukhi}"
);
testPropertyEscapes(
  /^\p{scx=Guru}+$/u,
  matchSymbols,
  "\\p{scx=Guru}"
);

const nonMatchSymbols = buildString({
  loneCodePoints: [
    0x000A04,
    0x000A29,
    0x000A31,
    0x000A34,
    0x000A37,
    0x000A3D,
    0x000A5D
  ],
  ranges: [
    [0x00DC00, 0x00DFFF],
    [0x000000, 0x000950],
    [0x000953, 0x000963],
    [0x000966, 0x000A00],
    [0x000A0B, 0x000A0E],
    [0x000A11, 0x000A12],
    [0x000A3A, 0x000A3B],
    [0x000A43, 0x000A46],
    [0x000A49, 0x000A4A],
    [0x000A4E, 0x000A50],
    [0x000A52, 0x000A58],
    [0x000A5F, 0x000A65],
    [0x000A77, 0x00A82F],
    [0x00A83A, 0x00DBFF],
    [0x00E000, 0x10FFFF]
  ]
});
testPropertyEscapes(
  /^\P{Script_Extensions=Gurmukhi}+$/u,
  nonMatchSymbols,
  "\\P{Script_Extensions=Gurmukhi}"
);
testPropertyEscapes(
  /^\P{Script_Extensions=Guru}+$/u,
  nonMatchSymbols,
  "\\P{Script_Extensions=Guru}"
);
testPropertyEscapes(
  /^\P{scx=Gurmukhi}+$/u,
  nonMatchSymbols,
  "\\P{scx=Gurmukhi}"
);
testPropertyEscapes(
  /^\P{scx=Guru}+$/u,
  nonMatchSymbols,
  "\\P{scx=Guru}"
);

reportCompare(0, 0);
