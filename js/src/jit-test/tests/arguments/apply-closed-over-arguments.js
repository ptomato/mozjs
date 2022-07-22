function bar(x,y) {
    return x + y;
}

function foo(x, y) {
    function closeOver() { return x; }
    return bar.apply({}, arguments);
}

var sum = 0;
for (var i = 0; i < 100; i++) {
    sum += foo(1,2);
}
assertEq(sum, 300)
