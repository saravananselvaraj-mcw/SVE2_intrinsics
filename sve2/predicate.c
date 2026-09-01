#include <stdio.h>
#include <stdint.h>
#include <arm_sve.h>

int main()
{
    float data[6] = {10, 20, 30, 40, 50, 60};
    int n = 6;
    int vl = svcntw(); // returns the number of 32-bit elements that can be processed in one vector operation

    for (int i = 0; i < n; i += vl)
    {
        svbool_t pred = svwhilelt_b32_s32(i, n); // predicate for active lanes
        svfloat32_t v = svld1_f32(pred, &data[i]);

        int32_t mask[vl];
        float vals[vl];

        svst1_s32(svptrue_b32(), mask, svsel_s32(pred, svdup_s32(1), svdup_s32(0)));
        svst1_f32(svptrue_b32(), vals, v);

        printf("i=%d predicate: ", i);
        for (int j = 0; j < vl; j++)
            printf("%s ", mask[j] ? "T" : "F");

        printf("\n  loaded:    ");
        for (int j = 0; j < vl; j++)
            mask[j] ? printf("%.0f ", vals[j]) : printf("-- ");
        printf("\n");
    }

    return 0;
}