func int sort(array<int, 25> &arr) {
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

func int main() {
    array<int, 25> arr;
    for (int i = 0; i < 25; ++i) {
        scan(arr[i]);
    }

    sort(arr);

    for (int i = 0; i < 25; ++i) {
        print(arr[i]);
        print(" ");
    }
    print('\n');

    return 0;
}