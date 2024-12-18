#include <iostream>

int t[100], d[100];

void push(int l, int r, int v) {
    t[v] += (r - l) * d[v];

    if (l + 1 != r) {
        d[2*v] += d[v];
        d[2*v + 1] += d[v];
    }
    d[v] = 0;
}

void upd(int ql, int qr, int x, int l, int r, int v) {
    push(l, r, v);
    if (r <= ql or qr <= l) return;
    if (ql <= l and r <= qr) {
        d[v] = x;
        push(l, r, v);
    }

    int m = (l + r) / 2;
    upd(ql, qr, x, l, m, 2*v);
    upd(ql, qr, x, m, r, 2*v + 1);
    t[v] = t[2*v] + t[2*v + 1];
}

int get(int ql, int qr, int l, int r, int v) {
    push(l, r, v);
    if (r <= ql or qr <= l) return 0;
    if (ql <= l and r <= qr) return t[v];

    int m = (l + r) / 2;
    return get(ql, qr, l, m, 2*v) + get(ql, qr, m, r, 2*v + 1);
}

int main() {
    int n;
    std::cin >> n;

    int q;
    std::cin >> q;

    for (int i = 0; i < q; ++i) {
        int c;
        std::cin >> c;

        int l, r;
        std::cin >> l >> r;

        if (c == 1) {
            int x;
            std::cin >> x;

            upd(l - 1, r, x, 0, n, 1);
        } else {
            std::cout << get(l - 1, r, 0, n, 1) << std::endl;
        }
    }

    return 0;
}