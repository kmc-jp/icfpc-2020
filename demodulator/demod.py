import sys

# num[i:]を解釈し、その結果と解釈した部分の長さのペアを返す
def modulate(num, i):
    if num[i : i + 2] == "00":
        return None, 2
    elif num[i : i + 2] == "11":
        val1, l1 = modulate(num, i + 2)
        print(l1)
        val2, l2 = modulate(num, i + 2 + l1)
        return [val1, val2], l1 + l2 + 2
    result = 0
    if num[i : i + 2] == "01":
        result = 1
    else:
        result = -1
    n = num[i + 2 :].find("0")
    val = 0
    for j in range(4 * n):
        val *= 2
        if v[i + 2 + n + 1 + j] == "1":
            val += 1
    result *= val
    return result, 2 + 4 * n + n + 1


v = sys.argv[1]
result, _ = modulate(v, 0)
print(result)
