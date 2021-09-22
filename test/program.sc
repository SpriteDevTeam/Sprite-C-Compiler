int add(int a, int b);

int main() {
    int a = 80;
    int b = 7;
    int c = add(a, b);
    printf("%d + %d = %d", a, b, c);
    return 0;
}

int add(int a, int b) {
    return a + b;
}
