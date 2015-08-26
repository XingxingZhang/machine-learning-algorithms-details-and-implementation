gender_detector is software that can detect if a Chinese name is a male name or a female name.

best features:
characters in a name without the first character (the first character is Family name, 
and it is probably not related to genders)

I tried to add unigram features, but it seems that they doesn't improve the results.
I also tried:
if a name is a single character name, e.g. Íõéª
if the last two characters of a name is the same, e.g. ¸ßÔ²Ô²
But they do not improve the results, either.
