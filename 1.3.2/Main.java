import java.util.ArrayList;

public class Main {
    public static void main(String[] args) {

        int[] even = new int[30];
        int[] odd = new int[30];

        for (int i = 0; i < 30; i++) {

            even[i] = 2 * (i + 1);
            odd[i] = 2 * i + 1;
        }

        ArrayList<ArrayList<Subset>> evenSubsets = generateEvenSubsets(even);

        OddSubsetBucket oddSubsets = genereateOddSubsets(odd);

        Result result = findSolutions(evenSubsets, oddSubsets);

        System.out.println("Total valid subsets: " + result.count);
        System.out.println("Lexicographically smallest: " + java.util.Arrays.toString(result.lexSmallest));
        System.out.println("Lexicographically largest: " + java.util.Arrays.toString(result.lexLargest));
        System.out.println("Min D(S): " + result.minD);
        System.out.println("Max D(S): " + result.maxD);
    }

    /**
     * Genererar alla delmängder med exakt fem element från den givna mängden av
     * jämna tal, och där produkten av elementen innehåller minst sex faktorer av 2.
     * 
     * Delmängderna grupperas efter deras elementsumma, där indexet i den yttre
     * listan motsvarar elementsumman.
     * 
     * @param subset array med jämna element som delmängderna ska bildas från
     * @return en lista av buckets där varje bucket innehåller delsummor med samma
     *         elementsumma
     * 
     */
    public static ArrayList<ArrayList<Subset>> generateEvenSubsets(int[] subset) {
        // Varje delmängd ska innehålla exakt 5 element
        // och elementprodukten ska minst innehålla 6 faktorer av 2
        final int TARGET_SIZE = 5;

        // Största möjliga summa för alla delmängder av E
        // Används för att skapa en bucket array.
        int maxSum = 60 + 58 + 56 + 54 + 52;

        // Skapar array som ska innehålla antalet faktorer av 2 för varje tal i subset
        // subset: {2,4,6,8,10}
        // exp2PerElement: {1,2,1,3,1}
        int[] exp2PerElement = new int[subset.length];
        for (int i = 0; i < subset.length; i++) {
            exp2PerElement[i] = counterFactorOfTwo(subset[i]);
        }

        // Beräknar maximalt antal återstående faktorer av 2 för att kunna beskära
        // sökningen
        int[] maxRemainingExp2 = new int[subset.length + 1];
        for (int i = subset.length - 1; i >= 0; i--) {
            maxRemainingExp2[i] = maxRemainingExp2[i + 1] + exp2PerElement[i];
        }

        // Skapar lista med buckets där index motsvarar elementsumman
        ArrayList<ArrayList<Subset>> buckets = new ArrayList<>();
        for (int i = 0; i <= maxSum; i++) {
            buckets.add(new ArrayList<>());
        }

        // Arrayen som innehåller den delmängd som byggs upp
        int[] current = new int[TARGET_SIZE];

        generateEvenHelper(subset, exp2PerElement, maxRemainingExp2, 0, 0, 0, 0, current, buckets);

        return buckets;
    }

    /**
     * 
     * Rekursiv hjälpfunktion som bygger delmängderna med fem element.
     * 
     * Sökningen beskärs när det inte finns tillräckligt med element kvar eller när
     * de återstående elementen inte kan ge tillräckligt många faktorer av 2.
     * 
     * @param subset           mängden av jämna tal
     * @param exp2PerElement   antal faktorer av 2 för varje element i subset
     * @param maxRemainingExp2 totalt antal faktorer av 2 som återstår från
     *                         respektive position
     * @param startIndex       från det index vi fortsätter välja
     * @param currentSize      antal element som vi hittlls har valt
     * @param currentSum       elementsumman av talen som hittils valde elementen
     * @param currentExp2      totalt antal faktorer av 2 i de valda elementen
     * @param current          delmängden som vi håller på att bygga
     * @param buckets          där färdiga delmängderna sparas efter sin summa
     */
    private static void generateEvenHelper(int[] subset, int[] exp2PerElement, int[] maxRemainingExp2, int startIndex,
            int currentSize, int currentSum, int currentExp2, int[] current, ArrayList<ArrayList<Subset>> buckets) {

        // Vi har hittat exakt 5 element, nu ska vi inte välja fler
        if (currentSize == 5) {

            // Vi kontrollerar om produkten innehåller minst 6 faktorer av 2
            if (currentExp2 >= 6) {
                Subset subsetResult = new Subset(current.clone(), currentSize);
                buckets.get(currentSum).add(subsetResult);
            }

            return;
        }

        // Om det inte finns tillräckligt många element att fylla delmängden
        // kan vi avsluta också.
        if (subset.length - startIndex < 5 - currentSize) {
            return;
        }

        // Om inte ens alla återstående faktorer av 2 räcker till 6
        // finns det ingen anledning att fortsätta på grenen.
        if (currentExp2 + maxRemainingExp2[startIndex] < 6) {
            return;
        }

        for (int i = startIndex; i < subset.length; i++) {
            current[currentSize] = subset[i];

            generateEvenHelper(subset, exp2PerElement, maxRemainingExp2, i + 1, currentSize + 1, currentSum + subset[i],
                    currentExp2 + exp2PerElement[i],
                    current, buckets);
        }

    }

    /**
     * Genererar alla delmängder med exakt sex element från den givna udda mängden.
     * Delmängderna lagras i en {@link OddSubsetBucket} och grupperas utefter
     * elementsumman och antal faktorer av 3,5 och 7.
     * 
     * @param subset en array med de udda talen som delmängderna ska skapas från
     * @return en bucketstruktur som innehåller de lagrade delmängderna
     */
    public static OddSubsetBucket genereateOddSubsets(int[] subset) {
        final int TARGET_SIZE = 6;

        OddSubsetBucket buckets = new OddSubsetBucket();

        int[] current = new int[TARGET_SIZE];

        genereateOddHelper(subset, 0, 0, 0, current, buckets);

        return buckets;
    }

    /**
     * Rekursiv hjälpfunktion som bygger delmängden med sex udda element.
     * 
     * @param subset      mängden av udda tal
     * @param startIndex  indexet vilket nästa element får väljas från
     * @param currentSize antal element som hittills har valts
     * @param currentSum  summan av de hittills valda elementen
     * @param current     array som innehåller den delmängd som håller på att byggas
     * @param buckets     en struktur där de färdiga udda delmängderna lagras.
     */
    private static void genereateOddHelper(int[] subset, int startIndex,
            int currentSize, int currentSum, int[] current, OddSubsetBucket buckets) {

        if (currentSize == 6) {
            Subset oddSubset = new Subset(current.clone(), currentSize);
            buckets.add(oddSubset); // Läggs i en följande struktur bucket[sum][exp3][exp5][exp7]
            return;
        }

        if (subset.length - startIndex < 6 - currentSize) {
            return;
        }

        /**
         * Vi vet att minsta jämna summan är 30:
         * 2 + 4 + 6 + 8 + 10 = 30
         * 
         * En uddadelmängd med elementsumma > 300 kan inte paras ihop med en jämna
         * delmängd
         * utan att överstiga 330
         * 
         */
        if (currentSum > 300) {
            return;
        }

        for (int i = startIndex; i < subset.length; i++) {
            current[currentSize] = subset[i];
            genereateOddHelper(subset, i + 1, currentSize + 1, currentSum + subset[i], current, buckets);
        }
    }

    /**
     * Söker efter alla giltiga delmängder genom att kombinera jämna och udda
     * delmängder vars elementsumma tillsammans blir 330
     * 
     * För varje jämna delmängd bestäms även hur många ytterligare faktorer av 3,5
     * och 7 som krävs från den udda delmängden
     * 
     * @param evenBuckets jämna delmängder grupperade efter elementsumma
     * @param oddBuckets  udda delmängder grupperade efter elementsumma och faktorer
     * @return ett Result-objekt med totalt antal giltiga lösningar, lexikografiska
     *         största/minsta samt minsta/största D(S)
     */
    public static Result findSolutions(ArrayList<ArrayList<Subset>> evenBuckets, OddSubsetBucket oddBuckets) {
        Result result = new Result();

        // OBS: börjar på 30 eftersom minsta möjliga element summa är 30, 1-29 är tomma.
        for (int evenSum = 30; evenSum < evenBuckets.size(); evenSum++) {

            // Tar alla delmängder med summan evenSum och sparar i en currentEvenBucket
            ArrayList<Subset> currentEvenBucket = evenBuckets.get(evenSum);
            int oddSum = 330 - evenSum;

            //
            if (oddSum < 0 || oddSum > 324) {
                continue;
            }

            // Kontrollerar hur många faktorer av 3,5,7 vi minst måste ha
            for (Subset evenSubset : currentEvenBucket) {
                int[] evenExp = evenSubset.getExponents();
                int need3 = Math.max(0, 3 - evenExp[1]);
                int need5 = Math.max(0, 2 - evenExp[2]);
                int need7 = Math.max(0, 1 - evenExp[3]);

                // Kollar bara de exponentBuckets som har tillräckligt med exponenter
                for (int exp3 = need3; exp3 <= 3; exp3++) {

                    for (int exp5 = need5; exp5 <= 2; exp5++) {

                        for (int exp7 = need7; exp7 <= 1; exp7++) {

                            // När vi kommit hit vet vi att alla lösningar i denna bucket är giltiga
                            ArrayList<Subset> validOddSubsets = oddBuckets.get(oddSum, exp3, exp5, exp7);

                            for (Subset oddSubset : validOddSubsets) {
                                processSolution(evenSubset, oddSubset, result);
                            }
                        }
                    }
                }
            }
        }
        return result;
    }

    /**
     * Behandlar en giltig kombination av en jämn och udda delmängd
     * 
     * Uppdaterar totalt antal lösningar, minsta och största värdet av D(S) samt den
     * lexikografiska största och minsta giltiga delmängden
     * 
     * @param even   den jämna delen av den giltiga delmängden
     * @param odd    den udda delen av den giltiga delmängden
     * @param result objektet där resultatet lagras
     */
    private static void processSolution(Subset even, Subset odd, Result result) {
        result.count++;

        // Om (O = 330 - E) => |E - O| = |E - (330- E)| = |2E - 330|
        int d = Math.abs(2 * even.getSum() - 330);

        if (d < result.minD) {
            result.minD = d;
        }
        if (d > result.maxD) {
            result.maxD = d;
        }

        int[] completeSubset = mergeSorted(even.getElements(), odd.getElements());
        if (result.lexSmallest == null
                || compareLexicographically(
                        completeSubset,
                        result.lexSmallest) < 0) {

            result.lexSmallest = completeSubset.clone();
        }

        if (result.lexLargest == null
                || compareLexicographically(
                        completeSubset,
                        result.lexLargest) > 0) {

            result.lexLargest = completeSubset.clone();
        }
    }

    /**
     * Slår ihop två sorterade arrayer till en gemensam sorterad arrray.
     * 
     * @param even sorterad array med jämna element
     * @param odd  sorterad array med udda element
     * @return en sorterad array som innehåller samtliga element
     */
    private static int[] mergeSorted(int[] even, int[] odd) {
        int[] result = new int[even.length + odd.length];
        int i = 0;
        int j = 0;
        int k = 0;

        while (i < even.length && j < odd.length) {
            if (even[i] < odd[j]) {
                result[k++] = even[i++];
            } else {
                result[k++] = odd[j++];
            }
        }

        while (i < even.length) {
            result[k++] = even[i++];
        }

        while (j < odd.length) {
            result[k++] = odd[j++];
        }

        return result;
    }

    /**
     * Jämför två arrayer lexikografiskt. Jämförelsen gör elementvis från vänster
     * till höger tills två olika element hittats.
     * 
     * @param a den första arrayen
     * @param b den andra arayen
     * @return ett negativt tal om b > a, ett positivt tal om a > b, annars 0.
     */
    private static int compareLexicographically(int[] a, int[] b) {

        for (int i = 0; i < a.length; i++) {

            if (a[i] < b[i]) {
                return -1;
            }

            if (a[i] > b[i]) {
                return 1;
            }
        }

        return 0;
    }

    /**
     * Beräknar hur många faktorer av 2 som förekommer i ett tal. Till exempel ger 8
     * resultatet 3 eftersom 8 = 2*2*2
     * 
     * @param x heltalet som ska undersökas
     * @return antalet faktorer av 2 i x
     */
    private static int counterFactorOfTwo(int x) {
        int count = 0;
        while (x % 2 == 0) {
            count++;
            x /= 2;
        }
        return count;
    }
}