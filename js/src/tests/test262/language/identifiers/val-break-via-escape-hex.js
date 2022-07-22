// |reftest| error:SyntaxError
// Copyright (C) 2015 the V8 project authors. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
es6id: 11.6
description: >
    SyntaxError expected: reserved words used as Identifier
    Names in UTF8: break
negative:
  phase: parse
  type: SyntaxError
---*/

$DONOTEVALUATE();

var \u{62}\u{72}\u{65}\u{61}\u{6b} = 123;;
