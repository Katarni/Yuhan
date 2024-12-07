#include <iostream>
#include <vector>

int sort(std::vector<int> &arr) {
    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 24; ++j) {
            if (arr[j] > arr[j + 1]) {
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
    return 0;
}

int main() {
    std::vector<int> arr(25);
    for (int i = 0; i < 25; ++i) {
        std::cin >> arr[i];
    }

    sort(arr);

    for (int i = 0; i < 25; ++i) {
        std::cout << arr[i] << ' ';
    }

    std::cout << std::endl;

    return 0;
}
