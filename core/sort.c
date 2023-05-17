/*
 *   Copyright (c) 2023 Anton Kundenko <singaraiona@gmail.com>
 *   All rights reserved.

 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:

 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.

 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */

#include "sort.h"

null_t swap(i64_t *a, i64_t *b)
{
    i64_t t = *a;
    *a = *b;
    *b = t;
}

i64_t partition(i64_t array[], i64_t low, i64_t high)
{
    i64_t pivot = array[high];
    i64_t i = (low - 1);

    for (i64_t j = low; j <= high - 1; j++)
    {
        if (array[j] > pivot)
        {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    swap(&array[i + 1], &array[high]);
    return (i + 1);
}

null_t quick_sort(i64_t array[], i64_t low, i64_t high)
{
    if (low < high)
    {
        i64_t pi = partition(array, low, high);

        quick_sort(array, low, pi - 1);
        quick_sort(array, pi + 1, high);
    }
}

null_t heapify(i64_t arr[], i64_t n, i64_t i)
{
    i64_t largest = i;
    i64_t left = 2 * i + 1;
    i64_t right = 2 * i + 2;

    if (left < n && arr[left] < arr[largest])
        largest = left;

    if (right < n && arr[right] < arr[largest])
        largest = right;

    if (largest != i)
    {
        swap(&arr[i], &arr[largest]);
        heapify(arr, n, largest);
    }
}

null_t heap_sort(i64_t arr[], i64_t n)
{
    for (i64_t i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    for (i64_t i = n - 1; i >= 0; i--)
    {
        swap(&arr[0], &arr[i]);
        heapify(arr, i, 0);
    }
}

#define MIN_MERGE 32

null_t insertion_sort(i64_t arr[], i64_t left, i64_t right)
{
    for (i64_t i = left + 1; i <= right; i++)
    {
        i64_t temp = arr[i];
        i64_t j = i - 1;
        while (j >= left && arr[j] < temp)
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

null_t merge(i64_t arr[], i64_t l, i64_t m, i64_t r)
{
    i64_t len1 = m - l + 1, len2 = r - m;
    i64_t left[len1], right[len2];
    for (i64_t i = 0; i < len1; i++)
        left[i] = arr[l + i];
    for (i64_t i = 0; i < len2; i++)
        right[i] = arr[m + 1 + i];

    i64_t i = 0;
    i64_t j = 0;
    i64_t k = l;

    while (i < len1 && j < len2)
    {
        if (left[i] >= right[j])
        {
            arr[k] = left[i];
            i++;
        }
        else
        {
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    while (i < len1)
    {
        arr[k] = left[i];
        k++;
        i++;
    }

    while (j < len2)
    {
        arr[k] = right[j];
        k++;
        j++;
    }
}

null_t tim_sort(i64_t arr[], i64_t n)
{
    for (i64_t i = 0; i < n; i += MIN_MERGE)
        insertion_sort(arr, i, (i + MIN_MERGE - 1) < (n - 1) ? (i + MIN_MERGE - 1) : (n - 1));

    for (i64_t size = MIN_MERGE; size < n; size = 2 * size)
    {
        for (i64_t left = 0; left < n; left += 2 * size)
        {
            i64_t mid = left + size - 1;
            i64_t right = (left + 2 * size - 1) < (n - 1) ? (left + 2 * size - 1) : (n - 1);

            if (mid < right)
                merge(arr, left, mid, right);
        }
    }
}

null_t rf_sort(rf_object_t *vec)
{
    i64_t len = vec->adt->len;
    i64_t *v = as_vector_i64(vec);

    quick_sort(v, 0, len - 1);
    // quick_sort(v, 0, len - 1);
    // heap_sort(v, len);
    // tim_sort(v, len);
}
