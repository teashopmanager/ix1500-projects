public class Subset {
    private int[] elements;
    private int sum;
    private int[] exponents; // {exp2, exp3, exp5, exp7}

    public Subset(int[] elements, int size) {

        this.elements = elements;
        this.sum = calculateSum();
        this.exponents = calculateExponents();

    }

    private int calculateSum() {
        int sum = 0;

        for (int x : elements) {
            sum += x;
        }

        return sum;
    }

    private int[] calculateExponents() {
        int[] primes = { 2, 3, 5, 7 };
        int[] exponents = { 0, 0, 0, 0 };

        for (int x : elements) {
            if (x == 0)
                continue;
            for (int i = 0; i < 4; i++) {
                while (x % primes[i] == 0) {
                    exponents[i]++;
                    x /= primes[i];
                }
            }
        }

        return exponents;
    }

    public int[] getElements() {
        return elements;
    }

    public int getSum() {
        return sum;
    }

    public int[] getExponents() {
        return exponents;
    }
}