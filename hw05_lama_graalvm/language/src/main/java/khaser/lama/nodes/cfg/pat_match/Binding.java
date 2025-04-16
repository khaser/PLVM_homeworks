package khaser.lama.nodes.cfg.pat_match;

public final class Binding {
    public String bindName;
    public Object value;

    public Binding(String bindName, Object value) {
        this.bindName = bindName;
        this.value = value;
    }
}
