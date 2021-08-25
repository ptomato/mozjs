#include <jsapi.h>
#include <js/CompilationAndEvaluation.h>
#include <js/Initialization.h>
#include <js/SourceText.h>

static JSClass global_class = {
    "global",
    JSCLASS_GLOBAL_FLAGS,
    &JS::DefaultGlobalClassOps,
};

int main(int argc, const char *argv[])
{
    JS_Init();

    JSContext *cx = JS_NewContext(8L * 1024 * 1024);
    if (!cx)
        return 1;
    if (!JS::InitSelfHostedCode(cx))
        return 1;

    JS::RealmOptions options;
    JS::RootedObject global(cx, JS_NewGlobalObject(cx, &global_class,
        nullptr, JS::FireOnNewGlobalHook, options));
    if (!global)
        return 1;

    JS::RootedValue rval(cx);

    {
        JSAutoRealm ar(cx, global);

        JS::CompileOptions opts(cx);
        opts.setFileAndLine("noname", 1);

        const char *code = R"js(
            `hello world, it is ${new Date()}`
        )js";

        JS::SourceText<mozilla::Utf8Unit> source;
        if (!source.init(cx, code, strlen(code), JS::SourceOwnership::Borrowed))
            return 1;

        if (!JS::Evaluate(cx, opts, source, &rval))
            return 1;
    }

    JSString *str = rval.toString();
    printf("%s\n", JS_EncodeStringToASCII(cx, str).get());

    JS_DestroyContext(cx);
    JS_ShutDown();
    return 0;
}
