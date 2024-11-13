func int main() {
    int n = 5;
    for (int i = 0; i < n; ++i) {
        i;
    }

    for (int i = 0, j = 0; i < n and j < 8; ++i, ++j) {
        i;
        j;
    }

    int i = 0;
    while (i < n) {
        ++i;
    }

    while (true) {
        if (i == 0) {
            break;
        }
    }

    while (true) {
        if (i == 0) break;
    }

    float h = 5.5;
    for (;; ++h) {
        if (h == 0) {
            break;
        } else {
            continue;
        }
    }
}
