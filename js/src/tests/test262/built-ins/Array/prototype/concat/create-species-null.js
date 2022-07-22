// Copyright (C) 2016 the V8 project authors. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.
/*---
esid: sec-array.prototype.concat
description: >
    A null value for the @@species constructor is interpreted as `undefined`
info: |
    1. Let O be ? ToObject(this value).
    2. Let A be ? ArraySpeciesCreate(O, 0).

    9.4.2.3 ArraySpeciesCreate

    [...]
    5. Let C be ? Get(originalArray, "constructor").
    [...]
    7. If Type(C) is Object, then
       a. Let C be ? Get(C, @@species).
       b. If C is null, let C be undefined.
    8. If C is undefined, return ? ArrayCreate(length).
features: [Symbol.species]
---*/

var a = [];
var result;

a.constructor = {};
a.constructor[Symbol.species] = null;

result = a.concat();

assert.sameValue(
  Object.getPrototypeOf(result),
  Array.prototype,
  'Object.getPrototypeOf(a.concat()) returns Array.prototype'
);
assert(Array.isArray(result), 'Array.isArray(result) must return true');

reportCompare(0, 0);
