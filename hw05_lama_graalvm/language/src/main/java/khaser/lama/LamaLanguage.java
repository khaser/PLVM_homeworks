package khaser.lama;

import com.oracle.truffle.api.CallTarget;
import com.oracle.truffle.api.TruffleLanguage;

import com.oracle.truffle.api.nodes.Node;
import khaser.lama.nodes.builtins.*;
import khaser.lama.nodes.funcs.LamaReadArgNode;
import khaser.lama.parser.LamaParser;
import khaser.lama.nodes.funcs.LamaFunctionRootNode;

@TruffleLanguage.Registration(id = LamaLanguage.ID,
                              name = "Lama")
public final class LamaLanguage extends TruffleLanguage<LamaContext> {
    public static final String ID = "lama";

    private static final LanguageReference<LamaLanguage> REF = LanguageReference.create(LamaLanguage.class);

    public static LamaLanguage get(Node node) {
        return REF.get(node);
    }

    public LamaLanguage() {
    }

    @Override
    protected CallTarget parse(ParsingRequest request) throws Exception {
        return LamaParser.parseLama(request.getSource().getReader());
    }

    @Override
    protected LamaContext createContext(Env env) {
        var ctx = new LamaContext(env);
        var rootFrame = new LamaFrame(null);
        ctx.curFrame = rootFrame;

        var readFunc = new LamaFunctionRootNode(this, new LamaBuiltinReadNode (), rootFrame, ctx);
        ctx.defFun("read", readFunc.getCallTarget());
        var writeFunc = new LamaFunctionRootNode(this, new LamaBuiltinWriteNode(new LamaReadArgNode(0)), rootFrame, ctx);
        ctx.defFun("write", writeFunc.getCallTarget());
        var lengthFunc = new LamaFunctionRootNode(this, LamaBuiltinLengthNodeGen.create(new LamaReadArgNode(0)), rootFrame, ctx);
        ctx.defFun("length", lengthFunc.getCallTarget());
        var stringFunc = new LamaFunctionRootNode(this, LamaBuiltinStringNodeGen.create(new LamaReadArgNode(0)), rootFrame, ctx);
        ctx.defFun("string", stringFunc.getCallTarget());

        return ctx;
    }
}
