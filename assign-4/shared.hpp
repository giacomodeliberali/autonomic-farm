#pragma once

auto mapFun = ([](auto input) {
    long i = 0;
    while (i++ < 1000)
    {
        // simulate work
    }
    return new pair(
        input * 2, // double the number
        input % 8  // even and odd numbers
    );
});

auto reduceFun = ([](auto left, auto right) {
    return left + right; // sum the event and odd numbers
});

auto hashFun = ([](auto key) {
    return key;
});