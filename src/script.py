import re
st = set()
WORD_DELIMS = r"[,./|\\!?\"\';: \n\r\t()»—«”“’]";

count = 0
with open("../texts/usa.txt", "r") as f:
    for row in f:
        for word in re.split(WORD_DELIMS, row):
            count += 1
            st.add(word)
for i in st:
    print(i)
print(len(st))
print(count)