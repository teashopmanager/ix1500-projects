# 3.1
U = set(range(1, 31))  # skapar en mängd med elementen 1-30
k = 7  # antalet element som ska finnas i varje delmängd
sumOfElement = 105  # vad summan av alla element i en delmängd ska bli


# funktionen som hittar alla giltiga delmängder
def findSubsets(U, k, sumOfElement):
    subsets = []
    U_list = list(U)

    def generateSubset(current, start, currentSum):
        if currentSum > sumOfElement:
            return

        if len(current) == k:
            if currentSum == sumOfElement:
                even = sum(x % 2 == 0 for x in current)
                odd = k - even

                product = 1
                for x in current:
                    product *= x

                if (even == 3 or odd == 3) and product % 360 == 0:
                    subsets.append(set(current))
            return

        for i in range(start, len(U_list)):
            current.append(U_list[i])
            generateSubset(current, i + 1, currentSum + U_list[i])
            current.pop()

    generateSubset([], 0, 0)

    return subsets


validSubsets = findSubsets(U, k, sumOfElement)

print("Number of valid subsets: ", len(validSubsets))
# print("Valid subsets:")

# for subset in validSubsets:
#    print(validSubsets)
