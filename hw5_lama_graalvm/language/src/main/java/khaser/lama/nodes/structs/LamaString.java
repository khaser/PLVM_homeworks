package khaser.lama.nodes.structs;

import com.oracle.truffle.api.CompilerDirectives;
import khaser.lama.LamaContext;

import java.util.Arrays;
import java.util.stream.Collectors;

public class LamaString extends LamaArray {
    public LamaString(String str) {
        super(str.chars().boxed().toArray());
    }

    @Override
    public String toString() {
        return "\""
                + Arrays.stream(els)
                    .map(el -> String.valueOf((char) ((int) LamaContext.unwrapRef(el))))
                    .collect(Collectors.joining())
                + "\"";
    }
}
