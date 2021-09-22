int loop_test() {
    int x = 30;
    int i;
    int j;
    for (i = 0; i < x ; i++) {
        printf("loop test: i = %d\n", i);
        if (i < x - 1) continue;
        else {
            break;
        }
        for (j = 0; j < i; j++) printf("inner\n");
    }

    return x+i-j == 114514;
}
