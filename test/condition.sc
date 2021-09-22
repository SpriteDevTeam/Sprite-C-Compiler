void condition_test() {
    int x = 10;
    int a;
    if (!~+-x) a = 1;
    else if (!x) {
        a = 2;
        if (x--) {
            a = 3;
        }
        else {
            if (x <<= (*&a) &= 2) a = 4;
        }
    }
    else a = 5;

    return;
}
