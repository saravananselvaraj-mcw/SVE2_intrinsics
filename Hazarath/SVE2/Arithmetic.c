//Vector Arithmetic Implementations of SVE2
#include <stdio.h>
#include <arm_sve.h>
int main()
{
    int size = 10;
    int Array1[10]={10,20,30,40,50,60,70,80,90,100};
    int Array2[10]={1,2,3,4,5,6,7,8,9,10};
    int Add_Res[10], Sub_Res[10], Mul_Res[10];
    /*Vector Implementations of Arithmetic functions
    svcntb -> Returns number of bytes a vector can hold.
    */
    int s = svcntb()/4;
    for(int i=0;i<size;i+=s)
    {
        // Calculate the predicate to specify number of active lanes in vector.
        svbool_t p = svwhilelt_b32(i,size);
        // Load chunks of data from input arrays
        svint32_t vec1 = svld1_s32(p,&Array1[i]);
        svint32_t vec2 = svld1_s32(p,&Array2[i]);
        // Add the two vectors together
        //There are multiple extensions: x->Leave the lanes , M->Merging and z->zeroing
        svint32_t Add_result = svadd_s32_x(p,vec1,vec2);
        // Subtract the two vectors together
        svint32_t Sub_result = svsub_s32_x(p,vec1,vec2);
        // Multiply the two vectors together
        svint32_t Mul_result = svmul_s32_x(p,vec1,vec2);
        // Store the result back to the output array
        svst1_s32(p, &Add_Res[i], Add_result);
        svst1_s32(p, &Sub_Res[i], Sub_result);
        svst1_s32(p, &Mul_Res[i], Mul_result);
    }
    printf("Addition Results:\n");
    for(int i=0;i<size;i++)
    {
        printf("%d ",Add_Res[i]);
    }
    printf("\nSubtraction Results:\n");
    for(int i=0;i<size;i++)
    {
        printf("%d ",Sub_Res[i]);
    }
    printf("\nMultiplication Results:\n");
    for(int i=0;i<size;i++)
    {
        printf("%d ",Mul_Res[i]);
    }
    printf("\n");
    return 0;
}
