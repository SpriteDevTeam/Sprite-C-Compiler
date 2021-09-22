struct Square {
    int length;
    int width;
};

int area(struct Square* s) {
    return s->length * s->width;
}

int main() {
    struct Square s;
    s.length = 5;
    s.width = 8;
    int a = area(&s);
    return 0;
}
