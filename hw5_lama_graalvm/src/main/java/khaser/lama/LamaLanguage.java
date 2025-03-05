package khaser.lama;

import com.oracle.truffle.api.CallTarget;
import com.oracle.truffle.api.TruffleLanguage;

import com.oracle.truffle.api.nodes.Node;
import khaser.lama.nodes.builtins.LamaBuiltinWriteNode;
import khaser.lama.nodes.funcs.LamaReadArgNode;
import khaser.lama.parser.LamaParser;
import khaser.lama.nodes.funcs.LamaFunctionRootNode;
import khaser.lama.nodes.builtins.LamaBuiltinReadNode;

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

        var readFunc = new LamaFunctionRootNode(this, new LamaBuiltinReadNode ());
        ctx.globFunSet("read", readFunc.getCallTarget());
        var writeFunc = new LamaFunctionRootNode(this, new LamaBuiltinWriteNode(new LamaReadArgNode(0)));
        ctx.globFunSet("write", writeFunc.getCallTarget());

        return ctx;
    }
}
