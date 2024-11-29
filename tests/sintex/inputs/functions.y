func int fa(int& i) {
    ++i;
}

func int h(int i) {
    i += 11;
}

func int a(int i, int j) {
    int cnt = 0;
    for (; i < 8; ++i) {
        for (; j < 8; ++j) {
            cnt += 1;
        }
    }
    return cnt;
}

func int main() {
    int i = 0;
    fa(i);

    h(0);
    h(i);

    int cnt = a(5, 6);
}

