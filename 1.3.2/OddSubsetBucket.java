import java.util.ArrayList;

public class OddSubsetBucket {
    private static final int MAX_SUM = 59 + 57 + 55 + 53 + 51 + 49;

    private ArrayList<Subset>[][][][] buckets;

    @SuppressWarnings({ "Unchecked", "unchecked" })
    public OddSubsetBucket() {
        buckets = new ArrayList[MAX_SUM + 1][4][3][2];

        for (int sum = 0; sum <= MAX_SUM; sum++) {
            for (int exp3 = 0; exp3 <= 3; exp3++) {
                for (int exp5 = 0; exp5 <= 2; exp5++) {
                    for (int exp7 = 0; exp7 <= 1; exp7++) {
                        buckets[sum][exp3][exp5][exp7] = new ArrayList<>();
                    }
                }
            }
        }
    }

    public void add(Subset subset) {
        int sum = subset.getSum();
        int[] exp = subset.getExponents();

        int exp3 = Math.min(exp[1], 3);
        int exp5 = Math.min(exp[2], 2);
        int exp7 = Math.min(exp[3], 1);

        buckets[sum][exp3][exp5][exp7].add(subset);
    }

    public ArrayList<Subset> get(int sum, int exp3, int exp5, int exp7) {

        if (sum < 0 || sum > MAX_SUM) {
            return new ArrayList<>();
        }

        return buckets[sum][exp3][exp5][exp7];
    }
}
