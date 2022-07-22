// |reftest| skip -- Temporal is not supported
// Copyright (C) 2021 Igalia, S.L. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
esid: sec-temporal.now
description: Tests that Temporal.Now meets the requirements for built-in objects
info: |
    Unless specified otherwise, a built-in object that is callable as a function is a built-in
    function object with the characteristics described in 10.3. Unless specified otherwise, the
    [[Extensible]] internal slot of a built-in object initially has the value true.

    Unless otherwise specified every built-in function and every built-in constructor has the
    Function prototype object [...] as the value of its [[Prototype]] internal slot.
features: [Temporal]
---*/

assert.sameValue(Object.isExtensible(Temporal.Now),
  true, "Built-in objects must be extensible.");

assert.sameValue(Object.prototype.toString.call(Temporal.Now),
  "[object Temporal.Now]", "Object.prototype.toString");

assert.sameValue(Object.getPrototypeOf(Temporal.Now),
  Object.prototype, "prototype");

assert.sameValue(Temporal.Now.prototype,
  undefined, "prototype property");

reportCompare(0, 0);
