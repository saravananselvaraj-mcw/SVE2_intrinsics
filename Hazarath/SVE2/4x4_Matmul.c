//4x4 Sized Matmul implementation using SVE2
#include <stdio.h>
#include <arm_sve.h>

int main()
{
    int Mat1[4][4], Mat2[4][4], Res[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            Mat1[i][j] = i + j + 2;
            Mat2[i][j] = i + j + 4;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        //Calculate the predicate vector which is of type bool that helps in tail part computation
        svbool_t pg = svwhilelt_b32(0, 4);
        //Resultant vector is initially dulicated with 0's
        svint32_t res = svdup_s32(0);
        for (int j = 0; j < 4; j++)
        {
            //Broadcast the ith row jth element of first matrix in a vector using duplication
            svint32_t elej = svdup_s32(Mat1[i][j]);
            //Load the jth row in second matrix using vector load with predicate
            svint32_t rowj = svld1_s32(pg, &Mat2[j][0]);
            //Perform fused multiplication and addition to multiply vectors and add them to resultant with predicate
            res = svmla_s32_z(pg,res, elej, rowj);
        }
        //Store the computed ith row of resultant matrix in the resultant vector with predicate
        svst1_s32(pg, &Res[i][0], res);
    }
    //Print the output
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%d ", Mat2[i][j]);
        }
        if(i==1)
        {
            printf(" x ");
        }
        else
        {
            printf("   ");
        }
        for (int j = 0; j < 4; j++)
        {
            printf("%d ", Mat2[i][j]);
        }
        if(i==1)
        {
            printf(" = ");
        }
        else
        {
            printf("   ");
        }
        for (int j = 0; j < 4; j++)
        {
            printf("%d ", Res[i][j]);
        }
        printf("\n");
    }
    return 0;
}
