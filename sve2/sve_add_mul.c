#include <arm_sve.h>
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    float a[6] = {10, 20, 30, 40, 50, 60};
    float b[6] = { 1,  2,  3,  4,  5,  6};
    float add[6];
    float mul[6];    

    int n = 6;
    int vl = svcntw();

    for (int i = 0; i < n; i += vl)
    {
        svbool_t pred = svwhilelt_b32_s32(i, n);   

        svfloat32_t va = svld1_f32(pred, &a[i]);
        svfloat32_t vb = svld1_f32(pred, &b[i]);

        //_x variants are used to specify that the operation should only be performed on active lanes, as determined by the predicate.
        // other variants are _m, _z
        // _m — merge: inactive lanes keep the value from the first input operand (they pass through unchanged).
        // _z — zero: inactive lanes are set to 0.
        //pred:        T    T    F    F
        // va:         10   20   30   40
        // vb:          1    2    3    4

        // _m result:  11   22   30   40   <- inactive lanes = va (first operand) passes through
        // _z result:  11   22    0    0   <- inactive lanes = 0
        svfloat32_t vadd = svadd_f32_x(pred, va, vb); // lane-wise a + b

        svfloat32_t vmul = svmul_f32_x(pred, va, vb); // lane-wise a * b

        svst1_f32(pred, &add[i], vadd);
        svst1_f32(pred, &mul[i], vmul);


        int32_t mask[vl];
        svst1_s32(svptrue_b32(), mask, svsel_s32(pred, svdup_s32(1), svdup_s32(0)));

        printf("i=%d predicate: ", i);
        for (int j = 0; j < vl; j++)
            printf("%s ", mask[j] ? "T" : "F");
        printf("\n");

    }

    printf("\nfinal add: ");
    for (int i = 0; i < n; i++)
        printf("%.0f ", add[i]);
    printf("\n");

    printf("final mul: ");
    for (int i = 0; i < n; i++)
        printf("%.0f ", mul[i]);
    printf("\n");

    return 0;
}