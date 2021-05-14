// Copyright 2020 Mathias Bynens. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
author: Mathias Bynens
description: >
  Unicode property escapes for `Script_Extensions=Nabataean`
info: |
  Generated by https://github.com/mathiasbynens/unicode-property-escapes-tests
  Unicode v13.0.0
esid: sec-static-semantics-unicodematchproperty-p
features: [regexp-unicode-property-escapes]
includes: [regExpUtils.js]
---*/

const matchSymbols = buildString({
  loneCodePoints: [],
  ranges: [
    [0x010880, 0x01089E],
    [0x0108A7, 0x0108AF]
  ]
});
testPropertyEscapes(
  /^\p{Script_Extensions=Nabataean}+$/u,
  matchSymbols,
  "\\p{Script_Extensions=Nabataean}"
);
testPropertyEscapes(
  /^\p{Script_Extensions=Nbat}+$/u,
  matchSymbols,
  "\\p{Script_Extensions=Nbat}"
);
testPropertyEscapes(
  /^\p{scx=Nabataean}+$/u,
  matchSymbols,
  "\\p{scx=Nabataean}"
);
testPropertyEscapes(
  /^\p{scx=Nbat}+$/u,
  matchSymbols,
  "\\p{scx=Nbat}"
);

const nonMatchSymbols = buildString({
  loneCodePoints: [],
  ranges: [
    [0x00DC00, 0x00DFFF],
    [0x000000, 0x00DBFF],
    [0x00E000, 0x01087F],
    [0x01089F, 0x0108A6],
    [0x0108B0, 0x10FFFF]
  ]
});
testPropertyEscapes(
  /^\P{Script_Extensions=Nabataean}+$/u,
  nonMatchSymbols,
  "\\P{Script_Extensions=Nabataean}"
);
testPropertyEscapes(
  /^\P{Script_Extensions=Nbat}+$/u,
  nonMatchSymbols,
  "\\P{Script_Extensions=Nbat}"
);
testPropertyEscapes(
  /^\P{scx=Nabataean}+$/u,
  nonMatchSymbols,
  "\\P{scx=Nabataean}"
);
testPropertyEscapes(
  /^\P{scx=Nbat}+$/u,
  nonMatchSymbols,
  "\\P{scx=Nbat}"
);

reportCompare(0, 0);
