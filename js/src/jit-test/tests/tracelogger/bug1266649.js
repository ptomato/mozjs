
var du = new Debugger();
if (typeof du.setupTraceLogger === "function") {
    du.setupTraceLogger({
        Scripts: true
    })
    oomTest(() => function(){});
}
