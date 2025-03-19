package khaser.lama.nodes.cfg.pat_match;

import khaser.lama.nodes.LamaScopeNode;

public final class LamaCaseBranch {
    public LamaPattern pat;
    public LamaScopeNode scope;

    public LamaCaseBranch(LamaPattern pat, LamaScopeNode scope) {
        this.pat = pat;
        this.scope = scope;
    }
}
