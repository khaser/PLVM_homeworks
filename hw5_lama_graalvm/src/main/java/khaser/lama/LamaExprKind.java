// import khaser.lama.LamaContext;
//
// public sealed class LamaExprKind permits LamaVal, LamaRef {
//     public abstract int resolveVal(LamaContext cont);
// }
//
// public final class LamaVal {
//     public int value;
//
//     public int resolveVal(LamaContext cont) {
//         return value;
//     }
// }
//
// public final class LamaRef {
//     public String sym;
//
//     public int resolveVal(LamaContext cont) {
//         return cont.globGet(cont);
//     }
// }
