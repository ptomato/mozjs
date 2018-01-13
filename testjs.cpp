#include "jsapi.h"
#include "js/Initialization.h"

static JSClassOps global_ops = {
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, JS_GlobalObjectTraceHook
};

/* The class of the global object. */
static JSClass global_class = {
    "global",
    JSCLASS_GLOBAL_FLAGS,
    &global_ops
};

int main(int argc, const char *argv[])
{
    JS_Init();

    JSContext *cx = JS_NewContext(8L * 1024 * 1024);
    if (!cx)
        return 1;
    if (!JS::InitSelfHostedCode(cx))
        return 1;

    {
        JSAutoRequest ar(cx);
        JS::CompartmentOptions options;
        JS::RootedObject global(cx, JS_NewGlobalObject(cx, &global_class,
            nullptr, JS::FireOnNewGlobalHook, options));
        if (!global)
            return 1;

        JS::RootedValue rval(cx);

        {
            JSAutoCompartment ac(cx, global);
            JS_InitStandardClasses(cx, global);

            const char *script = "'hello'+'world, it is '+new Date()";
            const char *filename = "noname";
            int lineno = 1;
            JS::CompileOptions opts(cx);
            opts.setFileAndLine(filename, lineno);
            if (!JS::Evaluate(cx, opts, script, strlen(script), &rval))
                return 1;
        }

        JSString *str = rval.toString();
        printf("%s\n", JS_EncodeString(cx, str));
    }

    JS_DestroyContext(cx);
    JS_ShutDown();
    return 0;
}
